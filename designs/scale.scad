use <fillets/fillets3d.scad>;
use <fillets/fillets2d.scad>;
use <chamfers/Chamfer.scad>;
use <threads/threads.scad>;

$fn=45;

diameter=4;
boxWidth=86;
wallThickness=3;
loadCellLength=45;

//base();
//loadCell();
//internals();
//top_cover();
bottom_cover();
//%internals();
//#%top_cover_inverted();
translate([boxWidth + 30, 0, 0]) {
        top_cover();
        //round_wall();
}

module round_wall(length = 20, height = 3) {
    topBottomFillet(t = height, b = 0, s = 20, r = height/3)
    rotate_extrude(angle = -90, convexity = 5) {
        difference() {
            square([length, height], center = false);
            square([length - height, height]);
            
        }
    }
}

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



module top_cover_inverted() {
    translate([boxWidth, boxWidth, 19]) {
        rotate([0,180,90]) {
            top_cover();
        }
    }
}


module top_cover() {
difference() {
union() {
#base();
translate([0,0,3]) {
    #internals();
}
translate([0, boxWidth, 1]) {
        round_wall(length = boxWidth * 0.8, height = 4);
        }
        
        translate([boxWidth, 0, 1]) {
        rotate(180)
                round_wall(length = boxWidth * 0.8, height = 4);
        }
        
        translate([0, 0, 1]) {
        rotate(90)
                round_wall(length = boxWidth * 0.8, height = 4);
        }
        
        translate([boxWidth, boxWidth, 1]) {
        rotate(-90)
                round_wall(length = boxWidth * 0.8, height = 4);
        }
}
connectionCenter=boxWidth/2;
translate([connectionCenter, connectionCenter, 1]) {
    #rotate(a=45) {
    translate([0, 4 - 6, 0]) {
            #screwPoint(15);
        }
            translate([0, 11.5 - 6, 0]) {
            #screwPoint(15);
        }
    }
}
}
}

module bottom_cover() {
    diagonal = sqrt(loadCellLength*loadCellLength)*0.5;
    mountPointXPos=boxWidth/2 - diagonal;
    mountPointYPos=boxWidth/2 + diagonal;
    height = 3;
    difference() {
        union() {
            topBottomFillet(t=height,b=0,r=1,s=30)
            cover(z=height);
            translate([mountPointXPos +0.5, mountPointYPos -0.5, 0]) {
                color("#f0f")
                topFillet(t=6,r=4,s=40)
                linear_extrude(height=6)
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


            translate([0, boxWidth, 1]) {
            round_wall(length = boxWidth * 0.8);
            }
            
            translate([boxWidth, 0, 1]) {
            rotate(180)
            round_wall(length = boxWidth * 0.8);
            }
            
            translate([0, 0, 1]) {
            rotate(90)
            round_wall(length = boxWidth * 0.8);
            }
            
            translate([boxWidth, boxWidth, 1]) {
            rotate(-90)
            round_wall(length = boxWidth * 0.8);
            }
            
        #innerWall(height);

            // esp32s3 connection
        espMove = wallThickness + 2;
        translate([espMove + 37.5 + 12, espMove, height])
        rotate(90) {
            #%esp32s3mini();
            translate([-2, 36,-1])
            difference() {
                hull() {
                    cube([1, 2, 5]);
                    translate([30, 0, 0])
                    cube([1, 2, 5]);
                    translate([33,0,0])
                    cube([1, 2, 0.1]); 
                    translate([2,5,0])
                    cube([20,10,0.1]);
                }
                translate([4,1,2])
                hull() {
                    sphere(d=4);
                    translate([26,0,0])
                    sphere(d=4);
                }
            }
            translate([-1, -0.9,-1])
            hull() {
                cube([5, 2, 5]);
                translate([15,0,0])
                cube([1, 2, 0.1]);
            }
            
        }

            // hx711 connection
            rotate(45)
            translate([boxWidth * 0.85, boxWidth * -0.15, 4.9]) {
            %hx711();
                footHeight = 5;
                    translate([2.5,2.5,-2])
                    color("#f00")
                    cylinder(r1=2,r2=1.5, h=footHeight);
                    
                    translate([2.5,27.7,-2])
                    color("#ff0")
                    cylinder(r1=2,r2=1.5, h=footHeight);

                    translate([20,27.7,-2])
                    color("#f0f")
                    cylinder(r1=2,r2=1.5, h=footHeight);
                    
                    translate([20,2.25,-2])
                    color("#0ff")
                    cylinder(r1=2,r2=1.5, h=footHeight);
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

module innerWall(height) {
    innerWallOffset = wallThickness * 1.2;
    color("red")
    
    translate([innerWallOffset, innerWallOffset, 0])
    difference() {
    #topFillet(t = height * 3, r = 0.5, s = 30)
    linear_extrude(height = height * 3)
//    rounding2d(1.01)
//        fillet2d(0.9)
            fillet2d(5)
            difference() {
              rounding2d(2)
              square(boxWidth - 2 * innerWallOffset);
              translate([1, 1, 0])
              rounding2d(1)
              square(boxWidth - 2 * innerWallOffset - 2);
            }
      translate([boxWidth - innerWallOffset * 2.8, 16, height * 2.5]) {
//            sphere(r=wallThickness * 1.2);
    rotate([0,90,0])
    #cylinder(h = 5, r = wallThickness * 1.2);
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
        topBottomFillet(t=12, b=0,r=1,s=30)
        cover(z=12);
        translate([wallThickness, wallThickness, wallThickness]) {
            cover(boxWidth - 2*wallThickness, 10);
        }
        translate([boxWidth - 19.5, -1, 12]) {
//            sphere(r=wallThickness);
rotate([0, 90, 90])
cylinder(5, r = wallThickness * 1.2);
        }
}
}

module cover(x=boxWidth, z=1) {
// topBottomFillet(t=1.5,b=0,r=0.5,s=10)
    linear_extrude(height=z)
    rounding2d(2)
    square([x,x]);
}

module internals() {
    connectionCenter=boxWidth/2;

    translate([connectionCenter, connectionCenter, 1]) {
        #%rotate(a=45) {
            color("#0f0")
            translate([-4.5, -6, 3]) {
                #%loadCell();
            }
        }
        rotate(45)
        translate([0,0,0])
        difference() {
            translate([0,1,-1])
            hull() {
                cube([11,15, 0.1], center = true);
                translate([0,0,3])
                cube([9,13,1], center = true);
                translate([0, 20, 0])
                cube([2, 20, 0.1], center = true);
            }
        }
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