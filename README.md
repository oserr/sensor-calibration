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

### Data fields from iOS app
PowerSense, the iOS app, provides most of the data listed below, but we only use
a subset of it.

0. timestamp(unix) -> time\_unix
1. attitude\_roll(radians)
2. attitude\_pitch(radians)
3. attitude\_yaw(radians)
4. rotation\_rate\_x(radians/s) -> rotate\_x
5. rotation\_rate\_y(radians/s) -> rotate\_y
6. rotation\_rate\_z(radians/s) -> rotate\_z
7. gravity\_x(G)
8. gravity\_y(G)
9. gravity\_z(G)
10. user\_acc\_x(G) -> acc\_x
11. user\_acc\_y(G) -> acc\_y
12. user\_acc\_z(G) -> acc\_z
13. magnetic\_field\_x(microteslas) -> mag\_x
14. magnetic\_field\_y(microteslas) -> mag\_y
15. magnetic\_field\_z(microteslas) -> mag\_z
16. latitude(degree)
17. longitude(degree)
18. altitude(meter)
19. speed(m/s)
20. course(degree)
21. timestamp(unix)
22. User Mark



[1]: http://x-io.co.uk/open-source-imu-and-ahrs-algorithms/
[2]: https://github.com/MarkusLange/RTCDue
