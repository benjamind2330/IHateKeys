#pragma once
#include "pin_control.h"
#include "units.h"

#include <Adafruit_INA219.h>

#include <limits>
#include <memory>

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

    class MotorSense
    {
    public:
        static std::unique_ptr<MotorSense> create(TwoWire &i2cWire)
        {
            std::unique_ptr<MotorSense> output{new MotorSense(i2cWire)};
            if (output->valid_)
            {
                return output;
            }

            return nullptr;
        }

        units::MilliAmps current()
        {
            auto val = units::MilliAmps::rep{ina219_.getCurrent_mA()};
            return units::MilliAmps{val};
        }

        void printDetails()
        {
            float shuntvoltage = 0;
            float busvoltage = 0;
            float current_mA = 0;
            float loadvoltage = 0;
            float power_mW = 0;

            shuntvoltage = ina219_.getShuntVoltage_mV();
            busvoltage = ina219_.getBusVoltage_V();
            current_mA = ina219_.getCurrent_mA();
            power_mW = ina219_.getPower_mW();
            loadvoltage = busvoltage + (shuntvoltage / 1000);

            Serial.print("Bus Voltage:   ");
            Serial.print(busvoltage);
            Serial.println(" V");
            Serial.print("Shunt Voltage: ");
            Serial.print(shuntvoltage);
            Serial.println(" mV");
            Serial.print("Load Voltage:  ");
            Serial.print(loadvoltage);
            Serial.println(" V");
            Serial.print("Current:       ");
            Serial.print(current_mA);
            Serial.println(" mA");
            Serial.print("Power:         ");
            Serial.print(power_mW);
            Serial.println(" mW");
            Serial.println("");
        }

    private:
        MotorSense(TwoWire &i2cWire)
        {
            if (!ina219_.begin(&i2cWire))
            {
                return;
            }

            ina219_.setCalibration_32V_1A();
            valid_ = true;
        }

        bool valid_ = false;
        Adafruit_INA219 ina219_;
    };

} // namespace hardware