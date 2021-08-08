# Room-Mapping-Robot

### Summary

This project utilizes the TI RSLK MAX robotics kit and an OPT3101 TOF sensor in order to navigate through and map out a room. It then displays the area on a LCD mounted to the robot.

Designed by Austin Welch and David Hutchins

### Demo Video
https://youtu.be/LeQ4pwptnrw

### Code Structure

In order to succesfully navigate a room and illustrate objects in it with accuracy we designed it to record its positional data of its coordinates relative of its
 starting position and its direction relative to its starting orientation. The orientation of the robot when it is started is set as 0 degrees.
To effectively illustrate the room the positional data in the form of coordinates is converted to a pixel on the LCD through the use of a grid.


#### Initialization

- The robot first displays an option for cell size to the user. The cell size options are 5cm and 10cm and this cell size represents how a pixel on
the display will map to its surroundings. For example if the user selects 10cm each pixel will represent 10cm which means the 200x200 px box on the Touch LCD will be
able to represent an area of 400 sq. meters (20m x 20m).

- Next the square representing the area will display and it will wait for the user to select where in the box they want the robot to start in, for example if you are
at the "bottom" of the room when you place the robot you will be able to represent this by touching the screen near the bottom of the box as its starting location.

#### Main Loop

This loop runs until the robot is shut down or reset so that it is continually mapping the space around it.

- Movement
  - The robot moves forward at 30% of its max speed.
- Sensor Readings
  - The robot then polls the OPT3101's 3 channels to evaluate the distances of objects in front of it. These distances are stored for later use in the navigation
  algorithm.
- Position Update and Plotting
  - The robot then internally records its movement from its last position and updates its current position. It then uses the sensor readings in conjunction with
   trigonometry to draw the objects it has seen between distances 5cm and 85cm away.
- Navigation
  - The robot then uses a navigation algorithm to determine if it needs to adjust its course based on factors such as how close the nearest object is and whether
  the robot is still within the bounds of the predescribed area. The robot after any turns are made then updates it position with the new direction it faces.



### Hardware Used:

TI-RSLK MAX : http://www.ti.com/rslk

Touch LCD BoosterPack BOOSTXL-K350QVG-S1 : https://www.ti.com/tool/BOOSTXL-K350QVG-S1

3-Channel Wide FOV Time-of-Flight Distance Sensor for TI-RSLK MAX Using OPT3101 : https://www.pololu.com/product/3680
