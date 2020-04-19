
#include <vector>
#include <limits>

#include <SPI.h>
#include <MFRC522.h>

#include "tricolor_led_control.h"
#include "uuid.h"

#define RST_PIN 0 // Configurable, see typical pin layout above
#define SS_PIN 2  // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

using namespace hardware;

TricolourControl<15, 4, 5> ledControl;

void setup()
{
  Serial.begin(9600);

  ledControl.disable();

  SPI.begin();                       // Init SPI bus
  mfrc522.PCD_Init();                // Init MFRC522
  delay(4);                          // Optional delay. Some board do need more time after init to be ready, see Readme
  mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

uint8_t writeVal = 0;
bool up = true;
Led::Colour colour = Led::Colour::BLUE;

void loop()
{
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  //Show UID on serial monitor
  Serial.println();
  Serial.print(" UID tag :");

  auto tag = Uuid::make_uuid(std::begin(mfrc522.uid.uidByte), std::begin(mfrc522.uid.uidByte) + mfrc522.uid.size);
  Serial.print(toString(tag));

  auto allowedTag = Uuid::make_uuid("79 43 4E 98");

  // String content = "";
  // byte letter;
  // for (byte i = 0; i < mfrc522.uid.size; i++)
  // {
  //   Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
  //   Serial.print(mfrc522.uid.uidByte[i], HEX);
  //   content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
  //   content.concat(String(mfrc522.uid.uidByte[i], HEX));
  // }
  // content.toUpperCase();
  // Serial.println();
  // if (content.substring(1) == "79 43 4E 98") //change UID of the card that you want to give access
  if (tag == allowedTag)
  {
    ledControl.enable(Led::Colour::GREEN);
    delay(1000);
    ledControl.disable();
  }

  else
  {
    ledControl.enable(Led::Colour::RED);
    delay(3000);
    ledControl.disable();
  }
}

#if 0
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 0 // Configurable, see typical pin layout above
#define SS_PIN 2  // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

void setup()
{
	Serial.begin(9600); // Initialize serial communications with the PC
	while (!Serial)
		;							   // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin();					   // Init SPI bus
	mfrc522.PCD_Init();				   // Init MFRC522
	delay(4);						   // Optional delay. Some board do need more time after init to be ready, see Readme
	mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop()
{
	// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
	if (!mfrc522.PICC_IsNewCardPresent())
	{
		return;
	}

	// Select one of the cards
	if (!mfrc522.PICC_ReadCardSerial())
	{
		return;
	}

	// Dump debug info about the card; PICC_HaltA() is automatically called
	mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}
#endif
