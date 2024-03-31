module pump_M4(h){
    color("white")
    union(){
        translate([40, 40, 10+h])
        cylinder(d=4, h=16);
        translate([40, 40, 26+h])
        cylinder(d=8, h=1);
    };
    color("white")
    union(){
        translate([40, -40, 10+h])
        cylinder(d=4, h=16);
        translate([40, -40, 26+h])
        cylinder(d=8, h=1);
    };
    color("white")
    union(){
        translate([-40, -40, 10+h])
        cylinder(d=4, h=16);
        translate([-40, -40, 26+h])
        cylinder(d=8, h=1);
    };
    color("white")
    union(){
        translate([-40, 40, 10+h])
        cylinder(d=4, h=16);
        translate([-40, 40, 26+h])
        cylinder(d=8, h=1);
    };
}

module shelf_heatSink_M4(){
    color("white")
    union(){
        translate([126, -53, -45])
        rotate([0,-90,0])
        cylinder(d=4, h=6);
        translate([127, -53, -45])
        rotate([0,90,0])
        cylinder(d=6, h=1);
    }
    color("white")
    union(){
        translate([126, -53, -150])
        rotate([0,-90,0])
        cylinder(d=4, h=6);
        translate([127, -53, -150])
        rotate([0,90,0])
        cylinder(d=6, h=1);
    }
    color("white")
    union(){
        translate([126, 52, -45])
        rotate([0,-90,0])
        cylinder(d=4, h=6);
        translate([127, 52, -45])
        rotate([0,90,0])
        cylinder(d=6, h=1);
    }
    color("white")
    union(){
        translate([126, 52, -150])
        rotate([0,-90,0])
        cylinder(d=4, h=6);
        translate([127, 52, -150])
        rotate([0,90,0])
        cylinder(d=6, h=1);
    }
}

module shelf_box_M4(){
    color("white")
    union(){
        translate([-138, -80, -16])
        cylinder(d=4, h=40);
        translate([-138, -80, 23])
        cylinder(d=8, h=1);
        translate([-138, 80, -16])
        cylinder(d=4, h=40);
        translate([-138, 80, 23])
        cylinder(d=8, h=1);
        translate([138, 80, -16])
        cylinder(d=4, h=40);
        translate([138, 80, 23])
        cylinder(d=8, h=1);
        translate([138, -80, -16])
        cylinder(d=4, h=40);
        translate([138, -80, 23])
        cylinder(d=8, h=1);
    };
}

