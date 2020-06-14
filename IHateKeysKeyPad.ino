#include "tricolor_led_control.h"
#include "rfid_access.h"
#include "motor_driver.h"
#include "constants.h"
#include "pin_control.h"
#include "keypad.h"
#include "door_actuation.h"
#include <memory>

using namespace hardware;
using namespace units::literals;

using LedControl = TricolourControl<constants::RED_PIN, constants::GREEN_PIN, constants::BLUE_PIN>;
using MotorDrive = MotorDriver<constants::MOTOR_ENABLE, constants::MOTOR_R1, constants::MOTOR_R2>;
std::unique_ptr<EntrySequence> entrySequence;
std::unique_ptr<DoorActuation<LedControl, MotorDrive>> doorActuation;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
  }

  auto motorSense = MotorSense::create(Wire, 200_mA);
  if (!motorSense)
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

  doorActuation.reset(new DoorActuation<LedControl, MotorDrive>(std::move(motorSense)));

  auto correct = [&doorActuation]() {
    return doorActuation->open();
  };

  auto incorrect = [&]() {
    return doorActuation->rejectAccess();
  };

  entrySequence.reset(new EntrySequence(std::move(keypad), constants::KEYPAD_TIMEOUT, correct, incorrect));
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