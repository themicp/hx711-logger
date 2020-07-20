#include "HX711.h"

const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 2;

HX711 scale;

void setup() {
  Serial.begin(38400);
  Serial.println("Initializing the scale");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale();
  scale.tare(); // reset scale to 0

  // TODO: Calibrate
  Serial.println(scale.get_units(5), 1);
  scale.set_scale(2280.f);
}

void loop() {
  if (scale.wait_ready_retry(10)) {
    Serial.println(scale.get_units(), 1);
  } else {
    Serial.println("--");
  }

  delay(12); // ~80Hz, matching the amplifier's rate
}
