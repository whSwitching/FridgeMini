
module fan(){
    color("whitesmoke")
    union(){
        rotate([-90,0,0])
        cylinder(d=42, h=22);
    
        for(r=[0:40:360]){
            rotate([0,r,0])
            linear_extrude(height = 55, twist = -45, slices = 20){
                square([1,22]);
            };
        }
    }
    
    union(){
        color("black")
        rotate([-90,0,0])
        translate([0,0,-0.1])
        cylinder(d=30, h=0.1);
        
        color("white")
        translate([1,-0.2,1])
        cube([6,1,6]);
        color("white")
        translate([-7,-0.2,1])
        cube([6,1,6]);
        color("white")
        translate([-7,-0.2,-7])
        cube([14,1,6]);
    }
}

module ForzenMagic120_heatsink(){
    color("SlateGray")
    union(){
        translate([98, -60, -157.5])
        cube([28,5,120]);
        translate([98, -55, -157.5])
        cube([2,5,10]);
        translate([98, -55, -47.5])
        cube([2,5,10]);
        translate([124, -55, -157.5])
        cube([2,5,10]);
        translate([124, -55, -47.5])
        cube([2,5,10]);
    };
    color("SlateGray")
    union(){
        translate([98, 55, -157.5])
        cube([28,5,120]);
        translate([98, 50, -157.5])
        cube([2,5,10]);
        translate([98, 50, -47.5])
        cube([2,5,10]);
        translate([124, 50, -157.5])
        cube([2,5,10]);
        translate([124, 50, -47.5])
        cube([2,5,10]);
    };
    color("SlateGray")
    for(w=[53:-2:-55]){
            translate([102, w, -157.5])
            cube([20,1,120]);
    }
    color("dimgray")
    union(){
        translate([98, -60, -38])
        cube([28,120,5]);
        translate([101, -57, -33])
        cube([22,114,8]);
    }
    union(){
        color("dimgray")
        union(){
            translate([98, -60, -163])
            cube([28,120,5]);
            translate([101, -57, -171])
            cube([22,114,8]);
            polyhedron(
                [
                    [101,-57,-171],[101,57,-171],[123,57,-171],[123,-57,-171],
                    [101,-50,-178],[101,50,-178],[123,50,-178],[123,-50,-178]
                ],
                [
                    [0,1,2,3],[4,5,1,0],[7,6,5,4],[5,6,2,1],[6,7,3,2],[7,4,0,3]
                ]
            );
        };    
        color("OrangeRed")
        translate([100, 14, -170])
        rotate([0,-90,0])
        cylinder(d=10, h=20);
        color("DarkTurquoise")
        translate([100, 0, -170])
        rotate([0,-90,0])
        cylinder(d=10, h=20);
    };
}

module ForzenMagic120_heatfan(){
    color("DarkSlateGray")
    union(){
        difference(){
            difference(){
                translate([73, -60, -158])
                cube([25,120,120]);
                // diff
                translate([72, 0, -98])
                rotate([0,90,0])
                cylinder(d=114, h=28);
            };
            union(){
                difference(){        
                    translate([77, 0, -98])
                    rotate([0,90,0])
                    cylinder(d=180, h=17);
                    translate([76, 0, -98])
                    rotate([0,90,0])
                    cylinder(d=140, h=20);
                };
                difference(){
                    translate([81, 0, -98])
                    rotate([0,90,0])
                    cylinder(d=145, h=9);
                    translate([78, 0, -98])
                    rotate([0,90,0])
                    cylinder(d=130, h=18);
                };
            }
        };
        translate([97, -40.3, -140])
        rotate([-45,0,0])
        cube([0.9,5,115]);
        translate([97, 40.3, -140])
        rotate([45,0,0])
        cube([0.9,5,115]);
    }
    translate([73, 0, -98])
    rotate([0,$t*360,-90])
    fan();
}

module heatfan_heatsink_M4(){
    color("white")
    union(){
        translate([73, -53, -45])
        rotate([0,90,0])
        cylinder(d=4, h=30);
        translate([72, -53, -45])
        rotate([0,90,0])
        cylinder(d=6, h=1);
    }
    color("white")
    union(){
        translate([73, -53, -150])
        rotate([0,90,0])
        cylinder(d=4, h=30);
        translate([72, -53, -150])
        rotate([0,90,0])
        cylinder(d=6, h=1);
    }
    color("white")
    union(){
        translate([73, 52, -45])
        rotate([0,90,0])
        cylinder(d=4, h=30);
        translate([72, 52, -45])
        rotate([0,90,0])
        cylinder(d=6, h=1);
    }
    color("white")
    union(){
        translate([73, 52, -150])
        rotate([0,90,0])
        cylinder(d=4, h=30);
        translate([72, 52, -150])
        rotate([0,90,0])
        cylinder(d=6, h=1);
    }
}
