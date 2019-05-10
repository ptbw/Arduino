# GY-63-MS5611-01BA03-Arduino
```C++
#include <GY63.h>

GY63 gy63;

void setup() {
  gy63.init(A14);  //CSB接口位置
  Serial.begin(9600);
}

void loop() {
  Serial.print(gy63.getTemperature(MS561101BA_D2_OSR_4096)); //必须先读取温度才能读取气压
  Serial.print(" C\t");
  Serial.print(gy63.getPressure(MS561101BA_D1_OSR_4096));
  Serial.println(" hPa");
}
```
