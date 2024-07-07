use <fillets/fillets3d.scad>;
use <fillets/fillets2d.scad>;
use <chamfers/Chamfer.scad>;
use <threads/threads.scad>;

$fn=45;

diameter=4;
boxWidth=80;
wallThickness=2;
loadCellLength=45;

//base();
//loadCell();
//internals();
//top_cover();
%bottom_cover();
%internals();


module support() {
    translate([-5,0,0])
    rotate([0,-90,0])
    translate([3,0,0])
    hull()
    {
        translate([2,2,0]) sphere(r=0.5);
        translate([-2,2,0]) sphere(r=0.5);
        translate([-2,-2,0]) sphere(r=1.5);
        translate([2,26,0]) sphere(r=0.5);
        translate([-2,30,0]) sphere(r=1.5);
    }
}


translate([boxWidth + 30, 0, 0]) {
    top_cover();
        top_cover();
}

module top_cover_inverted() {
    translate([boxWidth, boxWidth, 14]) {
        rotate([0,180,90]) {
            top_cover();
        }
    }
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
        topBottomFillet(t=1.5,b=0,r=1,s=30)
        cover(z=1.5);
        translate([mountPointXPos +0.5, mountPointYPos -0.5, 0]) {
            color("#f0f")
            topFillet(t=4,r=4,s=40)
            linear_extrude(height=4)
            rounding2d(5)
            fillet2d(5)
            square(16, center=true);
            translate([11,-15,0.5])
            rotate(45)
            support();
            translate([22,-4,0.5])
            rotate(45)
            support();
        }
    }
    holeOffset = -4.5;
    counterHoleDiameter = diameter+1.5;//diameter + 1.5;
    translate([mountPointXPos, mountPointYPos, 0]) {
        rotate(45) {
    translate([0, holeOffset, 0]) {
    cylinder(h=3, r1=(diameter+3)/2, r2=diameter/2);
                    #screwPoint(15);
                }
                translate([0, holeOffset + 7.5, 0]) {
                cylinder(h=3, r1=(diameter+3)/2, r2=diameter/2);
                    #screwPoint(15);
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
    cylinder(height, d=2);
}

module base() {
difference() {
        topBottomFillet(t=10, b=0,r=1,s=30)
        cover(z=10);
        translate([2, 2, 1]) {
            cover(boxWidth - 2*wallThickness, 10);
        }
    }
}

module cover(x=boxWidth, z=1) {
// topBottomFillet(t=1.5,b=0,r=0.5,s=10)
    linear_extrude(height=z)
    rounding2d(5)
    fillet2d(5)
    square([x,x]);
}

module internals() {
connectionCenter=boxWidth/2;// - 5.5; //- (12.1 /2);

translate([connectionCenter, connectionCenter, 1]) {
    #%rotate(a=45) {
        color("#f00")
//        difference() {
//            topFillet(t=3,r=2,s=30) {
//                translate([-5.5, -7.5, 0]) {
//                    cube([11, 18, 3], center=false);
//                }
//            }
//            translate([0, 4 - 6, 1]) {
//                #screwPoint(3);
//            }
//            translate([0, 11.5 - 6, 1]) {
//                #screwPoint(3);
//            }
//        }
        color("#0f0")
        translate([-4.5, -6, 3]) {
            #%loadCell();
        }
    }
    rotate(45)
    translate([0,0,0])
    difference() {
//        topFillet(t=2,r=3,s=30)
        translate([0,2,0])
        hull() {
            cube([11,15, 0.1], center = true);
            translate([0,0,2])
            cube([9,12,1], center = true);
            translate([0, 20, 0])
            cube([2, 20, 0.1], center = true);
        }
        translate([0, 4 - 6, 1]) {
            #screwPoint(3);
        }
            translate([0, 11.5 - 6, 1]) {
            #screwPoint(3);
        }
    }
}

// esp32s3 connection
espMove = wallThickness + 2;
translate([espMove + 37.5 + 10, espMove, 2])
rotate(90) {
    %esp32s3mini();
    translate([-3, 38,-1])
    hull() {
        cube([3, 2, 5]);
        translate([30, 0, 0])
        cube([1, 2, 0.1]);
    }
    translate([-3, -3,-1])
    hull() {
        cube([5, 2, 5]);
        translate([15,0,0])
        cube([1, 2, 0.1]);
    }
}

// hx711 connection
translate([boxWidth - 25, boxWidth - 40, 2]) {
%hx711();
        translate([2.5,2.5,-2])
        cylinder(r1=2,r2=1.5, h=4);
        translate([2.5,27.7,-2])
        cylinder(r1=2,r2=1.5, h=4);
        translate([20,27.7,-2])
        cylinder(r1=2,r2=1.5, h=4);
        translate([20,2.5,-2])
        cylinder(r1=2,r2=1.5, h=4);
}
}

module esp32s3mini() {
    color("#f0f")
    cube([26.5, 37.5, 5]);
} 

module hx711() {
    difference() {
        color("#0f2")
        cube([22.5, 30.2, 3]);
        translate([2.5,2.5,0])
        cylinder(r=1.9, h=4);
        translate([2.5,27.7,0])
        cylinder(r=1.9, h=4);
        translate([20,27.7,0])
        cylinder(r=1.9, h=4);
        translate([20,2.5,0])
        cylinder(r=1.9, h=4);
    }
}