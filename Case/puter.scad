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
// body_w = 182;

body_spc = 0.05*mm;

body_w = 6.875*mm-body_spc;
body_h = 2.125*mm-body_spc;
body_d = 2.5*mm;
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
	  // wiring hole
	  translate( [4*body_w/5, body_h/2, -e])
	       cylinder( d=0.375*mm, h=10);


	  echo("Cavity (mm)", body_w-2*body_thk, body_h-2*body_thk, body_d);
	  echo("Cavity (in)", (body_w-2*body_thk)/mm, (body_h-2*body_thk)/mm, body_d/mm);
     }
}


// front panel, centered

module panel() {
     translate( [-panel_w/2, -panel_h/2, 10])
	  cube( [panel_w, panel_h, body_thk]);
}

box();

// translate( [0, 0, 5]) {
//      difference() {
//        	  color("brown") panel();
// 	  translate( [-body_w+0.65*mm, 70, 0])
// 	       translate( [147.5, -69.7, 0]) oled_holes();
//      }
// }
// 
// 
// translate( [-body_w+0.65*mm, 70, 10]) {
//       color("green") import("car-control.stl");
//       translate( [147.5, -69.7, 3]) oled();
// }
