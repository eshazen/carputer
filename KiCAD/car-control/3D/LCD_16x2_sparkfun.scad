

module lcd() {

  // PCB with mounting holes
  difference() {
    color("#006020")  cube([80,36,1.6]);
    translate( [0, 0, -0.1]) {
      $fn = 24;
      translate( [2.5, 2.5, 0])  cylinder( d=2.5, h=1.8);
      translate( [2.5, 36-2.5, 0])  cylinder( d=2.5, h=1.8);
      translate( [80-2.5, 2.5, 0])  cylinder( d=2.5, h=1.8);
      translate( [80-2.5, 36-2.5, 0])  cylinder( d=2.5, h=1.8);
    }
  }
  // display body
  translate([4.35, 4.85, 3.2]) color("grey") cube([71.3, 26.3, 5.4]);
  // connector pins
  color("#FFFF00")
  translate( [8, 36-2.5, -5]) {
    $fn = 4;
    for( i=[0:15]) {
      translate( [i*2.54, 0, 0])
	cylinder( d=1.0,h=7);
    }
  }
}

lcd();


    
