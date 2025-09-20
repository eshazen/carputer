// OLED display dims
opcb_w = 100;
opcb_h = 1.316*mm;
opcb_t = 1;

odpy_w = 3.517*mm;
odpy_h = 1.224*mm;
odpy_t = 0.125*mm;

ohol_dy = 1.125*mm;
ohol_dx = 3.734*mm;
ohol_dia = 0.125*mm;

module hole_at( x, y, dia) {
     translate( [x, y, -e])
     cylinder( d=dia, h=20);
}

module oled() {
     dx = (opcb_w-ohol_dx)/2;
     dy = (opcb_h-ohol_dy)/2;
     translate( [-opcb_w/2, -opcb_h/2, 0]) {
	  difference() {
	       color("blue") cube( [opcb_w, opcb_h, opcb_t]);
	       hole_at( dx, dy, ohol_dia);
	       hole_at( opcb_w-dx, dy, ohol_dia);
	       hole_at( dx, opcb_h-dy, ohol_dia);
	       hole_at( opcb_w-dx, opcb_h-dy, ohol_dia);
	  }
	  translate( [(opcb_w-odpy_w)/2, (opcb_h-odpy_h)/2, opcb_t])
	  color("black") cube( [odpy_w, odpy_h, odpy_t]);
     }
}

module oled_holes() {
     dx = (opcb_w-ohol_dx)/2;
     dy = (opcb_h-ohol_dy)/2;
     spc = 0.25;			/* space around display */
     translate( [-opcb_w/2, -opcb_h/2, 0]) {
	  hole_at( dx, dy, ohol_dia);
	  hole_at( opcb_w-dx, dy, ohol_dia);
	  hole_at( dx, opcb_h-dy, ohol_dia);
	  hole_at( opcb_w-dx, opcb_h-dy, ohol_dia);

	  translate( [(opcb_w-odpy_w)/2-spc, (opcb_h-odpy_h)/2-spc, -e])
	  cube( [odpy_w+2*spc, odpy_h+2*spc, 10]);
     }
     
}
