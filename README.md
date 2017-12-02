# Sensor Calibration
A project for *18-847: Wireless Software Systems Architecture* at Carnegie
Mellon University.

## Purpose
The explosion of wireless devices has created new opportunities to enhance
security protocols by leveraging sensor data in wireless devices, but a key
challenge in using sensor data from multiple devices is the fact that wireless
devices rarely share the same orientation, making it difficult to correlate
data from multiple devices. Therefore, we explore sensor calibration techniques
to solve this problem.

## Misc Notes
* PowerDue has MPU\_9250, which has a gyroscope and accelerometer.
* The SparkFun library for the sensor can be updated with the correct address
  to work with MPU\_9250. The address is 0b1101000.
* Madgwick MATLAB scripts found [here][1].
* RTC PowerDue library found [here][2].

## Experimental Data
The experimental data is located under `data`, and is is organized as follows:

* `android`: contains data obtained from ArduinoSense Android app.
* `ios`: contains data obtained from PowerSense iOS app.
* `powerdue`: contains data obtained from PowerDue.

Each directory then contains data for three experiments:

1. Devices are share context in moving vehicle.
2. Devices don't share context as one is with pedestrian and the other in moving
   vehicle.
3. Devices don't share context as each is located in a different moving vehicle.

[1]: http://x-io.co.uk/open-source-imu-and-ahrs-algorithms/
[2]: https://github.com/MarkusLange/RTCDue
