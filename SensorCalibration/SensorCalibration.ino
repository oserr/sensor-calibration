#include <DebugMacros.h>
#include <LSM303CTypes.h>
#include <SparkFunIMU.h>
#include <SparkFunLSM303C.h>

#define RED 6
#define GREEN 7
#define BLUE 8

LSM303C myIMU;

void setup() {
    // Initialize serial connection and wait unti it is ready.
    SerialUSB.begin(9600);
    while (not SerialUSB);

    // Initialize the sensor.
    if (myIMU.begin() != IMU_SUCCESS)
    {
        SerialUSB.println("Failed myIMU setup.");
        while(1);
    }

    // Make sure the device recognizes itself.
    if (myIMU.magWhoAmI() != IMU_SUCCESS)
    {
        SerialUSB.println("Bad who am I value");
        while(1);
    }

    // Turn off all the LEDs.
    pinMode(RED, OUTPUT);
    digitalWrite(RED, LOW);
    pinMode(GREEN, OUTPUT);
    digitalWrite(GREEN, LOW);
    pinMode(BLUE, OUTPUT);
    digitalWrite(BLUE, LOW);

    SerialUSB.println("x,y,z");
}

void loop() {
    auto accelValue = myIMU.readAccelX();
    if (not isnan(accelValue)) {
        // Print x.
        SerialUSB.print(accelValue);
        SerialUSB.print(",");

        // Print y.
        accelValue = myIMU.readAccelY();
        SerialUSB.print(accelValue);
        SerialUSB.print(",");

        // Print z.
        accelValue = myIMU.readAccelZ();
        SerialUSB.println(accelValue);
    }

    // Sleep for 30 milliseconds to allow the sensor to update the values.
    delay(30);
}
