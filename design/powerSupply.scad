
module power_board_screw1(L=5)
{
    color("Silver")
    union()
    {
        cylinder(d=5, h=0.5, $fn=20);
        translate([0,0,0.2-L])
        cylinder(d=2, h=L, $fn=20);
        difference()
        {
            difference()
            {
                translate([0,0,-1.1])
                sphere(d=6, $fn=20);
                translate([-4,-4,-7.5])
                cube([8,8,8]);
            };
            union()
            {
                translate([-1.8,-0.4,0.8])
                cube([3.6,0.8,3]);
                translate([-0.4,-1.8,0.8])
                cube([0.8,3.6,3]);
            }
        }
    }
}

module power_board_screw2()
{
    color("Silver")
    translate([-3.5,-3.5,0])
    cube([7,7,1]);
    color("White")
    union()
    {
        translate([0,0,1.2])
        cylinder(d=5, h=0.5, $fn=20);
        translate([0,0,-3.5])
        cylinder(d=2, h=5, $fn=20);
        difference()
        {
            difference()
            {
                translate([0,0,0.1])
                sphere(d=6, $fn=20);
                translate([-4,-4,-6.3])
                cube([8,8,8]);
            };
            union()
            {
                translate([-3,-0.4,2])
                cube([6,0.8,3]);
                translate([-0.4,-1.8,2])
                cube([0.8,3.6,3]);
            }
        }
    }
}

module power_board_adj()
{
    union()
    {
        color("DarkSlateGray")
        translate([-4,-4,0])
        cube([8,8,4]);
        color("Orange")
        difference()
        {
            translate([0,0,4])
            cylinder(d=6, h=1);
            union()
            {
                translate([-2,-0.5,4.5])
                cube([4,1,1]);
                translate([-0.5,-2,4.5])
                cube([1,4,1]);
            }
        }
    };
}

module power_board_pins()
{
    color("Orange")
    union()
    {
        translate([0,0,13.5])
        cube([15,70,1]);
        translate([0,0,12.8])
        cube([1,70,1]);
    }
    union()
    {
        color("DarkSlateGray")
        difference()
        {
            cube([15,70,12]);
            for(y=[0:9.5:66]){
                translate([-10,y+3,2.5])
                cube([20,7.2,10]);
            }
        };
        color("Silver")
        {
            for(y=[0:9.5:66])
            {
                translate([4,y+6.6,2.6])
                power_board_screw2();
            }
        }
    }
}

module power_board()
{
    union()
    {
        union()
        {
            color("DarkGreen")
            difference()
            {
                cube([195,97,2]);
                translate([0,0,-1])
                cylinder(d=16, h=5, $fn=20);
            }
            color("Silver")
            translate([6,94.5,2.5])
            power_board_screw1(6.8);
            translate([190,94,2])
            power_board_screw1(6.3);
            translate([30,6,2])
            power_board_screw1(6.3);
            translate([190,6,2])
            power_board_screw1(6.3);
        }
        
        translate([7,13,2])
        power_board_adj();
        translate([0,20,2])
        power_board_pins();        
    }
}

module power_box()
{
    color("White")
    union()
    {
        // z1
        difference()
        {
            translate([16,0,39])
            cube([180,100,1]);
            union()
            {
                for(x=[23:10:190])
                {
                    for(y=[8:10:95])
                    {
                        translate([x,y,35])
                        cylinder(d=5, h=10);
                    }
                }
                for(x=[28:10:190])
                {
                    for(y=[13:10:95])
                    {
                        translate([x,y,35])
                        cylinder(d=5, h=10);
                    }
                }
            };
        };
        // y1
        union()
        {
            difference()
            {
                translate([16,99,0])
                cube([180,1,40]);
                union()
                {
                    for(x=[21:10:200])
                    {
                        for(z=[5:10:40])
                        {
                            translate([x,105,z])
                            rotate([90,0,0])
                            cylinder(d=5, h=10);
                        }
                    }
                    for(x=[26:10:190])
                    {
                         for(z=[10:10:35])
                        {
                            translate([x,105,z])
                            rotate([90,0,0])
                            cylinder(d=5, h=10);
                        }
                    }
                }
            };
            translate([15.5,94,6])
            cube([0.5,6,34]);
            translate([2,94,6])
            cube([14,6,0.5]);
        }
    };
    color("White")
    union()
    {
        // z2
        cube([200,100,1]);
        // y2
        cube([200,1,40]);
        // x1
        difference()
        {
            union()
            {
                translate([195,0,0])
                cube([1,100,40]);
                polyhedron(
                    [
                        [195,0,0],[200,0,0],[200,90,0],[195,90,0],
                        [195,0,10],[200,0,5],[200,90,5],[195,90,10],
                    ],
                    [
                        [0,1,2,3],[4,5,1,0],[7,6,5,4],[5,6,2,1],[6,7,3,2],[7,4,0,3]
                    ]
                );
            };
            translate([-1,1,0])
            polyhedron(
                [
                    [195,0,0],[200,0,0],[200,100,0],[195,100,0],
                    [195,0,10],[200,0,5],[200,100,5],[195,100,10],
                ],
                [
                    [0,1,2,3],[4,5,1,0],[7,6,5,4],[5,6,2,1],[6,7,3,2],[7,4,0,3]
                ]
            );
        };
    };
    // x2
    union()
    {
        color("White")
        translate([15,0,28])
        cube([0.5,100,12]);
        color("gray")
        union()
        {
            translate([15.4,87,32])
            rotate([90,0,-90])
            text("L",size=4,halign="center",valign="center", 
                     font="Arial:style=Bold");
            translate([15.4,77,32])
            rotate([90,0,-90])
            text("N",size=4,halign="center",valign="center", 
                     font="Arial:style=Bold");
            translate([15.4,67,32])
            rotate([90,0,-90])
            text("G",size=4,halign="center",valign="center", 
                     font="Arial:style=Bold");
            translate([15.4,57,32])
            rotate([90,0,-90])
            text("V-",size=4,halign="center",valign="center", 
                     font="Arial:style=Bold");
            translate([15.4,47,32])
            rotate([90,0,-90])
            text("V-",size=4,halign="center",valign="center", 
                     font="Arial:style=Bold");
            translate([15.4,37,32])
            rotate([90,0,-90])
            text("V+",size=4,halign="center",valign="center", 
                     font="Arial:style=Bold");
            translate([15.4,27,32])
            rotate([90,0,-90])
            text("V+",size=4,halign="center",valign="center", 
                     font="Arial:style=Bold");
            translate([15.4,14,32])
            rotate([90,0,-90])
            text("v+ adj",size=3,halign="center",valign="center", 
                     font="Arial:style=Bold");
        }
    }
}

module power()
{
    power_box();
    translate([2,2,4])
    power_board();
}

//power();