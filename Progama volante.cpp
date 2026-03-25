#include <Arduino.h>
#include <ArduinoJson.h>




// 🎮 JOYSTICK 1
#define J1_VRX 34
#define J1_VRY 35
#define J1_SW  32


// 🎮 JOYSTICK 2
#define J2_VRX 36
#define J2_VRY 39
#define J2_SW  33


// 🔄 ENCODER 1
#define ENC1_A 26
#define ENC1_B 25
#define ENC1_SW 13


// 🔄 ENCODER 2
#define ENC2_A 27
#define ENC2_B 14
#define ENC2_SW 2


// 🔘 BOTÕES
int buttonsPins[10] = {
  4, 16, 17, 5, 18, 19, 21, 22, 23, 13
};


// ⚙️ MICRO SWITCHES
#define MSW1 12
#define MSW2 15


volatile int enc1_count = 0;
volatile int enc2_count = 0;




// INTERRUPÇÕES
void IRAM_ATTR enc1_ISR_A() {
  if (digitalRead(ENC1_A) == digitalRead(ENC1_B)) enc1_count++;
  else enc1_count--;
}


void IRAM_ATTR enc1_ISR_B() {
  if (digitalRead(ENC1_A) != digitalRead(ENC1_B)) enc1_count++;
  else enc1_count--;
}


void IRAM_ATTR enc2_ISR_A() {
  if (digitalRead(ENC2_A) == digitalRead(ENC2_B)) enc2_count++;
  else enc2_count--;
}


void IRAM_ATTR enc2_ISR_B() {
  if (digitalRead(ENC2_A) != digitalRead(ENC2_B)) enc2_count++;
  else enc2_count--;
}




void setup() {
  Serial.begin(115200);


  pinMode(J1_SW, INPUT_PULLUP);
  pinMode(J2_SW, INPUT_PULLUP);


  pinMode(ENC1_A, INPUT_PULLUP);
  pinMode(ENC1_B, INPUT_PULLUP);
  pinMode(ENC1_SW, INPUT_PULLUP);


  pinMode(ENC2_A, INPUT_PULLUP);
  pinMode(ENC2_B, INPUT_PULLUP);
  pinMode(ENC2_SW, INPUT_PULLUP);


  attachInterrupt(ENC1_A, enc1_ISR_A, CHANGE);
  attachInterrupt(ENC1_B, enc1_ISR_B, CHANGE);
  attachInterrupt(ENC2_A, enc2_ISR_A, CHANGE);
  attachInterrupt(ENC2_B, enc2_ISR_B, CHANGE);


  for (int i = 0; i < 10; i++) {
    pinMode(buttonsPins[i], INPUT_PULLUP);
  }


  pinMode(MSW1, INPUT_PULLUP);
  pinMode(MSW2, INPUT_PULLUP);
}




void loop() {
  StaticJsonDocument<256> doc;


  doc["t"] = millis();


  // JOYSTICK 1
  JsonObject j1 = doc.createNestedObject("j1");
  j1["x"] = analogRead(J1_VRX);
  j1["y"] = analogRead(J1_VRY);
  j1["sw"] = (int)(!digitalRead(J1_SW));


  // JOYSTICK 2
  JsonObject j2 = doc.createNestedObject("j2");
  j2["x"] = analogRead(J2_VRX);
  j2["y"] = analogRead(J2_VRY);
  j2["sw"] = (int)(!digitalRead(J2_SW));


  // ENCODER 1
  JsonObject e1 = doc.createNestedObject("enc1");
  e1["count"] = enc1_count;
  e1["sw"] = (int)(!digitalRead(ENC1_SW));


  // ENCODER 2
  JsonObject e2 = doc.createNestedObject("enc2");
  e2["count"] = enc2_count;
  e2["sw"] = (int)(!digitalRead(ENC2_SW));


  // BOTÕES
  JsonArray btn = doc.createNestedArray("buttons");
  for (int i = 0; i < 10; i++) {
    btn.add((int)(!digitalRead(buttonsPins[i])));
  }


  // MICRO SWITCHES
  JsonArray msw = doc.createNestedArray("micros");
  msw.add((int)(!digitalRead(MSW1)));
  msw.add((int)(!digitalRead(MSW2)));


  serializeJson(doc, Serial);
  Serial.println();


  delay(10);
}