/*
  Smart LifeWatching

  Measures the patient's weight (HX711) and heart rate (BPM), signals
  weight > 80 kg and the heart rate status with LEDs, and shows weight,
  BPM and status on an LCD 2004. OOP based on Modest IoT: the Device
  mediates between sensors (events) and actuators (commands).

  The circuit (Arduino Uno):
  * HX711 DT -> D3, SCK -> D2
  * Heart rate sensor (potentiometer) SIG -> A0
  * Overweight LED -> D8; low / normal / high LEDs -> D9 / D10 / D11
  * LCD 2004 I2C (0x27): SDA -> A4, SCL -> A5

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.

  https://wokwi.com/projects/<projectID>
*/

#include "SmartLifeWatchingDevice.h"

// Device instance: its constructor creates the sensors and actuators
SmartLifeWatchingDevice device;

// Called once at power-on
void setup() {
    device.setup();
}

// Called continuously after setup()
void loop() {
    device.loop();
}
