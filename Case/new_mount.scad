//
// Miata NA dual-opening DIN mount
// reboot with more attention to detal
//
// note that bottom opening is narrower
//

mm = 25.4;
e = 0.1;
$fn = 32;

thin = 0;

// DIN opening 
din_w = 7.187*mm;		/* width - upper opening for radio */
din_w2 = 6.875*mm;
din_h = 2.125*mm;		/* height */
din_d = 1*mm;		/* depth */
// din_d = 0.5*mm;		/* fast test depth */

din_thk = 0.1*mm;		/* DIN wall thickness */

overall_h = 4.75*mm;
overall_w = 7.375*mm;

lower_w = 7.1875*mm;		/* lower bay overall width 7-3/16 */
lower_h = 2.25*mm;		/* lower bay height of narrower frame portion */

lower_w2 = 7.0625*mm;		/* bottom narrowing to fit panel opening */

upper_h = overall_h-lower_h;
side_cut_d = 0.4*mm;

w_diff = overall_w-lower_w;

frame_thk = 0.125*mm;

din_hgt = din_h + 2*din_thk;
din_wid = din_w + 2*din_thk;

din_c2c = overall_h-din_hgt;

// supports
supp_x = 1.625*mm;
supp_w = 0.5*mm;
supp_h = din_thk;
supp_l = 1*mm;
//supp_l = 0.35*mm;		/* fast test */

// bracket 
bkt_w = 0.5*mm;
bkt_h = 0.6*mm;
bkt_t = 0.125*mm;
bkt_hole = 0.225*mm;
bkt_slot = 0.4*mm-bkt_hole;

// upper bracket
ubkt_w = 0.5*mm;
ubkt_h = 1.0*mm;
ubkt_t = 0.125*mm;

// top_bkt = 1.063*mm;
top_bkt = 0.938*mm;
bot_bkt = 0.688*mm;

// DIN opening specified with, height, centered
module din_open( wid, hgt) {
     translate( [-wid/2, -hgt/2, -e]) {
	  cube( [wid, hgt, din_d+2*e]);
     }
}

module test_open2() {
     translate( [0, -din_c2c/2, 0]) {
	  din_open( din_w, din_h);
	  translate( [0, din_c2c, 0]) din_open( din_w2, din_h);
     }
}


// Two openings centered
module din_open2() {
     translate( [0, -din_c2c/2, 0]) {
	  din_open( din_w, din_h);
	  translate( [0, din_c2c, 0]) din_open( din_w2, din_h);
     }
}

// outer frame, solid, centered
module outer() {
     translate( [-overall_w/2, -overall_h/2, 0]) {
	  cube( [overall_w, upper_h, din_d]);
	  translate( [(overall_w-lower_w)/2, upper_h, 0])
	       cube( [lower_w, lower_h, din_d]);
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
module bracket( d) {
     difference() {
	  translate( [-bkt_w/2, -bkt_h/2, 0]) cube( [bkt_w+d, bkt_h, bkt_t]);
	  translate( [0, -bkt_slot/2, -e]) cylinder( d=0.225*mm, h=bkt_t+2*e);
	  translate( [0, bkt_slot/2, -e]) cylinder( d=0.225*mm, h=bkt_t+2*e);
	  translate( [-bkt_hole/2, -bkt_slot/2, -e]) cube( [bkt_hole, bkt_slot, bkt_t+2*e]);
     }
}

// upper bracket
module up_bracket( d) {
     difference() {
	  translate( [-ubkt_w/2, -bkt_h/2, 0]) cube( [ubkt_w+d, ubkt_h, ubkt_t]);
	  // slot for reference
	  translate( [0, -bkt_slot/2, -e]) cylinder( d=0.225*mm, h=bkt_t+2*e);
	  translate( [0, bkt_slot/2, -e]) cylinder( d=0.225*mm, h=bkt_t+2*e);
	  translate( [-bkt_hole/2, -bkt_slot/2, -e]) cube( [bkt_hole, bkt_slot, bkt_t+2*e]);
	  // new mounting hole
	  translate( [0.135*mm, 0.405*mm, -e]) color("red") cylinder( d=4, h=10);
     }
}


// four mounting brackets
module brackets() {
     translate( [-overall_w/2-bkt_w/2+e, overall_h/2-top_bkt, 0]) bracket(3);
     translate( [overall_w/2+bkt_w/2-e, overall_h/2-top_bkt, 0]) rotate( [0,0,180]) bracket(3);

//     translate( [-overall_w/2-bkt_w/2+e, -overall_h/2+bot_bkt, 0]) up_bracket(0);
     translate( [overall_w/2+bkt_w/2-e, -overall_h/2+bot_bkt, 0]) up_bracket(3);
     mirror( [1, 0, 0])     translate( [overall_w/2+bkt_w/2-e, -overall_h/2+bot_bkt, 0]) up_bracket(3);
}

// side cuts next to top
module side_cut() {
     translate( [overall_w/2+7, -din_c2c+frame_thk-5, din_d-side_cut_d]) {
	  rotate( [0, 0, 90]) {
	       cube( [din_h+5, 14.4, side_cut_d+e]);
	  }
     }
}

module side_cuts() {
     side_cut();
     mirror( [1, 0, 0]) side_cut();
}

module narrow_cut() {
     translate( [-lower_w/2-5, frame_thk-8, din_d-side_cut_d+e])
	  cube( [(lower_w-lower_w2)/2+5, din_h+20, side_cut_d]);
}

module narrow() {
     narrow_cut();
     mirror( [1, 0, 0]) narrow_cut();
}

cut1_x = 38;
cut1_dx = 15;

cut2_x = 129;
cut2_dx = 15;

// din_cut = 15;
din_cut = 10;

module mount() {

// frame with DIN openings
     difference() {
	  outer();
	  din_open2();
	  side_cuts();
	  narrow();
	  translate( [-din_w/2+cut1_x, -10, -20+din_cut]) cube( [cut1_dx, 20, 20]);
	  translate( [-din_w/2+cut2_x, -10, -20+din_cut]) cube( [cut2_dx, 20, 20]);
     }

     brackets();
// supports();
}

ow = overall_w+40;
oh = overall_h+40;

// slice thin
if( thin == 1) {
     difference() {
	  mount();
	  translate( [-ow/2-e, -oh/2-e, 1.6])
	       cube( [ow+2*e, oh+3*e, 30]);
     }
} else {
     mount();
}
