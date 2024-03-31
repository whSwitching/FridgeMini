

module aluminium_1060_plate(dz){
    color("silver")
    translate([-150, -95, dz])
    cube([300,190,3]);
}

module TEC1_12706(dz) {
    union(){
        color("red")
        translate([-20, -20, dz])
        cube([40,40,0.5]);
        color("white")
        translate([-20, -20, dz+0.5])
        cube([40,40,3]);
        color("blue")
        translate([-20, -20, dz+3.5])
        cube([40,40,0.5]);
    }
}


module box_fan_sink()
{
    color("White")
    union()
    {
        translate([23,23,0])
        cube([24,24,20]);
        rotate([0,0,-45])
        cube([2,98,20]);
        translate([70,0,0])
        rotate([0,0,45])
        cube([2,98,20]);
        difference()
        {
            difference()
            {
                union()
                {
                    for(y=[2:2:68]){
                        translate([-3,y,0])
                        cube([76,0.5,20]);
                    }
                }
                translate([-13,-15,-1])
                rotate([0,0,-45])
                cube([70,70,22]);
            }
            translate([-13,84,-1])
            rotate([0,0,-45])
            cube([70,70,22]);
        }
        difference()
        {
            difference()
            {
                union()
                {
                    for(x=[2:2:68]){
                        translate([x,0,0])
                        cube([0.5,70,20]);
                    }
                }
                translate([-13,-15,-1])
                rotate([0,0,45])
                cube([70,70,22]);
            }
            translate([86,-15,-1])
            rotate([0,0,45])
            cube([70,70,22]);
        }
    }
}

module box_fan_fan()
{
    union()
    {
        color("DimGray")
        difference()
        {
            difference()
            {
                difference()
                {
                    cube([70,70,15]);
                    translate([35,35,-1])
                    cylinder(d=68, h=17);
                }
                difference()
                {
                    translate([35,35,2])
                    cylinder(d=82, h=11);
                    translate([35,35,-1])
                    cylinder(d=69, h=17);
                }
            }
            union()
            {
                translate([4,4,-1])
                cylinder(d=5, h=17);
                translate([66,4,-1])
                cylinder(d=5, h=17);
                translate([4,66,-1])
                cylinder(d=5, h=17);
                translate([66,66,-1])
                cylinder(d=5, h=17);
            }
        }
        color("DimGray")
        translate([35,35,0])
        rotate([90,0,$t*-360])
        union(){
            rotate([-90,0,0])
            cylinder(d=30, h=15);
            for(r=[0:40:360]){
                rotate([0,r,0])
                linear_extrude(height = 33, twist = -45, slices = 20){
                    square([1,15]);
                };
            }
        }
        
        color("whitesmoke")
        translate([35,35,15])
        cylinder(d=20, h=0.1);
    }
}
module box_fan()
{
    box_fan_sink();
    
    color("silver")
    union()
    {
        translate([35,11,20])
        cylinder(d=6, h=1);
        translate([35,11,-5])
        cylinder(d=5, h=2);
        translate([35,11,-5])
        cylinder(d=3, h=25);
    }
    color("silver")
    union()
    {
        translate([35,61,20])
        cylinder(d=6, h=1);
        translate([35,61,-5])
        cylinder(d=5, h=2);
        translate([35,61,-5])
        cylinder(d=3, h=25);
    }

    translate([0,0,25])
    box_fan_fan();
    color("black")
    translate([4,4,20])
    cylinder(d=5, h=17);
    color("black")
    translate([66,4,20])
    cylinder(d=5, h=17);
    color("black")
    translate([66,66,20])
    cylinder(d=5, h=17);
    color("black")
    translate([4,66,20])
    cylinder(d=5, h=17);

}

