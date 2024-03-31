module ForzenMagic120_pump(dz){
    union(){
        color("gold")
        intersection(){
            translate([-30, -25, -2+dz])
            cube([60,50,2]);
            translate([0, 0, -2+dz])
            cylinder(d=68, h=2);
        };
        color("black")
        difference(){
            union(){
                translate([-39, -32, -3.1+dz])
                cube([78,64,1]);
                rotate([0,0,45])
                translate([-64, -5, -3.1+dz])
                cube([126,10,1]);
                rotate([0,0,-45])
                translate([-64, -5, -3.1+dz])
                cube([126,10,1]);
            };
            translate([-30, -40, -4+dz])
            cube([60,80,5]);
        };        
        color("dimgray")
        translate([0, 0, -45+dz])
        cylinder(d=70, h=43);
        color("darkgray")
        difference(){
            translate([0, 0, -46+dz])
            cylinder(d=70, h=1.1);
            translate([0, 0, -47+dz])
            cylinder(d=67, h=3);
        };
        color("white")
        union(){
            difference(){
                translate([0, 0, -45.1+dz])
                cylinder(d=58, h=0.2);
                translate([0, 0, -47+dz])
                cylinder(d=57, h=3);
            };
            difference(){
                translate([0, 0, -45.1+dz])
                cylinder(d=55, h=0.2);
                translate([0, 0, -47+dz])
                cylinder(d=54, h=3);
            };
            difference(){
                translate([0, 0, -45.1+dz])
                cylinder(d=52, h=0.2);
                translate([0, 0, -47+dz])
                cylinder(d=51, h=3);
            };
            translate([0, 0, -44.6+dz])
            rotate([180,0,180])
            text("Power Train®",size=4,halign="center",valign="center",
                 font="Arial:style=Italic");
        }
        color("OrangeRed")
        union(){
            translate([-30, 14, -9+dz])
            rotate([0,-90,0])
            cylinder(d=10, h=20);
            translate([-50, 14, -24+dz])
            cylinder(d=10, h=20);
        };
        color("DarkTurquoise")
        union(){
            translate([-30, 0, -9+dz])
            rotate([0,-90,0])
            cylinder(d=10, h=20);
            translate([-50, 0, -24+dz])
            cylinder(d=10, h=20);
        };
    }
}


module pump_pedestal(dz){
    color("gray")
    difference(){
        union(){
            translate([-31, -31, dz])
            cube([62,62,3]);
            rotate([0,0,45])
            translate([-61, -6, dz])
            cube([122,12,3]);
            rotate([0,0,-45])
            translate([-61, -6, dz])
            cube([122,12,3]);
        };
        translate([-18, -18, dz-1])
        cube([36,36,5]);
    }
}


module tube(dz){
    color("OrangeRed")
    union(){
        translate([-50, 14, dz])
        rotate([0,90,0])
        cylinder(d=10, h=126);
        translate([-50, 14, -5+dz])
        cylinder(d=10, h=166);
    }
    color("DarkTurquoise")
    union(){
        translate([-50, 0, dz])
        rotate([0,90,0])
        cylinder(d=10, h=126);
        translate([-50, 0, -5+dz])
        cylinder(d=10, h=166);
    }
}

//pump_pedestal(23);