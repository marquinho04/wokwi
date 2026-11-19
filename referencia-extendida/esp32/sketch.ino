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
  * ESP32 DevKit V1
  * HX711 load cell amplifier: DT -> GPIO 19, SCK -> GPIO 18
  * Heart rate sensor (simulated with a potentiometer): SIG -> GPIO 34
  * Overweight LED (blue) -> GPIO 4
  * Heart rate LEDs: low (blue) -> GPIO 25, normal (green) -> GPIO 26, high (red) -> GPIO 27
  * LCD 2004 I2C (0x27): SDA -> GPIO 21, SCL -> GPIO 22

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
