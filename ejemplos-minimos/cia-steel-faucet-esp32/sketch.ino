/*
  Cia Steel Faucet

  Touchless faucet: an ultrasonic sensor detects the user's hands, a blue
  LED shows the detection and a relay opens the water valve for 5 seconds.
  OOP based on Modest IoT (built with the generic kit).

  Created 19 November 2025
  By <Developer Full Name> - Moen, inc.

  https://wokwi.com/projects/<projectID>
*/

#include "CiaSteelFaucetDevice.h"

CiaSteelFaucetDevice device;

void setup() {
    device.setup();
}

void loop() {
    device.loop();
}
