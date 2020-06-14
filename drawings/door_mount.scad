include <BOSL/constants.scad>
use <BOSL/transforms.scad>


module handle_top() {
    radius = 15;
    height = 10;
    minor_rad = 3;


    minkowski() {
        linear_extrude(height)
        difference() {
            circle(radius, $fn=20);
            for (angle = [-180:60:180]) {
                move([radius*cos(angle), radius*sin(angle)])
                zrot(angle)
                xscale(0.5) circle(minor_rad, $fn=20);
            }
        }
        sphere(1, $fn=10);
    }
}




module handle() {

    shaft_rad = 5;
    shaft_height = 10;

    zmove(shaft_height) handle_top();
    cylinder(shaft_height, shaft_rad, shaft_rad);

    key_height = 5;
    key_length = 20;
    xmove(-key_length) ymove(-shaft_rad) cube([key_length, shaft_rad*2, key_height]);
}


back_plate_length = 100;
back_plate_width = 40;
back_plate_height = 3;
distance_between_screw_holes = 60;
screw_rad = 1;
screw_top_rad = 3;

module back_plate() {

    module screw_holes() {
        countersink_height = 2;
        xmove(distance_between_screw_holes/2) 
        {
            cylinder(back_plate_height*2, screw_rad, screw_rad, center = true, $fn=20);
            zmove(back_plate_height/2 - countersink_height)
            cylinder(countersink_height, 0, screw_top_rad, $fn=20);
        }

        xmove(-distance_between_screw_holes/2) 
        {
            cylinder(back_plate_height*2, screw_rad, screw_rad, center = true, $fn=20);
            zmove(back_plate_height/2 - countersink_height)
            cylinder(countersink_height, 0, screw_top_rad, $fn=20);
        }

    }

    module cut_out() {
        ymove(-5) xmove(-5) cube([40, back_plate_width-5, back_plate_height*2], center=true);
    }

    difference () {
        cube([back_plate_length, back_plate_width, back_plate_height], center = true);
        screw_holes();
        cut_out();    
    }

}


handle();
back_plate();