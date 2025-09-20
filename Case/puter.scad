//
// car computer enclosure
//

mm = 25.4;
$fn = 32;
e = 0.1;

include <oled.scad>

//
// case body
//
body_w = 182;
body_h = 52;
body_d = 5*mm;
// body_d = 10;     /* fast print */

body_thk = 1.6;
body_lip = 2.0;

panel_w = body_w + 2*body_lip;
panel_h = body_h + 2*body_lip;

// draw box with lip, centered
module box() {
     translate( [-body_w/2, -body_h/2, -body_d])
     difference() {
	  union() {
	       // lip
	       translate( [-body_lip, -body_lip, body_d-body_thk])
		    cube( [body_w+2*body_lip, body_h+2*body_lip, body_thk]);
	       // box body
	       cube( [body_w, body_h, body_d]);
	  }
	  // cavity
	  translate( [body_thk, body_thk, body_thk])
	       cube( [body_w-2*body_thk, body_h-2*body_thk, body_d]);
	  echo("Cavity", body_w-2*body_thk, body_h-2*body_thk, body_d);
     }
}


// front panel, centered

module panel() {
     translate( [-panel_w/2, -panel_h/2, 10])
	  % cube( [panel_w, panel_h, body_thk]);
}

box();
translate( [0, 0, 5]) {
     difference() {
       	  panel();
     }
}


translate( [-161, 70, 10]) color("green") import("car-control.stl");
translate( [-13.5, 0.1, 13])
 oled();

// translate( [-15.3, -4,0])
// rotate([0,0,180])
//  color("red")import("Unnamed-MKRZero_V5.0.stl");

