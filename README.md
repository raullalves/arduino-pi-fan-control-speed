# Digital PI speed control for a 4-pin fan

This project was part of a module in my college, Federal University of Bahia.
A Fan can be moddeled as a first order DC motor. Using that approach, the code applies a degree PWM, with is the max value (255), and measures the gain and the time constant, aiming to get an approximative first order transfer function.
Consequently, it calculates the proportional and integral gains, according to the values of mp (overshoot) and ts (steady time), which can me changed according to the user requirements.
Thus, it gets the potenciometer analog value, converts to a RPM speed scale, and assign that value as a set pointer to the controller.
Then, the PI controller lets the magic happens

# MATLAB
Matlab can plot the real time data obtained from a serial connection through arduino.

Any doubts, feel free to ask
