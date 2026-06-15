// Oscar the Grouch Kart - parametric massing mockup
//
// Representative solids (not manufacturer-accurate parts) to check packaging,
// track width, and center of gravity inside the can. Units: millimeters.
// Render in OpenSCAD; F5 = preview, F6 = render, then Export as STL.
//
// NOT render-verified yet. MEASURE YOUR CAN and set the params below.

// ---------------- Parameters (measure + edit) ----------------
$fn = 64;

// Trash can (32-gal round, tapered). Measure yours; these are estimates.
can_h        = 711;   // ~28 in tall
can_r_bottom = 190;   // ~15 in dia at base
can_r_top    = 254;   // ~20 in dia at rim
can_wall     = 4;

// Drive wheels (10 in hub motors, 80/65-6 tire)
wheel_d      = 254;   // 10 in
wheel_w      = 80;    // tire width
track        = 360;   // center-to-center of the two drive wheels

// Casters (4 in), fore and aft
caster_d     = 102;
caster_reach = 150;   // distance fore/aft from center

// Base plate (deck the motors + casters + seat bolt to)
plate_r      = 180;
plate_t      = 12;

// Component boxes (massing only)
batt   = [248, 70, 107];  // 48V pack approx LxWxH
vesc   = [120, 70, 30];
elec   = [80, 55, 25];    // ESP32 + buck enclosure

seat_h       = 240;       // seat top above the deck
show_rider   = false;     // rough rider proxy for CoG sanity

// Derived
wheel_r  = wheel_d/2;
caster_r = caster_d/2;
deck_z   = wheel_r;       // deck sits at axle height

// ---------------- Modules ----------------
module hub_wheel() {
  color("#222")
  rotate([90,0,0]) cylinder(h=wheel_w, r=wheel_r, center=true);
}

module caster() {
  color("#444") {
    rotate([90,0,0]) cylinder(h=30, r=caster_r, center=true);
    translate([0,0,caster_r]) cube([40,40,40], center=true); // swivel stem block
  }
}

module base_plate() {
  color("#9a8") translate([0,0,deck_z]) cylinder(h=plate_t, r=plate_r, center=true);
}

module can() {
  color([0.15,0.5,0.2,0.18]) // semi-transparent green (preview only)
  difference() {
    cylinder(h=can_h, r1=can_r_bottom, r2=can_r_top);
    translate([0,0,-1])
      cylinder(h=can_h+2, r1=can_r_bottom-can_wall, r2=can_r_top-can_wall);
  }
}

module box(dim, col) { color(col) cube(dim, center=true); }

module seat() {
  color("#852") translate([0,0,deck_z + plate_t/2 + seat_h])
    cylinder(h=40, r=140, center=true);
}

module rider() {
  if (show_rider)
    color([0.8,0.6,0.5,0.25])
    translate([0,0,deck_z + plate_t/2 + seat_h + 250])
      cylinder(h=460, r=160, center=true);
}

// ---------------- Assembly ----------------
module assembly() {
  can();
  base_plate();

  // drive wheels, axle along Y
  translate([0,  track/2, wheel_r]) hub_wheel();
  translate([0, -track/2, wheel_r]) hub_wheel();

  // fore/aft casters
  translate([ caster_reach, 0, caster_r]) caster();
  translate([-caster_reach, 0, caster_r]) caster();

  // electronics + battery low on the deck (keep CoG down)
  translate([0, 0, deck_z + plate_t/2 + batt[2]/2]) box(batt, "#36c");
  translate([90, 0, deck_z + plate_t/2 + vesc[2]/2]) box(vesc, "#888");
  translate([-90,0, deck_z + plate_t/2 + elec[2]/2]) box(elec, "#2a2");

  seat();
  rider();
}

assembly();

// To export ONE part as STL, comment out assembly() above and call just that
// module, e.g.:  base_plate();
