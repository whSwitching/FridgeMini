include<box.scad>
include<shelf.scad>
include<plate.scad>
include<ForzenMagic120_pump.scad>
include<ForzenMagic120_heatsink.scad>
include<DS18B20_TO92.scad>
include<powerSupply.scad>
include<screws.scad>

//box_cover(210);
//box(0);
shelf(-190);
shelf_box_M4();

ForzenMagic120_heatsink();
ForzenMagic120_heatfan();
heatfan_heatsink_M4();

shelf_heatSink_M4();

aluminium_1060_plate(20);

translate([-35, -35, 23.1])
box_fan();

// TEC1 cool side temp sensor
translate([0, -39, 24])
rotate([90,0,180])
ds18b20();

// TEC1 heat side temp sensor
translate([-25.5, 0, 17])
rotate([90,0,90])
ds18b20();

// box temp sensor
translate([0, -92, 110])
ds18b20();

TEC1_12706(16);
ForzenMagic120_pump(16);
pump_M4(-3);
tube(-170);

// power
translate([-125,100,-70])
rotate([-90,0,-90])
power();
shelf_power_M4();
