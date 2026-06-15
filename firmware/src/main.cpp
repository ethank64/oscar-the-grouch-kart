// Oscar the Grouch Kart - ESP32 drive controller
//
// Reads a Wii remote over Bluetooth (tilt = steer, buttons = gas/brake, held
// trigger = dead-man) and commands a dual VESC over UART in a differential-drive
// mix. See ../../docs/control-scheme.md and ../../docs/electrical.md.
//
// NOT YET COMPILE-TESTED on hardware. The values below are deliberately
// conservative starting points - bring it up wheels-off-the-ground and tune.
//
// Board: original ESP32 (ESP-WROOM-32). Needs Bluetooth Classic, which the
// S2/S3/C3 variants do NOT have - those will not work with the Wiimote.

#include <Arduino.h>
#include "ESP32Wiimote.h"
#include <VescUart.h>

// ===================== Configuration =====================

// --- VESC UART (ESP32 hardware Serial2) ---
static const uint32_t VESC_BAUD    = 115200;   // must match VESC Tool app baud
static const int      VESC_RX_PIN  = 16;       // ESP32 RX2  <- VESC TX
static const int      VESC_TX_PIN  = 17;       // ESP32 TX2  -> VESC RX
// The dual VESC = two units linked by CAN. We talk UART to the master and
// forward to the slave by its CAN id. Set this to the slave's id in VESC Tool
// (App Settings -> General -> CAN, "send status over CAN" enabled on both).
static const uint8_t  RIGHT_CAN_ID = 1;

// --- Drive feel / safety limits (START LOW, then raise while testing) ---
static const float MAX_MOTOR_CURRENT_A = 12.0f; // per-motor current at full gas
static const float BRAKE_CURRENT_A     = 8.0f;  // brake button current
static const float STEER_AUTHORITY     = 0.6f;  // 0..1, how hard tilt biases L/R
static const float SLEW_PER_SEC        = 2.5f;  // max change in normalized cmd/sec
                                                // (lower = gentler, no lurch)

// --- Wii remote tilt calibration (raw accel units; tune from debug print) ---
// Hold the remote sideways like a Wii Wheel. One accel axis swings with roll.
// Print the raw values (SERIAL DEBUG below), note level vs full-tilt, set these.
static const int   ACC_CENTER     = 128;   // raw reading held level
static const int   ACC_RANGE      = 70;    // raw delta at full usable tilt
static const float STEER_DEADZONE = 0.08f; // ignore tiny tilt around level
// If steering is reversed, flip this sign.
static const float STEER_SIGN     = 1.0f;

// --- Loop timing / failsafe ---
static const uint32_t CONTROL_HZ      = 50;    // control update rate
static const uint32_t LINK_TIMEOUT_MS = 250;   // no Wiimote data -> stop
static const bool     SERIAL_DEBUG    = true;  // print state to USB serial

// ===================== Globals =====================

ESP32Wiimote wiimote;
VescUart      vesc;

static float    g_left  = 0.0f;   // current normalized command, after slew (0..1)
static float    g_right = 0.0f;
static uint32_t g_lastWiimoteMs = 0;
static uint32_t g_lastControlMs = 0;
static uint32_t g_lastDebugMs   = 0;

// Latest decoded input
static bool  g_deadman  = false;
static bool  g_gas      = false;
static bool  g_brake    = false;
static float g_steer    = 0.0f;   // -1 (left) .. +1 (right)

// ===================== Helpers =====================

static float clampf(float v, float lo, float hi) {
  return v < lo ? lo : (v > hi ? hi : v);
}

// Move `current` toward `target` by at most `maxStep`.
static float slew(float current, float target, float maxStep) {
  float d = target - current;
  if (d >  maxStep) d =  maxStep;
  if (d < -maxStep) d = -maxStep;
  return current + d;
}

