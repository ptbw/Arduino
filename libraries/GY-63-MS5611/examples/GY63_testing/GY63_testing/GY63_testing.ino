#include <GY63.h>

GY63 gy63;

void setup() {
  gy63.init(A14);
  Serial.begin(9600);
}

void loop() {
  Serial.print(gy63.getTemperature(MS561101BA_D2_OSR_4096));
  Serial.print(" C\t");
  Serial.print(gy63.getPressure(MS561101BA_D1_OSR_4096));
  Serial.println(" hPa");
}
