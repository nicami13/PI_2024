#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define VRx A0  // Pin del eje X del joystick
#define VRy A1  // Pin del eje Y del joystick
#define buttonPin 7  // Pin del botón de pulso del joystick

RF24 radio(10, 9);  // CE, CSN

const byte address[5] = "00001";  // Dirección de comunicación

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);  // Configura el pin del botón como entrada con pull-up
  radio.begin();
  radio.openWritingPipe(address);  // Configura la dirección para el emisor
  radio.setPALevel(RF24_PA_MIN);   // Configura el nivel de potencia
  radio.setDataRate(RF24_250KBPS); // Ajusta la velocidad a 250 kbps
  radio.setChannel(110);           // Cambia el canal (debe coincidir con el receptor)
  radio.stopListening();           // Deja de escuchar (modo emisor)
}

void loop() {
  int joystickX = analogRead(VRx);  // Lee el eje X del joystick
  int joystickY = analogRead(VRy);  // Lee el eje Y del joystick
  int buttonState = digitalRead(buttonPin);  // Lee el estado del botón de pulso

  // Arreglo para enviar los datos: joystick X, joystick Y, botón
  int joystickData[3] = {joystickX, joystickY, buttonState};

  // Envía los valores del joystick y el botón
  bool success = radio.write(&joystickData, sizeof(joystickData));  // Enviar ambos valores

  // Verifica si el envío fue exitoso
  if (success) {
    Serial.println("Datos enviados correctamente.");
    Serial.print("Joystick X: ");
    Serial.print(joystickX);
    Serial.print(", Joystick Y: ");
    Serial.print(joystickY);
    Serial.print(", Botón: ");
    Serial.println(buttonState == LOW ? "Presionado" : "No presionado");
  } else {
    Serial.println("Error en el envío.");
  }

  delay(100);  // Pausa para no saturar la transmisión
}
