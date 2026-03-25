// --- Protótipo ---
void readEncoder();

// --- Definições Encoder ---
const int ENCODER_PIN_A = 3;
const int ENCODER_PIN_B = 4;
volatile long encoderPosition = 0;

// --- Definições Driver ---
const int RPWM_PIN = 11;
const int LPWM_PIN = 10;
const int R_EN_PIN = 7;
const int L_EN_PIN = 8;

// --- Controle ---
const long targetPosition = 0;  // posição alvo
const long tolerance = 10;       // margem de erro ±50 pulsos

const float Kp = 20.0;            // ganho proporcional

void setup() {
  Serial.begin(115200);

  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);

  pinMode(RPWM_PIN, OUTPUT);
  pinMode(LPWM_PIN, OUTPUT);
  pinMode(R_EN_PIN, OUTPUT);
  pinMode(L_EN_PIN, OUTPUT);

  digitalWrite(R_EN_PIN, HIGH);
  digitalWrite(L_EN_PIN, HIGH);

  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), readEncoder, CHANGE);
}

void loop() {
  long error = targetPosition - encoderPosition;

  Serial.print("Posição atual: ");
  Serial.print(encoderPosition);
  Serial.print(" | Erro: ");
  Serial.println(error);

  // Para motor se dentro da margem de erro ±50
  if (abs(error) <= tolerance) {
    controlMotor(0, 0);
  } else {
    int speed = (int)(Kp * abs(error));
    speed = constrain(speed, 0, 255);

    int direction = (error > 0) ? 1 : -1;

    controlMotor(speed, direction);
  }

  delay(30);
}

void readEncoder() {
  int A_state = digitalRead(ENCODER_PIN_A);
  int B_state = digitalRead(ENCODER_PIN_B);

  if (A_state == HIGH) {
    if (B_state == LOW) encoderPosition++;
    else encoderPosition--;
  } else {
    if (B_state == HIGH) encoderPosition++;
    else encoderPosition--;
  }
}

void controlMotor(int speed, int direction) {
  speed = constrain(speed, 0, 255);

  if (direction == 1) {
    analogWrite(RPWM_PIN, speed);
    analogWrite(LPWM_PIN, 0);
  } else if (direction == -1) {
    analogWrite(RPWM_PIN, 0);
    analogWrite(LPWM_PIN, speed);
  } else {
    analogWrite(RPWM_PIN, 0);
    analogWrite(LPWM_PIN, 0);
  }
}