//
// car computer enclosure
//
// TODO:
// re-print with rear access door
//

mm = 25.4;
$fn = 32;
e = 0.1;

include <oled.scad>

module tri_prism( width, height, length) {
     polyhedron(
	  points = [ [0,0,0], [0,height,0], [width,0,0], 
		     [0,0,length], [width, 0, length], [0,height,length]],
	  faces = [ [0, 1, 2], [3, 4, 5], [0, 2, 4, 3],
		    [0, 3, 5, 1], [1, 5, 4, 2]],
	  convexity = 10);
}

//
// case body
//
// body_spc = 0.05*mm;
body_spc = 0.075*mm;

body_w = 6.875*mm-body_spc;
body_h = 2.125*mm-body_spc;

//--- normal
// body_d = 2.5*mm;
body_d = 3.25*mm;  // add room for USB connector

// easier if these are just hard-wired now
door_w = 148;
door_h = 46;
door_x = 12;
door_y = 3;

// pcb_standoff_down = 0.25*mm + 0.7*mm;   // as last printed
 pcb_standoff_down = 0.7*mm;	// proposed update remove standoffs

//--- fast print
//body_d = 10;     /* fast print */
//pcb_standoff_down = 2;

body_thk = 1.6;
body_lip = 2.0;

panel_w = body_w + 2*body_lip;
panel_h = body_h + 2*body_lip;

panel_w_in = body_w-2*body_thk;
panel_h_in = body_h-2*body_thk;

// PCB mounting holes
pcb_hole_dx = 6.25*mm;
pcb_hole_dy = 1.5*mm;
// pcb_hole_dia = 0.098*mm;	/* 4-40 tap drill */
//pcb_hole_dia = 0.140*mm;	/* 6-32 test fit */
pcb_hole_dia = 0.137*mm;	/* 6-32 test fit */
pcb_standoff_dia = 0.25*mm;


panel_hole_dia = 0.15*mm;

pcb_so_supp_wid = 3;
pcb_so_supp_len = pcb_hole_dia/2+6-body_spc/2;

module standoff_at( x, y, dia, hole, hgt, rot) {
    translate( [x, y, 0]) {
      rotate( [0, 0, rot]) {
      difference() {
	union() {
	  cylinder( d=dia, h=hgt);
	  translate( [-pcb_so_supp_wid/2, -pcb_so_supp_len, 0])
	    cube( [pcb_so_supp_wid, pcb_so_supp_len, hgt]);
	}
	translate( [0, 0, -e])
	  cylinder( d=hole, h=hgt+2*e);
      }
    }
  }
}

// draw a lip, angled for no support
module lip() {
     translate( [body_w, body_h-e, body_d])
	  rotate( [90, 180, 270])
	  tri_prism( body_lip, body_lip, body_w);
     translate( [0, e, body_d])
	  rotate( [90, 180, 90])
	  tri_prism( body_lip, body_lip, body_w);
     translate( [body_w-e, 0, body_d])
	  rotate( [90, 180, 180])
	  tri_prism( body_lip, body_lip, body_h);
     translate( [e, body_h, body_d])
	  rotate( [90, 180, 0])
	  tri_prism( body_lip, body_lip, body_h);
     
}

// draw box with lip, centered
module box() {
     translate( [-body_w/2, -body_h/2, -body_d])
     difference() {
	  union() {
	       // box body
	       echo("Box ", body_w, ", ", body_h);
	       cube( [body_w, body_h, body_d]);
	       color("red") lip();
//	       // lip
//	       translate( [-body_lip, -body_lip, body_d-body_thk])
//		    cube( [body_w+2*body_lip, body_h+2*body_lip, body_thk]);
	  }
	  // cavity
	  translate( [body_thk, body_thk, body_thk])
	       cube( [body_w-2*body_thk, body_h-2*body_thk, body_d]);
	  // wiring hole
	  translate( [4*body_w/5, body_h/2, -e]) cylinder( d=0.375*mm, h=10);
	  // rear access
	  translate( [door_x, door_y, -e])
	       cube( [door_w, door_h, 10]);
	  echo("Cavity (mm)", body_w-2*body_thk, body_h-2*body_thk, body_d);
	  echo("Cavity (in)", (body_w-2*body_thk)/mm, (body_h-2*body_thk)/mm, body_d/mm);
	  echo("Door (mm)", door_w, door_h);
	  echo("Door (in)", door_w/mm, door_h/mm);
     }

     translate( [0, 0, -body_d+body_thk-e]) {
       standoff_at( -pcb_hole_dx/2, -pcb_hole_dy/2, pcb_standoff_dia, pcb_hole_dia, body_d-pcb_standoff_down, 270); 
       standoff_at( -pcb_hole_dx/2, pcb_hole_dy/2, pcb_standoff_dia, pcb_hole_dia, body_d-pcb_standoff_down, 270);
       standoff_at( pcb_hole_dx/2, -pcb_hole_dy/2, pcb_standoff_dia, pcb_hole_dia, body_d-pcb_standoff_down, 90); 
       standoff_at( pcb_hole_dx/2, pcb_hole_dy/2, pcb_standoff_dia, pcb_hole_dia, body_d-pcb_standoff_down, 90);
    }
}

module mounting_holes() {
  translate( [-pcb_hole_dx/2, -pcb_hole_dy/2, 0]) cylinder( h=20, d=panel_hole_dia);
  translate( [-pcb_hole_dx/2, pcb_hole_dy/2,  0]) cylinder( h=20, d=panel_hole_dia);
  translate( [pcb_hole_dx/2, -pcb_hole_dy/2,  0]) cylinder( h=20, d=panel_hole_dia);
  translate( [pcb_hole_dx/2, pcb_hole_dy/2, 0]) cylinder( h=20, d=panel_hole_dia);
}


// front panel, centered

module panel() {
  translate( [-panel_w/2, -panel_h/2, 0]) {
    cube( [panel_w, panel_h, body_thk]);
    translate( [body_lip+body_thk, body_lip+body_thk, -body_thk+e])
      cube( [panel_w_in, panel_h_in, body_thk]);
  }
     
}

// holes for switches, knob, LEDs, USB
module button_holes() {
  // LEDs
  translate( [-115, 35, 0])  
    cylinder( d=3.5, h=20);
  translate( [-108, 35, 0])  
    cylinder( d=3.5, h=20);
  // knob
  translate( [-114, 18.5, 0])
    cylinder( d=17, h=20);
  // left 2 switches
  cube( [18, 38, 20]);
  // USB
  translate( [12.7, -6.5, 0])
    cube( [12, 0.26*mm, 20]);
}

module panel_with_holes() {

  // front panel with holes
  translate( [0,0,16]) {
    difference() {
      panel();
      translate( [0, 0, -5]) {
	translate( [-9,2,0]) oled_holes();
	mounting_holes();
	translate( [42.7, -16, 0])
	  button_holes();
      }
      // Antenna hole
      translate( [pcb_hole_dx/2-0.325*mm, -pcb_hole_dy/2+0.475*mm, -5]) cylinder( h=20, d=0.26*mm);
    }
  }
}

projection() panel_with_holes();

// box();

// translate( [0, 0, 5]) {
//      difference() {
//        	  color("brown") panel();
// 	  translate( [-body_w+0.65*mm, 70, 0])
// 	       translate( [147.5, -69.7, 0]) oled_holes();
//      }
// }
// 
// 

// translate( [-body_w+0.62*mm, 71.8, 10]) {
//       color("green") import("car-control.stl");
//       translate( [147.5, -69.9, 3]) oled();
// }
