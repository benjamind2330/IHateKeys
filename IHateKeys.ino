#include "tricolor_led_control.h"
#include "rfid_access.h"
#include "motor_driver.h"
#include "constants.h"

#include <memory>

using namespace hardware;

TricolourControl<15, 4, 5> ledControl;

// MotorDriver<1, 2, 3> motorDriver;
std::unique_ptr<CardAccessManager<constants::SS_RFID, constants::RESET_RFID>>
    cardAccessManager;

void setup()
{
  Serial.begin(9600);

  ledControl.disable();

  CardRegistry cardRegistry;
  cardRegistry.addCard(make_card("79 43 4E 98", "Ben", CardData::Type::CARD));

  auto onApproved = [](CardData card) {
    Serial.println("Allowed Card: \n" + toString(card));
    ledControl.enable(Led::Colour::GREEN);
    delay(1000);
    ledControl.disable();
  };

  auto onDenied = [](CardData card) {
    Serial.print("Denied ID: " + toString(card.id));
    ledControl.enable(Led::Colour::RED);
    delay(3000);
    ledControl.disable();
  };

  cardAccessManager.reset(new CardAccessManager<constants::SS_RFID, constants::RESET_RFID>(std::move(cardRegistry), onApproved, onDenied));
}

void loop()
{
  cardAccessManager->run();
}