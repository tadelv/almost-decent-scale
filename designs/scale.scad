use <fillets/fillets3d.scad>;
use <fillets/fillets2d.scad>;
use <threads/threads.scad>;

diameter=2.5;
boxWidth=75;
wallThickness=2;
loadCellLength=45;

//base();
//loadCell();
//internals();
//top_cover();
#bottom_cover();
%internals();

translate([boxWidth + 30, 0, 0]) {
    top_cover();
}



module top_cover() {
base();
internals();
}

module bottom_cover() {
diagonal = sqrt(loadCellLength*loadCellLength)*0.5;
mountPointXPos=boxWidth/2 - diagonal;
mountPointYPos=boxWidth/2 + diagonal;
difference() {
    union() {
        cover();        
        translate([mountPointXPos, mountPointYPos, 0]) {
            color("#f0f")
            topFillet(t=3,r=4,s=40)
            linear_extrude(height=2)
            rounding2d(5)
            fillet2d(5)
            square(16, center=true);
        }
    }
    holeOffset = -4.5;
    translate([mountPointXPos, mountPointYPos, 0]) {
        rotate(45) {
    translate([0, holeOffset, -1]) {
        cylinder(2, d=diameter + 1, true, $fn=30);
                    #screwPoint(5);
                }
                translate([0, holeOffset + 7.5, -1]) {
                    cylinder(2, d=diameter + 1, true, $fn=30);
                    #screwPoint(5);
                }
                }
    }
}
}

module loadCell() { 
    difference() {
        cube([9, 45, 6]);
        translate([4.5, 4, -1]) {
            screwPoint();
        }
        translate([4.5, 11.5, -1]) {
            screwPoint();
        }
        translate([4.5, 45 - 11.5, -1]) {
            screwPoint();
        }
        translate([4.5, 45 - 4, -1]) {
            screwPoint();
        }
    }
}

module screwPoint(height=12) {
    ScrewThread(diameter, height)
    cylinder(height, d=1);
}

module base() {
difference() {
        cover(z=10);
        translate([2, 2, 1]) {
            cover(boxWidth - 2*wallThickness, 10);
        }
    }
}

module cover(x=boxWidth, z=1) {
    linear_extrude(height=z)
    rounding2d(5)
    fillet2d(5)
    square([x,x]);
}

module internals() {
connectionCenter=boxWidth/2;// - 5.5; //- (12.1 /2);

translate([connectionCenter, connectionCenter, 1]) {
        rotate(a=45) {
            color("#f00")
            difference() {
                topFillet(t=3,r=2,s=40) {
                    translate([-5.5, -7.5, 0]) {
                        cube([11, 18, 3], center=false);
                    }
                }
                translate([0, 4 - 6, 1]) {
                    #screwPoint(3);
                }
                translate([0, 11.5 - 6, 1]) {
                    #screwPoint(3);
                }
            }
            color("#0f0")
            translate([-4.5, -6, 3]) {
                %loadCell();
            }
        }
}
espMove = wallThickness + 2;
translate([espMove + 37.5 + 10, espMove - 2, 2])
rotate(90)
//translate([-25, -20, 0])
%esp32s3mini();
translate([boxWidth - 25, boxWidth - 40, 2])
hx711();
}

module esp32s3mini() {
    color("#f0f")
    cube([26.5, 37.5, 5]);
} 

module hx711() {
    color("#0f2")
    cube([22.5, 30.2, 3]);
}