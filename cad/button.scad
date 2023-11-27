echo(version=version());


w1 = 9.0;
w2 = 11.8;

wall = 3;
divider = 2.1;
h = 18.38 + 2*wall+divider;

d = 18;


module example_intersection()
{
    
union() {
	/*intersection() {
		difference() {
			union() {
				cube([30, 30, 30], center = true);
				translate([0, 0, -25])
					cube([15, 15, 50], center = true);
			}
			union() {
				cube([50, 10, 10], center = true);
				cube([10, 50, 10], center = true);
				cube([10, 10, 50], center = true);
			}
		}
		translate([0, 0, 5])
			cylinder(h = 50, r1 = 20, r2 = 5, center = true);
	}*/
    
    w = 13.85;
    hold = 16;
    h = 10.8;
    half = 6;
    wall = 1.5;
    eps = 0.001;
    difference() {
        
    cube([w + wall + wall, half + wall, h + wall]);
       
       translate([wall, -eps, -eps]) 
    cube([w, half, h]);
    }
    
       translate([0, half + wall - eps, 0]) 
    cube([w + 2 * wall, hold, wall]);
    
    /*rotate([90])
		difference() {
				translate([0, 0, h/2])
					cube([w2 + 2*wall, d, h], center = true);
				translate([0, 0, (h - wall * 2 - divider) / 2])
					cube([w1, d * 1.001, h * 1.001 - wall * 2 - divider], center = true);
				translate([0, 0, h - divider / 2 - wall])
					cube([w2 , d * 1.001, divider], center = true);
		}*/
}
}

example_intersection();
