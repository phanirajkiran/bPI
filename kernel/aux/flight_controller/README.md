### Flight Controller ###

This folder contains a flight controller for a quadrocoper in X configuration. 
Most of the configuration can be done in ./main.cpp.


#### Features ####
- Read inputs via PWM or PPM sum from an RC receiver
- 9/10 Dof sensor inputs (gyro+accel+mag+baro)
- Different sensor fusion algorithms for attitude stabilization
- Configurable PID controllers for Yaw, Pitch & Roll
- Motor controlling via 16 channel Adafruit I2C PWM board
- Command line interface via UART for debugging & statistics output

#### Usage ####
Make sure `flight_controller_main()` is called from `kernel/main.cpp` to enable
the flight controller.

After a short startup delay the green LED should blink. This means it waits for
the start condition. Then it changes to flying mode and it should blink faster.

start condition: an additional input switch is used for this. you need >4
receiver channels for this to work. if the switch has 3 states, the middle state
is used for manual flight mode: PID flight stabilization is then disabled.

##### Debugging #####
For testing, enable FLIGHT_CONTROLLER_DEBUG_MODE in common.hpp.

