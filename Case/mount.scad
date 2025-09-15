//
// Miata NA dual-opening DIN mount
//

mm = 25.4;
e = 0.1;
$fn = 32;

fast = 1;

// DIN opening 
//din_w = 7.25*mm;		/* width */
din_w = 7.187*mm;		/* width */
din_h = 2.125*mm;		/* height */
din_d = 1*mm;		/* depth */
// din_d = 0.25*mm;		/* fast test depth */

din_thk = 0.1*mm;		/* DIN wall thickness */

overall_h = 4.75*mm;
overall_w = 7.375*mm;

frame_thk = 0.125*mm;

din_hgt = din_h + 2*din_thk;
din_wid = din_w + 2*din_thk;

din_c2c = overall_h-din_hgt;

// supports
supp_x = 1.625*mm;
supp_w = 0.5*mm;
supp_h = din_thk;
// supp_l = 1*mm;
supp_l = 0.35*mm;		/* fast test */

// bracket 
bkt_w = 0.5*mm;
bkt_h = 0.6*mm;
bkt_t = 0.125*mm;
bkt_hole = 0.225*mm;
bkt_slot = 0.4*mm-bkt_hole;

// top_bkt = 1.063*mm;
top_bkt = 0.938*mm;
bot_bkt = 0.688*mm;

// DIN opening only
module din_open() {
     translate( [din_thk-din_wid/2, din_thk-din_hgt/2, -e])
	  cube( [din_w, din_h, din_d+2*e]);
}

// Two openings centered
module din_open2() {
     translate( [0, -din_c2c/2, 0]) {
	  din_open();
	  translate( [0, din_c2c, 0]) din_open();
     }
}

// outer frame, solid, centered
module outer() {
     translate( [-overall_w/2, -overall_h/2, 0]) {
	  cube( [overall_w, overall_h, din_d]);
     }
}

// one support
module support() {
     cube( [supp_w, supp_h, supp_l]);
}

// left, right and bottom supports
module supports() {
     translate( [-overall_w/2+supp_x-supp_w/2, overall_h/2-supp_h, -supp_l+e])
	  support();
     translate( [overall_w/2-supp_x-supp_w/2, overall_h/2-supp_h, -supp_l+e])
	  support();
     translate( [-overall_w/2+din_thk, overall_h/2-top_bkt-bkt_h/2, -supp_l+e])
	  rotate([0,0,90]) support();
     translate( [overall_w/2-din_thk+supp_h, overall_h/2-top_bkt-bkt_h/2, -supp_l+e])
	  rotate([0,0,90]) support();
}

// one mounting bracket with oval hole
module bracket() {
     difference() {
	  translate( [-bkt_w/2, -bkt_h/2, 0]) cube( [bkt_w, bkt_h, bkt_t]);
	  translate( [0, -bkt_slot/2, -e]) cylinder( d=0.225*mm, h=bkt_t+2*e);
	  translate( [0, bkt_slot/2, -e]) cylinder( d=0.225*mm, h=bkt_t+2*e);
	  translate( [-bkt_hole/2, -bkt_slot/2, -e]) cube( [bkt_hole, bkt_slot, bkt_t+2*e]);
     }
}

// four mounting brackets
module brackets() {
     translate( [-overall_w/2-bkt_w/2+e, overall_h/2-top_bkt, 0]) bracket();
     translate( [overall_w/2+bkt_w/2-e, overall_h/2-top_bkt, 0]) bracket();

     translate( [-overall_w/2-bkt_w/2+e, -overall_h/2+bot_bkt, 0]) bracket();
     translate( [overall_w/2+bkt_w/2-e, -overall_h/2+bot_bkt, 0]) bracket();
}

cut1_x = 38;
cut1_dx = 15;

cut2_x = 129;
cut2_dx = 15;

// din_cut = 15;
din_cut = 10;


// frame with DIN openings
difference() {
     outer();
     din_open2();
     translate( [-din_w/2-cut1_x, -10, -din_cut])
	  cube( [cut1_dx, 20, 20]);
     translate( [-din_w/2+cut1_x, -10, -20+din_cut])
	 cube( [cut1_dx, 20, 20]);
     translate( [-din_w/2+cut2_x, -10, -20+din_cut])
	 cube( [cut2_dx, 20, 20]);
}


brackets();
// supports();
