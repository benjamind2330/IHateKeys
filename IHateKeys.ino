#include "tricolor_led_control.h"
#include "rfid_access.h"
#include "motor_driver.h"
#include "constants.h"

#include <memory>

using namespace hardware;

TricolourControl<constants::RED_PIN, constants::GREEN_PIN, constants::BLUE_PIN> ledControl;
//MotorDriver<constants::MOTOR_ENABLE, constants::MOTOR_R1, constants::MOTOR_R2> motorDriver;

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

  //motorDriver.drive(hardware::Forward{}, 50);

  PinControl<constants::MOTOR_ENABLE, Pin::WriteMode> enable{};
  PinControl<constants::MOTOR_R1, Pin::WriteMode> r1;
  PinControl<constants::MOTOR_R2, Pin::WriteMode> r2;

  enable.high();
  r1.high();
  r2.low();
}

void loop()
{

  //cardAccessManager->run();
}