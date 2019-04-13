#!/usr/bin/env python

import numpy
import cv2
import cv_bridge
import rospy

from sensor_msgs.msg import Image, LaserScan
from geometry_msgs.msg import Twist

kernel = numpy.ones((50, 20), numpy.uint8)

class commanding_velocity:
    		
    def __init__(self):
        self.bridge = cv_bridge.CvBridge()
        self.image_sub = rospy.Subscriber('/camera/rgb/image_raw', Image,
                                          self.image_callback)
                                       
        self.cmd_vel_pub = rospy.Publisher('/mobile_base/commands/velocity', Twist,
                                           queue_size=1)
                                           
        self.image_sub_depth = rospy.Subscriber('/scan', LaserScan, self.cb)
        
        self.twist = Twist()
        
        self.foundAll = False;
        self.foundSomething = False;
        self.foundRed = False;
        self.foundYellow = False;  
        self.foundGreen = False;  
        self.foundBlue = False;  
        self.canMove = True;
        self.turnLeft = False;
        
    ##Laser Callback                                    
    def cb(self, incoming_data):
        ##Loops through every piece of data in the callback under .ranges
        for x in range(len(incoming_data.ranges)):
             ##if the range is less than 0.7 or is nothing, the robot stops moving
            if(incoming_data.ranges[x] < 0.7 or incoming_data.ranges[x] == None):                
                self.canMove = False
           ##if the robot is over 2 away, then it will move again  
            ##could change the x to 320 (middle)
        if(incoming_data.ranges[320] > 2 and self.canMove == False):
            self.canMove = True
            

    ##Rgb Camera Callback
    def image_callback(self, msg):
        ##Creates a window named camera View
        cv2.namedWindow("Camera View", 1)
        image = self.bridge.imgmsg_to_cv2(msg, desired_encoding='bgr8')
        hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
        
        ##Creates arrays with the colour thresholds
        lower_red = numpy.array([0, 100, 100])
        upper_red = numpy.array([10, 255, 255])
        
        lower_yellow = numpy.array([20, 100, 100])
        upper_yellow = numpy.array([30, 255, 255])
        
        lower_green = numpy.array([50, 100, 100])
        upper_green = numpy.array([70, 255, 255])
        
        lower_blue = numpy.array([110, 100, 100])
        upper_blue = numpy.array([130, 255, 255])
         
        ##Creates the colour masks for each colour between the ranges given above 
        redMask = cv2.inRange(hsv, lower_red, upper_red)
        yellowMask = cv2.inRange(hsv, lower_yellow, upper_yellow)
        greenMask = cv2.inRange(hsv, lower_green, upper_green)
        blueMask = cv2.inRange(hsv, lower_blue, upper_blue)
        
        h, w, d = image.shape
        search_top = 0
        search_bot = h
        
        redMask[0:search_top, 0:w] = 0
        redMask[search_bot:h, 0:w] = 0
        
        yellowMask[0:search_top, 0:w] = 0
        yellowMask[search_bot:h, 0:w] = 0
        
        greenMask[0:search_top, 0:w] = 0
        greenMask[search_bot:h, 0:w] = 0
        
        blueMask[0:search_top, 0:w] = 0
        blueMask[search_bot:h, 0:w] = 0

	
        redMask = cv2.erode(redMask, kernel)
        RM = cv2.moments(redMask)
        
        yellowMask = cv2.erode(yellowMask, kernel)
        YM = cv2.moments(yellowMask)
        
        greenMask = cv2.erode(greenMask, kernel)
        GM = cv2.moments(greenMask)
        
        blueMask = cv2.erode(blueMask, kernel)
        BM = cv2.moments(blueMask)   
        

