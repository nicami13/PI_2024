#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

// Pines para los motores del puente H
const int motorAForward = 4;
const int motorABackward = 5;
const int motorBForward = 6;
const int motorBBackward = 7;

Servo myServo;  // Crea un objeto Servo
RF24 radio(10, 9);  // CE, CSN

const byte address[5] = "00001";  // Dirección de comunicación

void setup() {
  Serial.begin(9600);
  myServo.attach(3);  // El servo está conectado al pin D3

  // Configuración de los pines del puente H
  pinMode(motorAForward, OUTPUT);
  pinMode(motorABackward, OUTPUT);
  pinMode(motorBForward, OUTPUT);
  pinMode(motorBBackward, OUTPUT);

  radio.begin();
  radio.openReadingPipe(0, address);  // Configura la dirección para el receptor
  radio.setPALevel(RF24_PA_MIN);      // Configura el nivel de potencia
  radio.setDataRate(RF24_250KBPS);    // Ajusta la velocidad a 250 kbps
  radio.setChannel(110);              // Cambia el canal (debe coincidir con el transmisor)
  radio.startListening();             // Modo receptor

  // Inicializar motores en estado detenido
  motorStop();
  Serial.println("Inicialización completa. Esperando datos...");
}

void loop() {
  if (radio.available()) {
    int joystickData[3];  // Arreglo para almacenar X, Y y botón
    radio.read(&joystickData, sizeof(joystickData));  // Lee los valores de joystick X, Y y el botón

    int joystickX = joystickData[0];
    int joystickY = joystickData[1];
    int buttonState = joystickData[2];

    // Mostrar datos recibidos para diagnóstico
    Serial.print("Joystick X: "); Serial.print(joystickX);
    Serial.print(" | Joystick Y: "); Serial.print(joystickY);
    Serial.print(" | Estado del Botón: "); Serial.println(buttonState);

    // CONTROL DEL SERVO SOLO CUANDO EL BOTÓN ESTÁ PRESIONADO
    if (buttonState == LOW) {  // Si el botón está presionado
      myServo.write(60);  // Mueve el servo a 60 grados
      Serial.println("Botón presionado, moviendo el servo a 60 grados.");
    } else {
      myServo.write(90); // Posición neutra del servo si no se presiona el botón
    }

    // CONTROL DE MOTORES (Movimiento del carro) basado en el eje Y (adelante/atrás)
    if (joystickY < 400) {  // Mover adelante (si joystick está hacia arriba)
      motorForward();
      Serial.println("Moviendo hacia adelante");
    } else if (joystickY > 600) {  // Mover hacia atrás (si joystick está hacia abajo)
      motorBackward();
      Serial.println("Moviendo hacia atrás");
    } else {
      motorStop();  // Detener los motores cuando el joystick está en el centro
      Serial.println("Parado");
    }

    // CONTROL DE DIRECCIÓN basado en el eje X (izquierda/derecha)
    if (joystickX < 400) {  // Girar a la izquierda
      motorTurnLeft();
      Serial.println("Girando a la izquierda");
    } else if (joystickX > 600) {  // Girar a la derecha
      motorTurnRight();
      Serial.println("Girando a la derecha");
    }

  } else {
    Serial.println("Esperando datos...");
  }

  delay(100);  // Retraso para reducir la carga de procesamiento
}

// Funciones para controlar los motores
void motorForward() {
  digitalWrite(motorAForward, HIGH);
  digitalWrite(motorABackward, LOW);
  digitalWrite(motorBForward, HIGH);
  digitalWrite(motorBBackward, LOW);
}

void motorBackward() {
  digitalWrite(motorAForward, LOW);
  digitalWrite(motorABackward, HIGH);
  digitalWrite(motorBForward, LOW);
  digitalWrite(motorBBackward, HIGH);
}

void motorTurnLeft() {
  digitalWrite(motorAForward, LOW);   // Detén motor A
  digitalWrite(motorABackward, LOW);
  digitalWrite(motorBForward, HIGH);  // Motor B hacia adelante
  digitalWrite(motorBBackward, LOW);
}

void motorTurnRight() {
  digitalWrite(motorAForward, HIGH);  // Motor A hacia adelante
  digitalWrite(motorABackward, LOW);
  digitalWrite(motorBForward, LOW);   // Detén motor B
  digitalWrite(motorBBackward, LOW);
}

void motorStop() {
  digitalWrite(motorAForward, LOW);
  digitalWrite(motorABackward, LOW);
  digitalWrite(motorBForward, LOW);
  digitalWrite(motorBBackward, LOW);
}
