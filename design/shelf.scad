module shelf(dz){
    x = 300;
    y = 230;
    z = 190;
    w = 23;
    h = 14;
    color("darkorange")
    union(){
        translate([-1*x/2, -1*y/2, dz])
        cube([w,h,z-h]);
        translate([-1*x/2, y/2-h, dz])
        cube([w,h,z-h]);
        translate([-1*x/2, -1*y/2, dz+z-h])
        cube([w,y,h]);
    }
    color("darkorange")
    union(){
        translate([x/2-w, y/2-h, dz])
        cube([w,h,z-h]);
        translate([x/2-w, -1*y/2, dz])
        cube([w,h,z-h]);
        translate([x/2-w, -1*y/2, dz+z-h])
        cube([w,y,h]);
    }
    sw = 1.5;
    sh = 10;
    // right
    color("silver")
    translate([x/2-w-sw, -1*y/2, dz+35])
    cube([sw,y,sh]);
    color("silver")
    translate([x/2-w-sw, -1*y/2, dz+35+105])
    cube([sw,y,sh]);
    // left
    color("silver")
    translate([-x/2+w, -1*y/2, dz+25])
    cube([sw,y,sh]);
    color("silver")
    translate([-x/2+w, -1*y/2, dz+25+80])
    cube([sw,y,sh]);
}

module shelf_power_M4(){
    color("white")
    union(){
        translate([-126, -53, -80])
        rotate([0,90,0])
        cylinder(d=4, h=6);
        translate([-127, -53, -80])
        rotate([0,-90,0])
        cylinder(d=6, h=1);
    }
    color("white")
    union(){
        translate([-126, -53, -160])
        rotate([0,90,0])
        cylinder(d=4, h=6);
        translate([-127, -53, -160])
        rotate([0,-90,0])
        cylinder(d=6, h=1);
    }
    color("white")
    union(){
        translate([-126, 52, -80])
        rotate([0,90,0])
        cylinder(d=4, h=6);
        translate([-127, 52, -80])
        rotate([0,-90,0])
        cylinder(d=6, h=1);
    }
    color("white")
    union(){
        translate([-126, 52, -160])
        rotate([0,90,0])
        cylinder(d=4, h=6);
        translate([-127, 52, -160])
        rotate([0,-90,0])
        cylinder(d=6, h=1);
    }
}
