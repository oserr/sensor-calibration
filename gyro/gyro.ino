/* MPU9250 Basic Example Code
 by: Kris Winer
 date: April 1, 2014
 license: Beerware - Use this code however you'd like. If you
 find it useful you can buy me a beer some time.
 Modified by Brent Wilkins July 19, 2016

 Demonstrate basic MPU-9250 functionality including parameterizing the register
 addresses, initializing the sensor, getting properly scaled accelerometer,
 gyroscope, and magnetometer data out. Added display functions to allow display
 to on breadboard monitor. Addition of 9 DoF sensor fusion using open source
 Madgwick and Mahony filter algorithms. Sketch runs on the 3.3 V 8 MHz Pro Mini
 and the Teensy 3.1.

 SDA and SCL should have external pull-up resistors (to 3.3V).
 10k resistors are on the EMSENSR-9250 breakout board.

 Hardware setup:
 MPU9250 Breakout --------- Arduino
 VDD ---------------------- 3.3V
 VDDI --------------------- 3.3V
 SDA ----------------------- A4
 SCL ----------------------- A5
 GND ---------------------- GND
 */

#include "quaternionFilters.h"
#include "MPU9250.h"

#define AHRS true         // Set to false for basic data read
#define SerialDebug true  // Set to true to get SerialUSB output for debugging

// Pin definitions
// int intPin = 12;  // These can be changed, 2 and 3 are the Arduinos ext int pins
int myLed  = 7;  // Set up pin 13 led for toggling

MPU9250 myIMU;

