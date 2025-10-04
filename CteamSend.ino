#include <PS4Controller.h>

void setup() {
  Serial.begin(115200);
  PS4.begin("88:13:BF:82:4A:6A");
  Serial.println("Ready.");
}

void loop() {
  if (PS4.isConnected()) {
    int x = PS4.LStickX();
    int y = PS4.LStickY() + 256;
    Serial.print(String(x) + "\n");
    Serial.print(String(y) + "\n");
    
    if (PS4.RStickX() < -30){
      Serial.print("1000\n");
    } else if(PS4.RStickX() > 30){
      Serial.print("1001\n");
    }
    if (PS4.R1()){
      Serial.print("k");
    } 
    if (PS4.L1()){
      Serial.print("o");
    }
    if (PS4.R2()){
      Serial.print("l");
    } 
    if (PS4.L2()){
      Serial.print("p");
    }
    if (PS4.Circle()){
      Serial.print("m");
    }
    if (PS4.Square()){
      Serial.print("q");
    }
    if (PS4.Right()){
      Serial.print("n");
    }
    if (PS4.Left()){
      Serial.print("r");
    }
    delay(20);
  }
}