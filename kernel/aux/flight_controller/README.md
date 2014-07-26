### Flight Controller ###

This folder contains a flight controller for a quadrocoper in X configuration. 
Most of the configuration can be done in ./main.cpp.


#### Features ####
- Read inputs via PWM or PPM sum from an RC receiver
- 9/10 Dof sensor inputs (gyro+accel+mag+baro)
- Different sensor fusion algorithms for attitude stabilization
- Configurable PID controllers for Yaw, Pitch & Roll
- Motor controlling via 16 channel Adafruit I2C PWM board
- Command line interface via UART for debugging

#### Usage ####
Make sure `flight_controller_main()` is called from `kernel/main.cpp`

