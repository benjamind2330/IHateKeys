#include "tricolor_led_control.h"
#include "rfid_access.h"
#include "motor_driver.h"
#include "constants.h"
#include "pin_control.h"
#include <memory>

using namespace hardware;

// TricolourControl<constants::RED_PIN, constants::GREEN_PIN, constants::BLUE_PIN> ledControl;
MotorDriver<constants::MOTOR_ENABLE, constants::MOTOR_R1, constants::MOTOR_R2> motorDriver;
std::unique_ptr<MotorSense> motor_sense;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
  }

  motor_sense = MotorSense::create(Wire);

  // ledControl.disable();
  motorDriver.drive(Forward{}, 150);
}

void loop()
{
  Serial.println("========================");
  Serial.println(motor_sense->current().count());
  delay(1000);
}