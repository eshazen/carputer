//
// standoff hole size test
//

mm = 25.4;
$fn = 32;
e = 0.1;

pcb_hole_dia = 0.110*mm;	/* 6-32 tap drill */
pcb_standoff_dia = 0.25*mm;


module standoff_at( x, y, dia, hole, hgt, rot) {
    translate( [x, y, 0]) {
      rotate( [0, 0, rot]) {
      difference() {
	union() {
	  cylinder( d=dia, h=hgt);
//	  translate( [-pcb_so_supp_wid/2, -pcb_so_supp_len, 0])
//	    cube( [pcb_so_supp_wid, pcb_so_supp_len, hgt]);
	}
	translate( [0, 0, -e])
	  cylinder( d=hole, h=hgt+2*e);
      }
    }
  }
}


cube( [120, 25, 1.6]);
for( i=[0:10]) {
     hole_dia = mm*(0.110 + 0.005*i);
     echo("Hole = ", hole_dia/mm);
     standoff_at( 10+10*i, 12.5, pcb_standoff_dia, hole_dia, 0.5*mm, 0);
}
