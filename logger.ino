#include "SPI.h"
#include "SD.h"
#include "HX711.h"

const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 2;
const int SD_PIN = 10;
const int LED_PIN = 8;
const int SD_OFFLOAD_INTERVAL = 10000; // ms
unsigned long last_toggle = 0;
unsigned long last_sync = 0;
bool led_open = false;
String line = "";
File data_file;

HX711 scale;

void status_led_off() {
  led_open = false;
  digitalWrite(LED_PIN, LOW);
}

void status_led_on() {
  led_open = true;
  digitalWrite(LED_PIN, HIGH);
}

void status_led_toggle(int interval = 1000) {
  if (millis() - last_toggle > interval) {
    last_toggle = millis();
    if (led_open) {
      status_led_off();
    } else {
      status_led_on();
    }
  }
}

void setup() {
  Serial.begin(38400);
  pinMode(LED_PIN, OUTPUT);
  status_led_on();

  if (!SD.begin(SD_PIN)) {
    while (1) {
      status_led_toggle(200);
    }
  }
  data_file = SD.open("load.log", FILE_WRITE);
  data_file.println("### NEW MEASUREMENT ###");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale();
  scale.tare(); // reset scale to 0

  // TODO: Calibrate
  // Serial.println(scale.get_units(5), 1);
  scale.set_scale(2280.f);
}

void loop() {
  line = String(millis()) + ',';
  if (scale.wait_ready_retry(10)) {
    line += String(scale.get_units());
    status_led_toggle();
  } else {
    line += "--";
    status_led_off();
  }

  data_file.println(line);
  if (millis() - last_sync > SD_OFFLOAD_INTERVAL) {
    last_sync = millis();
    data_file.close();
    data_file = SD.open("load.log", FILE_WRITE);
  }

  delay(12); // ~80Hz, matching the amplifier's rate
}
