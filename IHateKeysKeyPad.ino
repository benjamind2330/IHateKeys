#include "tricolor_led_control.h"
#include "rfid_access.h"
#include "motor_driver.h"
#include "constants.h"
#include "pin_control.h"
#include "keypad.h"
#include <memory>

using namespace hardware;
using namespace units::literals;

// TricolourControl<constants::RED_PIN, constants::GREEN_PIN, constants::BLUE_PIN> ledControl;
MotorDriver<constants::MOTOR_ENABLE, constants::MOTOR_R1, constants::MOTOR_R2> motorDriver;
std::unique_ptr<MotorSense> motor_sense;
std::unique_ptr<EntrySequence> entrySequence;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
  }

  motor_sense = MotorSense::create(Wire, 200_mA);
  if (!motor_sense)
  {
    Serial.println("Keypad initialization failure.");
    while (1)
      ;
  }

  auto keypad = Keypad::create(Wire);
  if (!keypad)
  {
    Serial.println("Keypad initialization failure.");
    while (1)
      ;
  }

  auto correct = []() {
    Serial.println("Opening!\n");
    return true;
  };

  auto incorrect = []() {
    Serial.println("Wrong Code!\n");
    return true;
  };

  entrySequence.reset(new EntrySequence(std::move(keypad), 2000, correct, incorrect));

  // ledControl.disable();
  motorDriver.drive(Forward{}, 150);
}

int run = 0;

void loop()
{
  //Serial.println("========================");
  //Serial.println(motor_sense->current().count());
  //Serial.println(motor_sense->stalled() ? "stalled" : "running");
  //Serial.println("----------------------");

  entrySequence->run();
  delay(50);
}