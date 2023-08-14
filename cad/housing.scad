echo(version=version());

$fn= $preview ? 16 : 1024;

// Higher definition curves
$fs = $preview ? 2 : 0.01;

module r_cube(size = [1, 1, 1], center = false, radius = 0.5, apply_to = "all") {
	// If single value, convert to [x, y, z] vector
	size = (size[0] == undef) ? [size, size, size] : size;

	translate_min = radius;
	translate_xmax = size[0] - radius;
	translate_ymax = size[1] - radius;
	translate_zmax = size[2] - radius;

	diameter = radius * 2;

	module build_point(type = "sphere", rotate = [0, 0, 0]) {
		if (type == "sphere") {
			sphere(r = radius);
		} else if (type == "cylinder") {
			rotate(a = rotate)
			cylinder(h = diameter, r = radius, center = true);
		}
	}

	obj_translate = (center == false) ?
		[0, 0, 0] : [
			-(size[0] / 2),
			-(size[1] / 2),
			-(size[2] / 2)
		];

	translate(v = obj_translate) {
		hull() {
			for (translate_x = [translate_min, translate_xmax]) {
				x_at = (translate_x == translate_min) ? "min" : "max";
				for (translate_y = [translate_min, translate_ymax]) {
					y_at = (translate_y == translate_min) ? "min" : "max";
					for (translate_z = [translate_min, translate_zmax]) {
						z_at = (translate_z == translate_min) ? "min" : "max";

						translate(v = [translate_x, translate_y, translate_z])
						if (
							(apply_to == "all") ||
							(apply_to == "xmin" && x_at == "min") || (apply_to == "xmax" && x_at == "max") ||
							(apply_to == "ymin" && y_at == "min") || (apply_to == "ymax" && y_at == "max") ||
							(apply_to == "zmin" && z_at == "min") || (apply_to == "zmax" && z_at == "max")
						) {
							build_point("sphere");
						} else {
							rotate = 
								(apply_to == "xmin" || apply_to == "xmax" || apply_to == "x") ? [0, 90, 0] : (
								(apply_to == "ymin" || apply_to == "ymax" || apply_to == "y") ? [90, 90, 0] :
								[0, 0, 0]
							);
							build_point("cylinder", rotate);
						}
					}
				}
			}
		}
	}
}

measured_sensor_height = 39.78;
measured_sensor_with_wing_height = 46.44;
wing_height = measured_sensor_with_wing_height - measured_sensor_height;

MISMEASURE = 20;
measured_h = 28.99 + MISMEASURE;

wall = 3;

circle_foot = 2;
circle_d = 43.37 + 1.0 + circle_foot;
circle_h = measured_h - wing_height + wall + 0.5; // 25;

servo_l_center = 37.56;
servo_l_outer = 28.82;
servo_w = 20.70;
servo_h = 15;

// h[eight], r[adius], [rou]n[d]
module r_cylinder(h, r, n=0) {
    if (n == 0) {
        cylinder(h, r, r);
    } else {        
      rotate_extrude(convexity=1) {
        offset(r=n) offset(delta=-n) square([r,h]);
        square([n, h]);
      }
  }
}

module cover() {
    translate([0, 0, circle_h - wall])
    difference() {
        r_cylinder(wall, circle_d/2);
        
        translate([0, 0, -0.1])
        r_cylinder(wall * 2, servo_l_center - servo_l_outer);
    }
}

module servo_support() {
    translate([circle_d / 2 + wall / 2 - 0.05, 0, circle_h/2])
    cube([servo_l_outer + wall, servo_w + wall * 2, circle_h], center = true);
}

module bow1() {
    intersection() {
        translate([500, 0, 0])
        cube([1000, 1000, 1000], center = true);
        difference() {
            union() {
                r_cylinder(circle_h, circle_d / 2 + wall);
                servo_support();
            }
            
            translate([0,0,-0.001])
            r_cylinder(circle_h * 1.001, circle_d / 2);
        }
    }
}
module bow2() {        
    intersection() {
        translate([500, 0, 0])
        cube([1000, 1000, 1000], center = true);
        union() {
            cover();
            difference() {
                difference() {
                    translate([0,0,-wall])
                    r_cylinder(wall * 2, circle_d / 2 + wall + circle_foot, 0.6);
                    translate([-0.001, -50, -100 + 0.001])
                    cube(100);
                }
                
                translate([0,0,-0.001])
                r_cylinder(circle_h * 1.001, circle_d / 2 - circle_foot);
            }
        }
    };
    difference() {
        translate([-circle_foot, circle_d / 2 - circle_foot, -10])
        r_cube([circle_foot, wall + circle_foot * 2, wall + 10], radius=0.6, apply_to="xmin");
        translate([-50 -0.001, -50, -100 + 0.001])
        cube(100);
    }
    difference() {
        translate([-circle_foot, -circle_d / 2 - circle_foot - wall, -10])
        r_cube([circle_foot, wall + circle_foot * 2, wall + 10], radius=0.6, apply_to="xmin");
        translate([-50 -0.001, -50, -100 + 0.001])
        cube(100);
    }
}

module servo_walls() {
    union() {
        translate([servo_l_outer/2 + wall/2,0,0])
        translate([0,0,(wall + servo_h)/2])
        cube([wall, servo_w + wall*2, wall + servo_h], center = true);
        translate([0,servo_w/2 + wall/2,0])
        translate([0,0,(wall + servo_h)/2])
        cube([servo_l_outer, wall, wall + servo_h], center = true);
        translate([0,-servo_w/2 - wall/2,0])
        translate([0,0,(wall + servo_h)/2])
        cube([servo_l_outer, wall, wall + servo_h], center = true);
    }
}

module servo_base() {
    translate([0,0,wall/2])
    cube([servo_l_outer, servo_w, wall], center = true);
}

module servo_box() {
    translate([0,0,circle_h-wall])
    translate([servo_l_outer/2,0,0])
    translate([servo_l_center - servo_l_outer,0,0])
    union() {
        servo_walls();
        servo_base();
    }
}

module entrypoint()
{
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
    
    union() {
        bow1();
        bow2();
        servo_box();
    }
}

entrypoint();
