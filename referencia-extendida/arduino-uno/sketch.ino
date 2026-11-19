/*
  Smart LifeWatching

  First device of the LifeWatching line of Smart LifeMonitor, inc.
  Measures the patient's weight (load cell + HX711) and heart rate (BPM),
  turns on a blue LED when the weight is greater than 80 kg, signals the
  heart rate status with three LEDs (blue = low < 60, green = normal 60-100,
  red = high > 100) and shows weight, BPM and status on an LCD 2004 (I2C).

  Embedded Application built with Object-Oriented Programming on the
  Modest IoT approach (Device as mediator, Sensors emit Events,
  Actuators execute Commands).

  The circuit:
  * Arduino Uno
  * HX711 load cell amplifier: DT -> D3, SCK -> D2
  * Heart rate sensor (simulated with a potentiometer): SIG -> A0
  * Overweight LED (blue) -> D8
  * Heart rate LEDs: low (blue) -> D9, normal (green) -> D10, high (red) -> D11
  * LCD 2004 I2C (0x27): SDA -> A4, SCL -> A5

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
