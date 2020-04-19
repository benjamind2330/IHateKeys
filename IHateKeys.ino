
#include <vector>
#include <limits>

#include "tricolor_led_control.h"
#include "rfid_access.h"

#define RST_PIN 0 // Configurable, see typical pin layout above
#define SS_PIN 2  // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

using namespace hardware;

TricolourControl<15, 4, 5> ledControl;
CardRegistry cardRegistry;

void setup()
{
  Serial.begin(9600);

  ledControl.disable();

  SPI.begin();                       // Init SPI bus
  mfrc522.PCD_Init();                // Init MFRC522
  delay(4);                          // Optional delay. Some board do need more time after init to be ready, see Readme
  mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

  cardRegistry.addCard(make_card("79 43 4E 98", "Ben", CardData::Type::CARD));
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
  auto tag = Uuid::make_uuid(std::begin(mfrc522.uid.uidByte), std::begin(mfrc522.uid.uidByte) + mfrc522.uid.size);
  auto card = cardRegistry.card(tag);

  if (card)
  {
    Serial.println("Allowed Card: \n" + toString(*card));
    ledControl.enable(Led::Colour::GREEN);
    delay(1000);
    ledControl.disable();
  }

  else
  {
    Serial.print("Denied ID: " + toString(tag));
    ledControl.enable(Led::Colour::RED);
    delay(3000);
    ledControl.disable();
  }
}