// Send a per-motor current command to both VESCs (master + CAN slave).
static void driveCurrent(float leftA, float rightA) {
  vesc.setCurrent(leftA);                 // master = left motor
  vesc.setCurrent(rightA, RIGHT_CAN_ID);  // slave  = right motor (CAN forward)
}

static void driveBrake(float brakeA) {
  vesc.setBrakeCurrent(brakeA);
  vesc.setBrakeCurrent(brakeA, RIGHT_CAN_ID);
}

// Everything off: coast. Also resets the ramp so re-arming starts from zero.
static void driveStop() {
  g_left = 0.0f;
  g_right = 0.0f;
  vesc.setCurrent(0.0f);
  vesc.setCurrent(0.0f, RIGHT_CAN_ID);
}

// ===================== Input decode =====================

static void readWiimote() {
  wiimote.task();
  if (wiimote.available() <= 0) return;

  g_lastWiimoteMs = millis();

  uint16_t btn   = wiimote.getButtonState();
  AccelState acc = wiimote.getAccelState();

  // Dead-man = back trigger (B). Gas = button 2. Brake = button 1.
  g_deadman = (btn & BUTTON_B);
  g_gas     = (btn & BUTTON_TWO);
  g_brake   = (btn & BUTTON_ONE);

  // Roll about the long axis -> steering. acc.yAxis is the usual swing axis when
  // the remote is held sideways; change to xAxis/zAxis if your hold differs.
  float s = STEER_SIGN * ((int)acc.yAxis - ACC_CENTER) / (float)ACC_RANGE;
  if (fabsf(s) < STEER_DEADZONE) s = 0.0f;
  g_steer = clampf(s, -1.0f, 1.0f);

  if (SERIAL_DEBUG && millis() - g_lastDebugMs > 200) {
    g_lastDebugMs = millis();
    Serial.printf("acc[%d,%d,%d] dead=%d gas=%d brk=%d steer=%.2f L=%.2f R=%.2f\n",
                  acc.xAxis, acc.yAxis, acc.zAxis,
                  g_deadman, g_gas, g_brake, g_steer, g_left, g_right);
  }
}

// ===================== Control step =====================

static void controlStep() {
  bool linkAlive = (millis() - g_lastWiimoteMs) < LINK_TIMEOUT_MS;

  // Failsafe: no remote, or dead-man released -> coast and bail.
  if (!linkAlive || !g_deadman) {
    driveStop();
    return;
  }

  // Brake button overrides throttle.
  if (g_brake) {
    g_left = 0.0f;
    g_right = 0.0f;
    driveBrake(BRAKE_CURRENT_A);
    return;
  }

  // Differential mix. Gas is digital (0 or 1); the slew limiter ramps it.
  float throttle = g_gas ? 1.0f : 0.0f;
  float targetL  = clampf(throttle + STEER_AUTHORITY * g_steer, 0.0f, 1.0f);
  float targetR  = clampf(throttle - STEER_AUTHORITY * g_steer, 0.0f, 1.0f);

  float maxStep = SLEW_PER_SEC / (float)CONTROL_HZ;
  g_left  = slew(g_left,  targetL, maxStep);
  g_right = slew(g_right, targetR, maxStep);

  driveCurrent(g_left * MAX_MOTOR_CURRENT_A, g_right * MAX_MOTOR_CURRENT_A);
}

// ===================== Arduino entry points =====================

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\nOscar Kart controller booting...");

  Serial2.begin(VESC_BAUD, SERIAL_8N1, VESC_RX_PIN, VESC_TX_PIN);
  vesc.setSerialPort(&Serial2);

  wiimote.init();
  Serial.println("Waiting for Wii remote (press 1+2 to pair)...");
}

void loop() {
  readWiimote();  // poll BT as fast as possible

  uint32_t now = millis();
  if (now - g_lastControlMs >= (1000UL / CONTROL_HZ)) {
    g_lastControlMs = now;
    controlStep();
  }
}
