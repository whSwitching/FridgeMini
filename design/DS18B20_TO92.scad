
module ds18b20()
{
    union()
    {
        color("black")
        difference()
        {
            cylinder(d=5, h=5, $fn=30);
            translate([-5, -11.2, -2])
            cube([10,10,10]);
        };
        color("silver")
        union()
        {
            translate([-1.55, 0.3, -15])
            cube([0.3,0.3,15]);
            translate([-0.15, 0.3, -15])
            cube([0.3,0.3,15]);
            translate([1.25, 0.3, -15])
            cube([0.3,0.3,15]);
        }
    }
}

