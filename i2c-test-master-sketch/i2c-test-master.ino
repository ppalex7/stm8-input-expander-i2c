#include <Wire.h>

#define STM_REQ_PIN 2
#define STM_I2C_ADDR 0x34

volatile bool iFlag;
volatile bool iValue;
volatile bool autoReq;

void interruptHandler() {
  iFlag = true;
  iValue = digitalRead(2);
}

void setup() {
  while (!Serial)
    ;
  Serial.begin(9600);
  Serial.println(F("serial initialized"));
  Wire.begin();
  Serial.println(F("wire initialized"));

  pinMode(STM_REQ_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(STM_REQ_PIN), interruptHandler, CHANGE);
  Serial.println(F("interrupt attached"));
}

void loop() {
  if (Serial.available() > 0) {
    int input = Serial.read();
    switch (input) {
      // just for UART check
      case 'n':
        Serial.println(F("enable LED"));
        digitalWrite(LED_BUILTIN, HIGH);
        break;
      case 'f':
        Serial.println(F("disable LED"));
        digitalWrite(LED_BUILTIN, LOW);
        break;
      case 'i':
        i2c_req(STM_I2C_ADDR);
        break;
      case 'I':
        i2c_req(STM_I2C_ADDR << 1);
        break;
      case 's':
        i2c_scan();
        break;
      case 'S':
        i2c_scan2();
        break;
      case 'a':
        autoReq = true;
        break;
      case 'A':
        autoReq = false;
        break;
    }
  }
  if (iFlag) {
    Serial.print(F("external interrupt :"));
    Serial.println(iValue);
    if (autoReq) {
      while (digitalRead(STM_REQ_PIN)) {
        i2c_req(STM_I2C_ADDR);
      }
    }
    iFlag = false;
  }

  delayMicroseconds(1000);
}


void i2c_req(uint8_t addr) {
  Serial.print(F("request i2c "));
  Serial.print(addr, HEX);
  Serial.print(": ");
  Wire.requestFrom(addr, 2u);
  bool printHeader = true;
  while (Wire.available()) {
    int b = Wire.read();
    if (printHeader) {
      Serial.print(F("response i2c "));
      printHeader = false;
    }
    for (int i = 7; i >= 0; i--) {
      int mask = 0b1u << i;
      bool digit = (bool)(b & mask);
      Serial.print(digit, BIN);
      if (i % 4 == 0) {
        Serial.print(' ');
      }
    }
    Serial.print(' ');
  }
  Serial.println(F("; finish i2c"));
}

void i2c_scan2() {
  for (uint8_t addr = 0; addr <= 254; addr++) {
    i2c_req(addr);
  }
}

void i2c_scan() {
  int count = 0;
  for (uint8_t i = 0; i < 254; i++) {
    Serial.print(F("begin transmission "));
    Serial.println(i, HEX);
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0) {
      Serial.print("Found address: ");
      Serial.print(i, DEC);
      Serial.print(" (0x");
      Serial.print(i, HEX);
      Serial.println(")");
      count++;
      delay(1);  // maybe unneeded?
    }            // end of good response
  }              // end of for loop
  Serial.println("Done.");
  Serial.print("Found ");
  Serial.print(count, DEC);
  Serial.println(F(" device(s)."));
}
