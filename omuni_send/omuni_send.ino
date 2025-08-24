#include <PS4Controller.h>

void setup() {
  Serial.begin(115200);
  PS4.begin("88:13:BF:82:4A:6A");
  Serial.println("Ready.");
}

void loop() {
  if (PS4.isConnected()) {
    Serial.print(String(PS4.LStickX()) + "," + String(PS4.LStickY()) + "\n");
    delay(100);
  }
}
