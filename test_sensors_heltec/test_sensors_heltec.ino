#include <Wire.h>
#include "config/HELTEC_DOL53_T9602_wifi_config.h"

uint8_t t9206_data[4];

void T9602_read() {
  Wire1.requestFrom(T9602_ADR_, 4);
  int bytes_read;
  for (int i = 0; i < 4; ++i) {
    bytes_read = t9206_data[i] = Wire1.read();
  }
#ifdef DEBUG_MODE_
  Serial.print("T9602 read: ");
  Serial.println(bytes_read);
#endif
}

int analogRead_average(int pin) {
  int no_read = 5;
  //dummy for ADC mux
  int average = analogRead(pin);
  average = 0;
  delay(500);
  for (int i = 0; i < no_read; ++i) {
    average += analogRead(pin);
    delay(100);
  }
#ifdef DEBUG_MODE_
  Serial.print("tot analog value: ");
  Serial.println(average);
#endif
  average = average / no_read;
  average = ADC_LUT[average];
  return average;
}

float get_nh3() {
  int analog = analogRead_average(DOL53_VALUE_PIN_);
  float nh3 = ((analog * 10) / 4095.0) * 10;
#ifdef DEBUG_MODE_
  Serial.print("nh3: ");
  Serial.println(nh3);
#endif
  return nh3;
}

float get_hum() {
  float hum = (float)((((t9206_data[0] & 0x3F ) << 8) + t9206_data[1]) / 16384.0) * 100.0;
#ifdef DEBUG_MODE_
  Serial.print("hum: ");
  Serial.println(hum);
#endif
  return hum;
}

float get_temp() {
  float temp = (float)((unsigned((t9206_data[2] * 64)) + unsigned((t9206_data[3] >> 2 ))) / 16384.0) * 165.0 - 40.0;
#ifdef DEBUG_MODE_
  Serial.print("temp: ");
  Serial.println(temp);
#endif
  return temp;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  #ifdef DEBUG_MODE_
    while (!Serial) {}
  #endif
    pinMode(T9602_PWR_PIN_, OUTPUT);
    Wire1.begin(SDA, SCL);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(T9602_PWR_PIN_, HIGH);
  delay(2000);
  T9602_read();
  get_nh3();
  get_temp();
  get_hum();
}
