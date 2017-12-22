
Udacity SDC Program 
Term 2 
Project 4
--------------------
Madhu Kakarala

Project 4 : PID Controller

The objective of the project is to use PID Controller system to control a car to drive along
a provided reference trajectory.

1.	Describe the effect each of the P, I, D components had in your implementation.
In simple terms,
-'P' (Proportional) is a measure of how far the car is from the reference (center of the road)
and how much the car needs to steer to move towards center of the road.
-'I' (Integral) is a measure of past error and indicates a constant error factor in the steering i
either to the left or right.
-'D' (Derivative) is a measure of the oscillation around the center line and this parameter can be used to tune'
and minimize the oscillation.

2.	Describe how the final hyperparameters were chosen.
I started with the PID coefficient values provided in the lectures (P = 0.2, I = 0.004, and D = 3.0) and
a throttle of 0.3. The car was too wobbly around the center of the road and was overshooting the track boundary.
I first tuned P to a higher value, but that seem to be over-correcting and did not improve the drive along the track.
The original value of 0.2 or 0.1 seemed optimal.
The Integral component did not have any significant impact indicating that a constant steering offset was not necessary.
The main problem seemed to be with the car oscillating around the
center and this oscllation could be minimized by increasing the D (derivative) coefficient. After tuning a few values of
'D', I arrived at 3. The car was able to drive a full loop of the track with this set of coefficient values,
 although it still had some oscillation. This can possibly be improved further by tuning the 'D' coefficient.
 (The comments in the code in main.cpp:40-57 outline the iterative tuning process)

Other adjustments:
1. I updated the original code to use command line arguments so that the PID coefficients could be passed in as
arguments. The default values are P=0.2 I=0 D=3 if no arguments are passed in. This helped with speeding up the
process of tuning the coefficients by iteratively passing in different values for teh coefficients without having
to change the code and recompile each time.
2. The car was overshooting the tracks on some turns. This occured because the car was going too fast to adjust on turns.
I addressed this issue by  scalling down the throttle value during turns if the car was moving at speeds higher
that 15mph. This resolved the overshooting issue.
3. Twiddle is a good way to arrive at the optimal set of PID coefficients instead of the manual tuning method.
However the simulator does not allow for fast calculation of the CTE for each set of parameters. This limits our ability
to use Twiddle algorithm to tune the parameters.

The video of the car driving around the track with the tuned PID parameters is available here:
 https://www.youtube.com/watch?v=D1UfXftXF0s&feature=youtu.be

