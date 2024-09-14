#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 53  // Slave Select pin (SDA)
#define RST_PIN 49  // Reset pin (RST)
#define BUTTON_PIN 7  // Pin connected to the button

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
bool readingActive = false;  // Flag to indicate whether the card reading is active
bool writingMode = false;    // Flag to indicate whether the system is in writing mode
byte keyCardData[64];  // Array to hold the data read from the card (16 pages * 4 bytes = 64 bytes)

void setup() {
  Serial.begin(9600);  // Initialize serial communications
  SPI.begin();  // Initialize SPI bus
  mfrc522.PCD_Init();  // Initialize MFRC522

  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Set button pin as input with internal pull-up

  Serial.println("Press the button to start looking for cards...");
}

void loop() {
  // Check if the button is pressed to toggle reading/writing mode
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(200);  // Debounce delay
    if (!readingActive && !writingMode) {  // Button press starts reading mode
      readingActive = true;  // Start reading mode
      Serial.println("Card reading activated. Bring your card close to the reader...");
    } else if (readingActive) {  // Button press after reading starts writing mode
      readingActive = false;  // Stop reading mode
      writingMode = true;     // Enable writing mode
      Serial.println("Reading complete. Now looking for a tag to write to...");
    }
    delay(500);  // Additional debounce delay
  }

  // If reading is active, continuously look for cards to read
  if (readingActive) {
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      Serial.println("MIFARE Ultralight EV1 card detected.");
      
      // Read and store the first 16 pages
      if (readUltralightPages()) {
        readingActive = false;  // Stop reading once the card is read
        writingMode = true;     // Enable writing mode after reading a card
        Serial.println("Card data read successfully. Press the button again to write to another tag.");
      } else {
        Serial.println("Error reading card data.");
      }

      // Halt the card and stop communication
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
    }
  }

  // If writing mode is active, look for a new card to write the data to
  if (writingMode) {
    if (mfrc522.PICC_IsNewCardPresent()) {
      Serial.println("Tag detected, attempting to read serial...");
      if (mfrc522.PICC_ReadCardSerial()) {
        Serial.println("MIFARE Ultralight EV1 tag detected. Writing data...");
        
        // Write the stored data to the new tag
        if (writeUltralightPages()) {
          Serial.println("Data written successfully.");
          writingMode = false;  // Disable writing mode after the write operation
          Serial.println("Process complete. Press the button to start again.");
        } else {
          Serial.println("Error writing to the tag.");
        }
      } else {
        Serial.println("Error reading serial from tag.");
      }

      // Halt the card and stop communication
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
    }
  }
}

// Function to read the first 16 pages (4 bytes per page) of MIFARE Ultralight EV1
bool readUltralightPages() {
  byte buffer[18];  // Buffer to hold page data
  byte size = sizeof(buffer);

  for (byte page = 0; page < 16; page++) {
    MFRC522::StatusCode status = mfrc522.MIFARE_Read(page, buffer, &size);
    if (status == MFRC522::STATUS_OK) {
      Serial.print("Page ");
      Serial.print(page);
      Serial.print(": ");
      for (byte i = 0; i < 4; i++) {  // Each page contains 4 bytes
        keyCardData[page * 4 + i] = buffer[i];  // Store the data for writing later
        Serial.print(buffer[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
    } else {
      Serial.print("Reading failed at page ");
      Serial.print(page);
      Serial.print(": ");
      Serial.println(mfrc522.GetStatusCodeName(status));
      return false;  // Return false if any page fails to read
    }
  }
  return true;  // Return true if all pages are read successfully
}

// Function to write the stored data to the new MIFARE Ultralight tag
bool writeUltralightPages() {
  MFRC522::StatusCode status;

  for (byte page = 0; page < 16; page++) {
    Serial.print("Writing to page ");
    Serial.print(page);
    Serial.print("...");
    status = mfrc522.MIFARE_Ultralight_Write(page, &keyCardData[page * 4], 4);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(" failed: ");
      Serial.println(mfrc522.GetStatusCodeName(status));
      return false;  // Return false if the write fails
    } else {
      Serial.println(" success.");
    }
  }
  return true;  // Return true if all pages are written successfully
}
