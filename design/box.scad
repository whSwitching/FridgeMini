include<ForzenMagic120_pump.scad>

module box(dz){
    x = 340;
    y = 230;
    z = 210;
    t = 20;
    color("whitesmoke")
    difference(){
        difference(){
            translate([-1*x/2, -1*y/2, dz])
            cube([x,y,z]);
            translate([-1*x/2+t, -1*y/2+t, dz+t])
            cube([x-2*t,y-2*t,z-t+5]);
        };
        translate([0, 0, dz-5])
        linear_extrude(height = 30, twist = 0, slices = 20){
            projection(false)
            ForzenMagic120_pump(dz);
        }
    }
}

module box_cover(dz){
    x = 340;
    y = 230;
    t = 20;
    color("Gainsboro")
    translate([0,-90,90])
    rotate([-30,0,0])
    union(){
        translate([-1*x/2, -1*y/2, dz])
        cube([x,y,t]);
        difference(){
            translate([-1*x/2+t, -1*y/2+t, dz-5])
            cube([x-2*t,y-2*t,5]);
            translate([-1*x/2+t+10, -1*y/2+t+10, dz-6])
            cube([x-2*t-20,y-2*t-20,10]);
        };
    }
}
