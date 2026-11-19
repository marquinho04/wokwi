/*
  Smart LifeWatching

  Measures the patient's weight (HX711) and heart rate (BPM), signals
  weight > 80 kg and the heart rate status with LEDs, and shows weight,
  BPM and status on an LCD 2004. OOP based on Modest IoT.

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.

  https://wokwi.com/projects/<projectID>
*/

#include "SmartLifeWatchingDevice.h"

SmartLifeWatchingDevice device;

void setup() {
    device.setup();
}

void loop() {
    device.loop();
}