##########################################################################  
        
        ##The robot hasn't found all 4 colours
        if (self.foundAll == False): 
            ##Prints to see if it has found the colours, and if the robot can move
            print ("Yellow Found : ",self.foundYellow)  
            print ("Red Found : ",self.foundRed)
            print ("Green Found : ",self.foundGreen)
            print ("Blue Found : ",self.foundBlue) 
            print ('Can Move? : ',self.canMove)
            
            ##If the robot cannot move            
            if(self.canMove == False):          
                if(self.turnLeft == False):
                    ##If the robot is turning Right
                    self.twist.angular.z = -1  
                else:
                    ##If the robot is turning Left
                    self.twist.angular.z = 1
                ##Robot reverses a bit
                self.twist.linear.x = -0.4                
                self.twist.angular.x = 0
                ##Publishes to the twist topic
                self.cmd_vel_pub.publish(self.twist)
            
            ##If the robot can move
            if(self.canMove == True):
                ##If it hasn't found yellow
                ##Checks to see if the camera can see the colour in the yellow Mask
                if(YM['m00'] != 0 and self.foundYellow == False):
                    ##Creates a circle at where the robot sees the colour
                    cx = int(YM['m10']/YM['m00'])
                    cy = int(YM['m01']/YM['m00'])
                    cv2.circle(image, (cx, cy), 20, (0, 0, 255), -1)
                    ##adjusts the error for whilst it is moving, so if it sees a colour it moves towards it
                    err = cx - w/2
                    self.foundSomething = True
                    ##if the colour is closer to the robot set it to found
                    if yellowMask[h-1, w/2] != 0:
                        self.twist.linear.x = 0
                        self.foundYellow = True
                        self.FoundSomething = False
                        print('Yellow has been found!')
                    else:
                        ##if not, move towards the colour at 0.7
                        if(self.canMove == True):
                            self.twist.linear.x = 0.7
                else:
                    ##    LOOKING FOR MORE
                    ##if all hav been found, stop searching
                    if(self.foundYellow == True and self.foundRed == True and self.foundGreen == True and self.foundBlue == True):
                        self.foundAll = True
                        print('All colours have been found!')
                    else:   
                        ##toggles the direction to rotate
                        if(self.canMove == True):
                            if(self.turnLeft == True):
                                self.turnLeft = False
                            else:
                                self.turnLeft = True
                                ##move forward to find more
                            self.twist.angular.z = 0
                            self.twist.linear.x = 0.7
                            self.foundSomething = False
                            self.cmd_vel_pub.publish(self.twist)
                            #print('Searching')
                
                ##repeats for all the colours                
                
                if(RM['m00'] != 0 and self.foundRed == False):
                    cx = int(RM['m10']/RM['m00'])
                    cy = int(RM['m01']/RM['m00'])
                    cv2.circle(image, (cx, cy), 20, (0, 0, 255), -1)
                    err = cx - w/2
                    self.foundSomething = True
                    if redMask[h-1, w/2] != 0:
                        self.twist.linear.x = 0
                        self.foundRed = True
                        self.FoundSomething = False
                        print('Red has been found!')
                    else:
                        if(self.canMove == True):
                            self.twist.linear.x = 0.7
                            print('FORWARD')
                        
                if(BM['m00'] != 0 and self.foundBlue == False):
                    cx = int(BM['m10']/BM['m00'])
                    cy = int(BM['m01']/BM['m00'])
                    cv2.circle(image, (cx, cy), 20, (0, 0, 255), -1)
                    err = cx - w/2
                    self.foundSomething = True
                    if blueMask[h-1, w/2] != 0:
                        self.twist.linear.x = 0
                        self.foundBlue = True
                        self.FoundSomething = False
                        print('Blue has been found!')
                    else:
                        if(self.canMove == True):
                            self.twist.linear.x = 0.7
                            print('FORWARD')
                
                if(GM['m00'] != 0 and self.foundGreen == False): 
                    cx = int(GM['m10']/GM['m00'])
                    cy = int(GM['m01']/GM['m00'])
                    cv2.circle(image, (cx, cy), 20, (0, 0, 255), -1)
                    err = cx - w/2
                    self.foundSomething = True
                    if greenMask[h-1, w/2] != 0:
                        self.twist.linear.x = 0
                        self.foundGreen = True
                        self.FoundSomething = False
                        print('Green has been found!')
                    else:
                        if(self.canMove == True):
                            self.twist.linear.x = 0.7
                            print('FORWARD')
            
            ##puts the updated camera view onto the window
            cv2.imshow("Camera View", image)
            cv2.waitKey(3)
            
            ##takes the error and turns towards it (hence towards the colour) if no colour, no err to adjust
            if(self.foundSomething == True):
                self.twist.angular.z = -float(err) /100
                self.cmd_vel_pub.publish(self.twist)
                   
####################################################################
        
        
cv2.startWindowThread()
rospy.init_node('commanding_velocity')
follower = commanding_velocity()
rospy.spin()

cv2.destroyAllWindows()