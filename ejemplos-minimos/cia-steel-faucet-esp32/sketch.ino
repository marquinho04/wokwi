/*
  Cia Steel Faucet

  Touchless faucet: an ultrasonic sensor detects the user's hands, a blue
  LED shows the detection and a relay opens the water valve for 5 seconds.
  OOP based on Modest IoT: the Device mediates between sensors (events)
  and actuators (commands).

  The circuit (ESP32 DevKit V1):
  * HC-SR04 TRIG -> GPIO 5, ECHO -> GPIO 18
  * Blue LED -> GPIO 4
  * Relay module IN -> GPIO 23

  Created 19 November 2025
  By <Developer Full Name> - Moen, inc.

  https://wokwi.com/projects/<projectID>
*/

#include "CiaSteelFaucetDevice.h"

// Device instance: its constructor creates the sensors and actuators
CiaSteelFaucetDevice device;

// Called once at power-on
void setup() {
    device.setup();
}

// Called continuously after setup()
void loop() {
    device.loop();
}
