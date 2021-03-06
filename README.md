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

## Repo Info
* `ProjectReport.pdf`: The final project report.
* `data`: Contains data collected in experiments.
* `guidelines`: Project guidelines.
* `gyro`: The PowerDue module used to collect readings from the MPU9250.
* `madgwick`: Madgwick MATLAB algorithm scripts.
    * `Example2.m`: Script to apply orientation transformation.
* `milestone`: Project milestone report.
* `mpu9250`: Git submodule for the git repo of the MPU9250 library.
* `report`: The latex files for the final project report.
* `rtcdue`: Git submodule for the git repo of the RTCDue library.
* `context.ipynb`: IPython notebook implementing, training, and evaluating the
   neural network model.
* `convert_raw_data.py`: Python script to aggregate the raw data into files
   with readings from the same time.
* `environment-linux.yml`: A conda environment file that can be used to create
   the development environment needed to replicate the work on a Linux machine.
* `environment-mac.yml`: A conda environment file that can be used to create
   the development environment needed to replicate the work on a Mac.

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


## Replicating analysis
To replicate the work on your end, you can follow the instructions below. Note
the instuctions below are for Linux and Mac. You might be able to use the same
environment files on Windows, however, if there are errors, then you can create
the environment manually with conda by creating an environment with the
following packages: pandas, tensorflow, sickit-learn, jupyter, matplotlib,
numpy, scipy, kears, and python=3.

1. [Install][3] anaconda toolchain.
2. From within the project folder, run
   `conda env create -n envname -f envfile.yml`. This will install the packages
   listed in `envfile.yml` in an environment named `envname`.
    * To install for Linux, use the file `environment-linux.yml`.
    * To install for Mac, use the file `environment-mac.yml`.
    * If you want to create the environment file locally instead of globally,
      use the `-p envpath` instead of `-n envfile`.
3. Now that the environment has been created, you can activate the environment
   by running `source activate envname`, where `envname` is either the name of
   the global environment or the path to the local environment.

The steps above create the environment. To repliate the results, proceed by

1. Create the input data files: run `python convert_raw_data.py`.
2. Launch the jupyter notebook: `jupyter notebook`.
3. From the browser tab opened by jupyter, launch `context.ipynb` by double
   clicking on it.
4. Run all the cells in `context.ipynb`.

Note that these instructions install TensorFlow without GPU support. To
install TensorFlow with GPU support, use conda to uninstall `tensorflow` and
then install `tensorflow-gpu`.

[1]: http://x-io.co.uk/open-source-imu-and-ahrs-algorithms/
[2]: https://github.com/MarkusLange/RTCDue
[3]: https://conda.io/docs/user-guide/install/index.html
