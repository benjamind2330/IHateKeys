#pragma once
#include "pin_control.h"

#include <limits>

namespace hardware
{

    struct Forward
    {
    };

    struct Reverse
    {
    };

    using Percentage = float;

    template <Pin::Id ENABLE, Pin::Id R1, Pin::Id R2>
    class MotorDriver
    {
    public:
        MotorDriver()
        {
            freeRun();
        }

        void drive(Forward, Percentage speed)
        {
            auto pinVal = percentageToPinVal(speed);
            r2_.low();
            r1_.high();
            enable_.write(pinVal);
        }

        void drive(Reverse, Percentage speed)
        {
            auto pinVal = percentageToPinVal(speed);
            r1_.low();
            r2_.high();
            enable_.write(pinVal);
        }

        void dynamicBraking()
        {
            enable_.low();
            r1_.high();
            r2_.high();
        }

        void freeRun()
        {
            enable_.low();
            r1_.low();
            r2_.low();
        }

    private:
        Pin::AnalogValue percentageToPinVal(Percentage percentage) const
        {
            auto val = static_cast<Pin::AnalogValue>(std::abs(percentage) / 100.0f * PWMRANGE);
            Serial.println("Creating analog val " + String(val));
            return val;
        }

        PinControl<ENABLE, Pin::WriteMode> enable_{};
        PinControl<R1, Pin::WriteMode> r1_{};
        PinControl<R2, Pin::WriteMode> r2_{};
    };

} // namespace hardware