void setup()
{
  // TWBR = 12;  // 400 kbit/sec I2C speed
  SerialUSB.begin(9600);

  // Set up the interrupt pin, its set as active high, push-pull
  //pinMode(intPin, INPUT);
  //digitalWrite(intPin, LOW);
  pinMode(myLed, OUTPUT);
  digitalWrite(myLed, HIGH);

  // Read the WHO_AM_I register, this is a good test of communication
  byte c = myIMU.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
  SerialUSB.print(F("MPU9250 I AM 0x"));
  SerialUSB.print(c, HEX);
  SerialUSB.print(F(" I should be 0x"));
  SerialUSB.println(0x71, HEX);

  if (c == 0x71) // WHO_AM_I should always be 0x71
  {
    SerialUSB.println(F("MPU9250 is online..."));

    // Start by performing self test and reporting values
    myIMU.MPU9250SelfTest(myIMU.selfTest);
    SerialUSB.print(F("x-axis self test: acceleration trim within : "));
    SerialUSB.print(myIMU.selfTest[0],1); SerialUSB.println("% of factory value");
    SerialUSB.print(F("y-axis self test: acceleration trim within : "));
    SerialUSB.print(myIMU.selfTest[1],1); SerialUSB.println("% of factory value");
    SerialUSB.print(F("z-axis self test: acceleration trim within : "));
    SerialUSB.print(myIMU.selfTest[2],1); SerialUSB.println("% of factory value");
    SerialUSB.print(F("x-axis self test: gyration trim within : "));
    SerialUSB.print(myIMU.selfTest[3],1); SerialUSB.println("% of factory value");
    SerialUSB.print(F("y-axis self test: gyration trim within : "));
    SerialUSB.print(myIMU.selfTest[4],1); SerialUSB.println("% of factory value");
    SerialUSB.print(F("z-axis self test: gyration trim within : "));
    SerialUSB.print(myIMU.selfTest[5],1); SerialUSB.println("% of factory value");

    // Calibrate gyro and accelerometers, load biases in bias registers
    myIMU.calibrateMPU9250(myIMU.gyroBias, myIMU.accelBias);

    myIMU.initMPU9250();
    // Initialize device for active mode read of acclerometer, gyroscope, and
    // temperature
    SerialUSB.println("MPU9250 initialized for active data mode....");

    // Read the WHO_AM_I register of the magnetometer, this is a good test of
    // communication
    byte d = myIMU.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
    SerialUSB.print("AK8963 ");
    SerialUSB.print("I AM 0x");
    SerialUSB.print(d, HEX);
    SerialUSB.print(" I should be 0x");
    SerialUSB.println(0x48, HEX);

    if (d != 0x48)
    {
      // Communication failed, stop here
      SerialUSB.println(F("Communication failed, abort!"));
      SerialUSB.flush();
      abort();
    }

    // Get magnetometer calibration from AK8963 ROM
    myIMU.initAK8963(myIMU.factoryMagCalibration);
    // Initialize device for active mode read of magnetometer
    SerialUSB.println("AK8963 initialized for active data mode....");

    if (SerialDebug)
    {
      //  SerialUSB.println("Calibration values: ");
      SerialUSB.print("X-Axis factory sensitivity adjustment value ");
      SerialUSB.println(myIMU.factoryMagCalibration[0], 2);
      SerialUSB.print("Y-Axis factory sensitivity adjustment value ");
      SerialUSB.println(myIMU.factoryMagCalibration[1], 2);
      SerialUSB.print("Z-Axis factory sensitivity adjustment value ");
      SerialUSB.println(myIMU.factoryMagCalibration[2], 2);
    }

    // Get sensor resolutions, only need to do this once
    myIMU.getAres();
    myIMU.getGres();
    myIMU.getMres();

    // The next call delays for 4 seconds, and then records about 15 seconds of
    // data to calculate bias and scale.
    myIMU.magCalMPU9250(myIMU.magBias, myIMU.magScale);
    SerialUSB.println("AK8963 mag biases (mG)");
    SerialUSB.println(myIMU.magBias[0]);
    SerialUSB.println(myIMU.magBias[1]);
    SerialUSB.println(myIMU.magBias[2]);

    SerialUSB.println("AK8963 mag scale (mG)");
    SerialUSB.println(myIMU.magScale[0]);
    SerialUSB.println(myIMU.magScale[1]);
    SerialUSB.println(myIMU.magScale[2]);
    delay(2000); // Add delay to see results before SerialUSB spew of data

    if(SerialDebug)
    {
      SerialUSB.println("Magnetometer:");
      SerialUSB.print("X-Axis sensitivity adjustment value ");
      SerialUSB.println(myIMU.factoryMagCalibration[0], 2);
      SerialUSB.print("Y-Axis sensitivity adjustment value ");
      SerialUSB.println(myIMU.factoryMagCalibration[1], 2);
      SerialUSB.print("Z-Axis sensitivity adjustment value ");
      SerialUSB.println(myIMU.factoryMagCalibration[2], 2);
    }
  } // if (c == 0x71)
  else
  {
    SerialUSB.print("Could not connect to MPU9250: 0x");
    SerialUSB.println(c, HEX);

    // Communication failed, stop here
    SerialUSB.println(F("Communication failed, abort!"));
    SerialUSB.flush();
    abort();
  }
}

