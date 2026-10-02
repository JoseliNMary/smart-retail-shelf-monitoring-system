#include "HX711.h"
#include <SPI.h>
#include <MFRC522.h>

// ==========================================
// HX711
// ==========================================
#define HX711_DT 34
#define HX711_SCK 32

HX711 scale;

// ==========================================
// RFID MFRC522
// ==========================================
#define RFID_SS 5
#define RFID_RST 22

#define RFID_SCK 18
#define RFID_MISO 19
#define RFID_MOSI 23

MFRC522 rfid(RFID_SS, RFID_RST);

// ==========================================
// BUZZER
// ==========================================
#define BUZZER 4

// ==========================================
// BUILT-IN LED
// ==========================================
#define LED_PIN 2

// ==========================================
// VARIABLES
// ==========================================
long previousReading = 0;

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("======================================");
  Serial.println("       SMART RETAIL SHELF TEST");
  Serial.println("======================================");

  // ======================================
  // LED
  // ======================================

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // ======================================
  // BUZZER
  // ======================================

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  // ======================================
  // HX711
  // ======================================

  scale.begin(HX711_DT, HX711_SCK);

  Serial.println("HX711 initialized");

  if (scale.is_ready()) {

    Serial.println("HX711 is READY!");

  } else {

    Serial.println("ERROR: HX711 is NOT detected!");

  }

  Serial.println();
  Serial.println("Remove all weight from the load cell.");
  Serial.println("Taring...");

  delay(2000);

  scale.tare();

  Serial.println("Tare complete!");
  Serial.println("Place weight on the load cell.");
  Serial.println();

  if (scale.is_ready()) {
    previousReading = scale.read();
  }

  // ======================================
  // RFID
  // ======================================

  Serial.println("Starting RFID reader...");

  SPI.begin(
    RFID_SCK,
    RFID_MISO,
    RFID_MOSI,
    RFID_SS
  );

  rfid.PCD_Init();

  delay(100);

  byte version = rfid.PCD_ReadRegister(MFRC522::VersionReg);

  Serial.print("RFID Reader Version: 0x");
  Serial.println(version, HEX);

  if (version == 0x00 || version == 0xFF) {

    Serial.println("WARNING: RFID reader communication problem!");

  } else {

    Serial.println("RFID reader detected!");

  }

  Serial.println();
  Serial.println("======================================");
  Serial.println("SYSTEM READY");
  Serial.println("======================================");
  Serial.println("RFID: Scan a card/tag");
  Serial.println("Load Cell: Place/remove weight");
  Serial.println();
}


// ==========================================
// RFID FUNCTION
// ==========================================

void checkRFID() {

  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println();
  Serial.println("******** RFID CARD DETECTED ********");

  Serial.print("Card UID: ");

  for (byte i = 0; i < rfid.uid.size; i++) {

    if (rfid.uid.uidByte[i] < 0x10) {
      Serial.print("0");
    }

    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }

  Serial.println();

  Serial.print("UID Size: ");
  Serial.print(rfid.uid.size);
  Serial.println(" bytes");

  // ======================================
  // BUZZER
  // ======================================

  digitalWrite(BUZZER, HIGH);
  delay(200);

  digitalWrite(BUZZER, LOW);
  delay(100);

  digitalWrite(BUZZER, HIGH);
  delay(200);

  digitalWrite(BUZZER, LOW);

  // ======================================
  // LED BLINK
  // ======================================

  digitalWrite(LED_PIN, HIGH);
  delay(200);

  digitalWrite(LED_PIN, LOW);

  Serial.println("RFID accepted - Buzzer + LED activated");
  Serial.println("************************************");
  Serial.println();

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

}


// ==========================================
// WEIGHT FUNCTION
// ==========================================

void checkWeight() {

  if (!scale.is_ready()) {
    return;
  }

  long reading = scale.read();

  long difference = reading - previousReading;

  // Detect significant weight change
  if (abs(difference) > 5000) {

    Serial.println();
    Serial.println("******** WEIGHT CHANGE ********");

    Serial.println("Weight change detected!");

    // ====================================
    // LED BLINK
    // ====================================

    digitalWrite(LED_PIN, HIGH);
    delay(150);

    digitalWrite(LED_PIN, LOW);
    delay(150);

    digitalWrite(LED_PIN, HIGH);
    delay(150);

    digitalWrite(LED_PIN, LOW);

    Serial.println("LED blinked");
    Serial.println("*******************************");
    Serial.println();

    previousReading = reading;
  }
}


// ==========================================
// MAIN LOOP
// ==========================================

void loop() {

  // Check RFID
  checkRFID();

  // Check weight
  checkWeight();

  delay(50);
}
