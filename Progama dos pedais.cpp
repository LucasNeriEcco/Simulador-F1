#include "HX711.h"
#include "HID-Project.h"
#include "EEPROM.h"

// Valores configuráveis pelo usuário

const int deadBand = 2; // Ajuste para um valor maior se os inputs estiverem sendo ativados sem interação do usuário.
int clutchPin = A3; // Pino analógico onde a embreagem está conectada
int throttlePin = A2; // Pino analógico onde o acelerador está conectado
uint8_t dataPin = 2; // Pino de dados do amplificador da célula de carga
uint8_t clockPin = 3; // Pino de clock do amplificador da célula de carga

// Não modifique nenhum valor abaixo desta linha.

int XYRangeMax = 32767;
int XYRangeMin = -32768;
int ZRangeMax = 127;
int ZRangeMin = -128;

bool enableAutoCalibration = false;
unsigned long calibrationTimer;

// Valores de calibração
int clutchMin = 10000;
int clutchMax = 0;

int throttleMin = 10000;
int throttleMax = 0;

int brakeMin = 10000;
int brakeMax = 0;

// Armazenamento dos valores brutos
int clutchValue;
int throttleValue;
int brakeValue;

// Configuração da célula de carga
float loadCellScale = 420.0983;
HX711 scale;

void setup() {
  Serial.begin(115200);
  delay(500);

  readCalibrationData(1);

  scale.begin(dataPin, clockPin);
  scale.set_scale(loadCellScale);
  scale.tare();

  Serial.println("Deseja calibrar? y/n");
  Gamepad.begin();

}

void loop() {
  if(Serial){
    if (Serial.available() > 0) {
      char c = Serial.read();
      if (c == '\n' || c == '\r') return;
      if (c == 'y' || c == 'Y') {
        Serial.println("Iniciando calibração automática...");
        Serial.println("Mova todos os pedais até seus limites máximos várias vezes. A calibração será concluída automaticamente em 1 minuto.");
        enableAutoCalibration = true;
        calibrationTimer = millis();

        // Resetar valores de calibração
        clutchMin = 10000;
        clutchMax = 0;

        throttleMin = 10000;
        throttleMax = 0;

        brakeMin = 10000;
        brakeMax = 0;
      }
    }
  }

  readInput();

  if(enableAutoCalibration){
    autoCalibration();
    if(calibrationTimer + 60000 < millis()){ // Se 1 minuto passar sem ajustes de calibração, desativa a calibração e salva os valores.
      enableAutoCalibration = false;
      writeCalibrationData(1);
      Serial.println("Calibração concluída!");
    }
  } else {
    Gamepad.xAxis(outputScale(clutchValue, clutchMin, clutchMax, XYRangeMin, XYRangeMax));
    Gamepad.yAxis(outputScale(throttleValue, throttleMin, throttleMax, XYRangeMin, XYRangeMax));
    Gamepad.zAxis(outputScale(brakeValue, brakeMin, brakeMax, ZRangeMin, ZRangeMax));

    Gamepad.write();
  }
}

void autoCalibration(){
  if(clutchValue > clutchMax){
    clutchMax = clutchValue;
  }
  if(clutchValue < clutchMin && clutchValue > 0){
    clutchMin = clutchValue;
  }

  if(throttleValue > throttleMax){
    throttleMax = throttleValue;
  }
  if(throttleValue < throttleMin && throttleValue > 0){
    throttleMin = throttleValue;
  }

  if(brakeValue > brakeMax){
    brakeMax = brakeValue;
  }
  if(brakeValue < brakeMin && brakeValue > 0){
    brakeMin = brakeValue;
  }
}

void readInput(){
  clutchValue = analogRead(A3);
  throttleValue = analogRead(A2);
  brakeValue = scale.get_units(5);
}

int outputScale(int value, int min, int max, int rangeMin, int rangeMax){ 
  if(value > max){
    value = max;
  }
  if(value < min && value > 0){
    value = min;
  }
  if(value < min + deadBand){
    return rangeMin;
  }
  if(value > max - deadBand){
    return rangeMax;
  }

  return map(value, min, max, rangeMin, rangeMax);

}

void writeIntIntoEEPROM(int address, int number){ // Divide o int "number" em BYTES e grava na EEPROM no endereço informado
  byte byte1 = number >> 8;
  byte byte2 = number & 0xFF;
  EEPROM.write(address, byte1);
  EEPROM.write(address + 1, byte2);
}

int readIntFromEEPROM(int address){ // Retorna o valor int da EEPROM no endereço +1
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address + 1);
  return (byte1 << 8) + byte2;
}

void readCalibrationData(int startAdr){ // Lê os dados de calibração da EEPROM
  clutchMin = readIntFromEEPROM(1 + startAdr);
  clutchMax = readIntFromEEPROM(3 + startAdr);
  throttleMin = readIntFromEEPROM(5 + startAdr);
  throttleMax = readIntFromEEPROM(7 + startAdr);
  brakeMin = readIntFromEEPROM(9 + startAdr);
  brakeMax = readIntFromEEPROM(11 + startAdr);
}

void writeCalibrationData(int startAdr){ // Salva os dados de calibração na EEPROM
  writeIntIntoEEPROM(1 + startAdr, clutchMin);
  writeIntIntoEEPROM(3 + startAdr, clutchMax);
  writeIntIntoEEPROM(5 + startAdr, throttleMin);
  writeIntIntoEEPROM(7 + startAdr, throttleMax);
  writeIntIntoEEPROM(9 + startAdr, brakeMin);
  writeIntIntoEEPROM(11 + startAdr, brakeMax);
}