void loop()
{
  // If intPin goes high, all data registers have new data
  // On interrupt, check if data ready interrupt
  if (myIMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
  {
    myIMU.readAccelData(myIMU.accelCount);  // Read the x/y/z adc values

    // Now we'll calculate the accleration value into actual g's
    // This depends on scale being set
    myIMU.ax = (float)myIMU.accelCount[0] * myIMU.aRes; // - myIMU.accelBias[0];
    myIMU.ay = (float)myIMU.accelCount[1] * myIMU.aRes; // - myIMU.accelBias[1];
    myIMU.az = (float)myIMU.accelCount[2] * myIMU.aRes; // - myIMU.accelBias[2];

    myIMU.readGyroData(myIMU.gyroCount);  // Read the x/y/z adc values

    // Calculate the gyro value into actual degrees per second
    // This depends on scale being set
    myIMU.gx = (float)myIMU.gyroCount[0] * myIMU.gRes;
    myIMU.gy = (float)myIMU.gyroCount[1] * myIMU.gRes;
    myIMU.gz = (float)myIMU.gyroCount[2] * myIMU.gRes;

    myIMU.readMagData(myIMU.magCount);  // Read the x/y/z adc values

    // Calculate the magnetometer values in milliGauss
    // Include factory calibration per data sheet and user environmental
    // corrections
    // Get actual magnetometer value, this depends on scale being set
    myIMU.mx = (float)myIMU.magCount[0] * myIMU.mRes
               * myIMU.factoryMagCalibration[0] - myIMU.magBias[0];
    myIMU.my = (float)myIMU.magCount[1] * myIMU.mRes
               * myIMU.factoryMagCalibration[1] - myIMU.magBias[1];
    myIMU.mz = (float)myIMU.magCount[2] * myIMU.mRes
               * myIMU.factoryMagCalibration[2] - myIMU.magBias[2];
  } // if (readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)

  // Must be called before updating quaternions!
  myIMU.updateTime();

  // Sensors x (y)-axis of the accelerometer is aligned with the y (x)-axis of
  // the magnetometer; the magnetometer z-axis (+ down) is opposite to z-axis
  // (+ up) of accelerometer and gyro! We have to make some allowance for this
  // orientationmismatch in feeding the output to the quaternion filter. For the
  // MPU-9250, we have chosen a magnetic rotation that keeps the sensor forward
  // along the x-axis just like in the LSM9DS0 sensor. This rotation can be
  // modified to allow any convenient orientation convention. This is ok by
  // aircraft orientation standards! Pass gyro rate as rad/s
  MahonyQuaternionUpdate(myIMU.ax, myIMU.ay, myIMU.az, myIMU.gx * DEG_TO_RAD,
                         myIMU.gy * DEG_TO_RAD, myIMU.gz * DEG_TO_RAD, myIMU.my,
                         myIMU.mx, myIMU.mz, myIMU.deltat);

  if (!AHRS)
  {
    myIMU.delt_t = millis() - myIMU.count;
    if (myIMU.delt_t > 500)
    {
      if(SerialDebug)
      {
        // Print acceleration values in milligs!
        SerialUSB.print("X-acceleration: "); SerialUSB.print(1000 * myIMU.ax);
        SerialUSB.print(" mg ");
        SerialUSB.print("Y-acceleration: "); SerialUSB.print(1000 * myIMU.ay);
        SerialUSB.print(" mg ");
        SerialUSB.print("Z-acceleration: "); SerialUSB.print(1000 * myIMU.az);
        SerialUSB.println(" mg ");

        // Print gyro values in degree/sec
        SerialUSB.print("X-gyro rate: "); SerialUSB.print(myIMU.gx, 3);
        SerialUSB.print(" degrees/sec ");
        SerialUSB.print("Y-gyro rate: "); SerialUSB.print(myIMU.gy, 3);
        SerialUSB.print(" degrees/sec ");
        SerialUSB.print("Z-gyro rate: "); SerialUSB.print(myIMU.gz, 3);
        SerialUSB.println(" degrees/sec");

        // Print mag values in degree/sec
        SerialUSB.print("X-mag field: "); SerialUSB.print(myIMU.mx);
        SerialUSB.print(" mG ");
        SerialUSB.print("Y-mag field: "); SerialUSB.print(myIMU.my);
        SerialUSB.print(" mG ");
        SerialUSB.print("Z-mag field: "); SerialUSB.print(myIMU.mz);
        SerialUSB.println(" mG");

        myIMU.tempCount = myIMU.readTempData();  // Read the adc values
        // Temperature in degrees Centigrade
        myIMU.temperature = ((float) myIMU.tempCount) / 333.87 + 21.0;
        // Print temperature in degrees Centigrade
        SerialUSB.print("Temperature is ");  SerialUSB.print(myIMU.temperature, 1);
        SerialUSB.println(" degrees C");
      }

      myIMU.count = millis();
      digitalWrite(myLed, !digitalRead(myLed));  // toggle led
    } // if (myIMU.delt_t > 500)
  } // if (!AHRS)
  else
  {
    // SerialUSB print and/or display at 0.5 s rate independent of data rates
    myIMU.delt_t = millis() - myIMU.count;

    // update LCD once per half-second independent of read rate
    if (myIMU.delt_t > 500)
    {
      if(SerialDebug)
      {
        SerialUSB.print("ax = ");  SerialUSB.print((int)1000 * myIMU.ax);
        SerialUSB.print(" ay = "); SerialUSB.print((int)1000 * myIMU.ay);
        SerialUSB.print(" az = "); SerialUSB.print((int)1000 * myIMU.az);
        SerialUSB.println(" mg");

        SerialUSB.print("gx = ");  SerialUSB.print(myIMU.gx, 2);
        SerialUSB.print(" gy = "); SerialUSB.print(myIMU.gy, 2);
        SerialUSB.print(" gz = "); SerialUSB.print(myIMU.gz, 2);
        SerialUSB.println(" deg/s");

        SerialUSB.print("mx = ");  SerialUSB.print((int)myIMU.mx);
        SerialUSB.print(" my = "); SerialUSB.print((int)myIMU.my);
        SerialUSB.print(" mz = "); SerialUSB.print((int)myIMU.mz);
        SerialUSB.println(" mG");

        SerialUSB.print("q0 = ");  SerialUSB.print(*getQ());
        SerialUSB.print(" qx = "); SerialUSB.print(*(getQ() + 1));
        SerialUSB.print(" qy = "); SerialUSB.print(*(getQ() + 2));
        SerialUSB.print(" qz = "); SerialUSB.println(*(getQ() + 3));
      }

// Define output variables from updated quaternion---these are Tait-Bryan
// angles, commonly used in aircraft orientation. In this coordinate system,
// the positive z-axis is down toward Earth. Yaw is the angle between Sensor
// x-axis and Earth magnetic North (or true North if corrected for local
// declination, looking down on the sensor positive yaw is counterclockwise.
// Pitch is angle between sensor x-axis and Earth ground plane, toward the
// Earth is positive, up toward the sky is negative. Roll is angle between
// sensor y-axis and Earth ground plane, y-axis up is positive roll. These
// arise from the definition of the homogeneous rotation matrix constructed
// from quaternions. Tait-Bryan angles as well as Euler angles are
// non-commutative; that is, the get the correct orientation the rotations
// must be applied in the correct order which for this configuration is yaw,
// pitch, and then roll.
// For more see
// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
// which has additional links.
      myIMU.yaw   = atan2(2.0f * (*(getQ()+1) * *(getQ()+2) + *getQ()
                    * *(getQ()+3)), *getQ() * *getQ() + *(getQ()+1)
                    * *(getQ()+1) - *(getQ()+2) * *(getQ()+2) - *(getQ()+3)
                    * *(getQ()+3));
      myIMU.pitch = -asin(2.0f * (*(getQ()+1) * *(getQ()+3) - *getQ()
                    * *(getQ()+2)));
      myIMU.roll  = atan2(2.0f * (*getQ() * *(getQ()+1) + *(getQ()+2)
                    * *(getQ()+3)), *getQ() * *getQ() - *(getQ()+1)
                    * *(getQ()+1) - *(getQ()+2) * *(getQ()+2) + *(getQ()+3)
                    * *(getQ()+3));
      myIMU.pitch *= RAD_TO_DEG;
      myIMU.yaw   *= RAD_TO_DEG;

      // Declination of SparkFun Electronics (40°05'26.6"N 105°11'05.9"W) is
      // 	8° 30' E  ± 0° 21' (or 8.5°) on 2016-07-19
      // - http://www.ngdc.noaa.gov/geomag-web/#declination
      myIMU.yaw  -= 8.5;
      myIMU.roll *= RAD_TO_DEG;

      if(SerialDebug)
      {
        SerialUSB.print("Yaw, Pitch, Roll: ");
        SerialUSB.print(myIMU.yaw, 2);
        SerialUSB.print(", ");
        SerialUSB.print(myIMU.pitch, 2);
        SerialUSB.print(", ");
        SerialUSB.println(myIMU.roll, 2);

        SerialUSB.print("rate = ");
        SerialUSB.print((float)myIMU.sumCount / myIMU.sum, 2);
        SerialUSB.println(" Hz");
      }

      myIMU.count = millis();
      myIMU.sumCount = 0;
      myIMU.sum = 0;
    } // if (myIMU.delt_t > 500)
  } // if (AHRS)
}
