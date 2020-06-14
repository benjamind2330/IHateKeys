include <BOSL/constants.scad>
use <BOSL/transforms.scad>
use <Fillet/fillets.scad>

board_width = 68;
board_length = 95; 
board_hole_radius = 2;
hole_distance_from_side = 4;
board_depth = 1;
power_depth = 14;
p2_width = 9/2;
p2_depth = 11;

connector_width = 12.5;
connector_length = 13;
connector_depth = 5.5;
c1_position = [0, 9.5];
c2_position = [0, 9.5 + connector_width + 1];

module hole_positions(hole_size, quality = 10) {
    move([hole_distance_from_side, hole_distance_from_side]) circle(hole_size, $fn = quality);
    move([board_length - hole_distance_from_side, hole_distance_from_side]) circle(hole_size, $fn = quality);
    move([board_length - hole_distance_from_side, board_width - hole_distance_from_side]) circle(hole_size, $fn = quality);
    move([hole_distance_from_side, board_width - hole_distance_from_side]) circle(hole_size, $fn = quality);
}


module board() {

    linear_extrude(board_depth) difference() {
        square([board_length, board_width]);
        hole_positions(board_hole_radius);
    }

    module connections() {

        for(pos = [c1_position, c2_position]) {
            move(pos) cube([connector_length, connector_width, connector_depth]);
        }
        ymove(37) cube([10, 10, power_depth]);
        zmove(p2_width) ymove(50 + p2_width) yrot(90) cylinder(p2_depth, p2_width, p2_width);
    }
    zmove(board_depth) color("grey") connections();

}

wall_width = 3;
base_width = 2;
offset_to_board = 1;
cut_in_length = 8;

stand_off_height = power_depth + 1;
wall_height = stand_off_height + board_depth*2;

module base() {

    adder = wall_width + offset_to_board;
    fillet_size = 5;

    box_height = wall_height + base_width;
    box_width = board_width+adder;
    box_length = board_length + adder;
    cut_in_size = 1.5;
    cut_in_length = 5;

    difference() {
        minkowski() {
            cube([box_length - fillet_size, box_width - fillet_size, box_height - fillet_size], center = true);
            sphere(fillet_size /2, $fn=15);
        }
        minkowski() {
            zmove(base_width + fillet_size/2) cube([box_length - wall_width - fillet_size, box_width - wall_width - fillet_size, box_height+2], center=true);    
            sphere(fillet_size /3, $fn=15);
        }
        minkowski() {
            union() {
                for (x = [0:1:4]) {
                    for (y = [0:1:4]) {
                        move([-10, -20]) move([x*10, y*10, -box_height/2]) cube([5, 5, 10], center = true);
                    }
                }
            }
            sphere(0.5);
        }
        zmove(box_height/2 - cut_in_length/2 - 0.5) cube([box_length - cut_in_size*2, box_width - cut_in_size*2, cut_in_length + 1], center = true);
        move([-box_length/2, 0, base_width+1.5]) cube([20, 50, 20], center=true);
        move([box_length/2 - 5, -20, -1]) yrot(90) cylinder(10, 4.5, 4.5);
    }


    module standoffs() {
        move([-box_length/2+adder/2, -box_width/2+adder/2,  -box_height/2 + base_width]) linear_extrude(stand_off_height) hole_positions(board_hole_radius + 1, 20);
        move([-box_length/2+adder/2, -box_width/2+adder/2,  -box_height/2 + base_width + stand_off_height]) linear_extrude(board_depth*2) hole_positions(board_hole_radius -0.25, 20);
        difference() {
            zmove(-box_height/2 + base_width + fillet_size/2 -3.135) cube([board_length+2, board_width+2, 1], center = true);
            zmove(-box_height/2 + base_width + fillet_size/2 -3.135) cube([board_length+2 - 20, board_width+2 - 20, 2], center = true);
        }
    }

    if ($preview) {
        standoffs();
    } else {
        add_fillets(R=1,OB=1000,axis="xyz",fn=10)
        {
            standoffs();
        }
    }
    
}


module rounded_box(dims, fillet_radius, fn = 10) {
    curve = 2*fillet_radius;
    box = [dims[0] - curve, dims[1] - curve, dims[2] - curve];
    minkowski() {
        cube(box, center = true);
        sphere(fillet_radius, $fn = fn);
    }
}

function get_z_move_distance(box1_height, box2_hieght, diff) =
    (abs(box1_height - box2_hieght)/2) + diff;

module top() {

    existing_width = 71.65;
    existing_length = 98.2;
    existing_height = 18.3;

    thickness = 2;
    radius = 1;

    module frame() {

        difference() {
            rounded_box([existing_length + thickness*2, existing_width + thickness*2, existing_height/2 + thickness*2], radius);
            zmove(get_z_move_distance(existing_height/2 + thickness*2, existing_height*2, thickness))
            rounded_box([existing_length, existing_width, existing_height*2], radius);
        }
    }

    b_length = 17.5;
    buttons_length = (3*b_length + 3)/2;

    module buttons() {
        b_width = 15;
        
        rad = 2;
        height = 10;
        xmove(-buttons_length)
        xmove(rad)
        for (row = [0:1:2]) {
            xmove(row + b_length*row)
            ymove(-(4*b_width + 3)/2)
            ymove(rad) 
            union() {
                for (col = [0:1:3]) {
                    ymove(col + col*b_width)
                    minkowski() {
                        cube([b_length - 2*rad, b_width - 2*rad, height]);
                        cylinder(height/2, rad, rad, $fn=15);
                    }
                    
                
                }
            }
        }
    }

    dist_buttons_from_top = 12.4;
    dist_buttons_from_bottom = 30.6;

    total_length = dist_buttons_from_bottom + dist_buttons_from_top + buttons_length;
    move_distance = (-total_length/2 + dist_buttons_from_bottom + buttons_length/2);


    module connector_cutout() {
        zmove(3) xmove(-existing_length / 2) cube([10, existing_width-radius*2, 15], center=true);
    }


    difference() {
        frame();
        xmove(move_distance) zmove(-10) buttons();
        connector_cutout();
    }

}

top();


*base_2();
*board();
*base();