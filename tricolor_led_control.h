#include "pin_control.h"

namespace hardware
{

struct Led
{
    enum class Colour
    {
        RED,
        GREEN,
        BLUE,
        NONE
    };
};

template <Pin::Id RED, Pin::Id GREEN, Pin::Id BLUE>
class TricolourControl
{
    using Colour = Led::Colour;

public:
    void enable(Colour colour, Pin::AnalogValue value = std::numeric_limits<Pin::AnalogValue>::max())
    {
        disable();

        switch (colour)
        {
        case Colour::RED:
            red_.write(value);
            break;
        case Colour::GREEN:
            green_.write(value);
            break;
        case Colour::BLUE:
            blue_.write(value);
            break;
        case Colour::NONE:
            break;
        }

        currentlyEnabled_ = colour;
    }

    void disable()
    {
        red_.low();
        green_.low();
        blue_.low();
    }

    Colour currentlyEnabled() const noexcept
    {
        return currentlyEnabled_;
    }

private:
    Colour currentlyEnabled_{Colour::NONE};
    PinControl<RED, Pin::WriteMode> red_{};
    PinControl<GREEN, Pin::WriteMode> green_{};
    PinControl<BLUE, Pin::WriteMode> blue_{};
};

} // namespace hardware