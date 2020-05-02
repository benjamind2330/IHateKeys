#include "tricolor_led_control.h"
#include "rfid_access.h"
#include "motor_driver.h"
#include "constants.h"
#include "pin_control.h"
#include <memory>

using namespace hardware;

TricolourControl<constants::RED_PIN, constants::GREEN_PIN, constants::BLUE_PIN> ledControl;
MotorDriver<constants::MOTOR_ENABLE, constants::MOTOR_R1, constants::MOTOR_R2> motorDriver;

std::unique_ptr<CardAccessManager<constants::SS_RFID, constants::RESET_RFID>>
    cardAccessManager;

void setup()
{
  Serial.begin(9600);
  while (!Serial)
  {
  }

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
    Serial.println("Denied ID: " + toString(card.id));
    ledControl.enable(Led::Colour::RED);
    delay(3000);
    ledControl.disable();
  };

  cardAccessManager.reset(new CardAccessManager<constants::SS_RFID, constants::RESET_RFID>(std::move(cardRegistry), onApproved, onDenied));
}

bool forward = true;

void loop()
{
  //  cardAccessManager->run();

  // if (forward)
  // {
  //   motorDriver.drive(Forward{}, 50);
  //   forward = false;
  // }
  // else
  // {
  //   motorDriver.drive(Reverse{}, 100);
  //   forward = true;
  // }

  // delay(1000);
}