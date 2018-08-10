"# ArduinoFiles" 

GPS001:
- Uses GPS
- Has the 'Card Buffer'
- Uses SD Card
- Runs a timer interupt
In general looks good even if a little over kill


Motorcycle_DataLogger:
- Uses SD Card
- Uses 6Axis sensor
- uses I2C wire communication ?? for sensor ??
- Runs a timer interupt
- Every thing done in timer
Could be good to combine the GPS above with this.


Prototype_DataLogger:
Much like GPS001 but more advanced.
Combine this and the motorcycle one above,
remove the card buffer and sd card logic
add some logic for reading digital and analogue inputs

Need to get the blue tooth connection for Race Chrono
Need a new mega???
Whats the difference in coding between Mega and Due??


