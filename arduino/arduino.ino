#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);

Servo doorServo;
const int servoPin = 6; // Servo control pin

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  doorServo.attach(servoPin);
  doorServo.write(0); // Door initially closed
  Serial.println("RFID Reader Initialized. Waiting for card...");
}

void loop() {
  // FIX: Add proper logical AND operator && and parentheses
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return;

  String uidString = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uidString += "0";
    uidString += String(rfid.uid.uidByte[i], HEX);
  }
  uidString.toUpperCase();

  Serial.println(uidString);

  // FIX: Use proper logical OR operator 
  if (uidString == "5525B902" ) {
    Serial.println("Access Granted - Opening Door");
    doorServo.write(90); // Open door
    delay(3000);         // Wait 3 seconds
    doorServo.write(0);  // Close door
  } else {
    Serial.println("Access Denied");
  }

  rfid.PICC_HaltA();
}
