#include <PS4Controller.h>

void setup() {
  Serial.begin(115200);
  PS4.begin(88:13:BF:82:4A:6A);
  Serial.println("Ready.");
}

void loop() {
  // Below has all accessible outputs from the controller
  if (PS4.isConnected()) {
    Serial.write(PS4.LStickX());
    Serial.write(",");
    Serial.write(PS4.LStickY());
    Serial.write("n");
  }
}
