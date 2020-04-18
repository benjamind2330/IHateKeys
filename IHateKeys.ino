
#include <vector>
#include <limits>

#include "tricolor_led_control.h"

using namespace hardware;

TricolourControl<4, 0, 5> ledControl;

void setup()
{
  Serial.begin(9600);

  ledControl.disable();
}

uint8_t writeVal = 0;
bool up = true;
Led::Colour colour = Led::Colour::BLUE;

void loop()
{
  if (up)
  {
    ledControl.enable(colour, writeVal++);
  }
  else
  {
    ledControl.enable(colour, writeVal--);
  }

  if (up && writeVal == std::numeric_limits<uint8_t>::max())
  {
    up = false;
  }
  else if (!up && writeVal == 0)
  {
    up = true;
    switch (colour)
    {

    case Led::Colour::BLUE:
      colour = Led::Colour::RED;
      break;
    case Led::Colour::RED:
      colour = Led::Colour::GREEN;
      break;
    case Led::Colour::GREEN:
      colour = Led::Colour::BLUE;
      break;
    }
  }

  delay(10);
}
