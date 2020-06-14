#pragma once

#include "tricolor_led_control.h"
#include "motor_driver.h"

template <typename TLedControl, typename TMotorDriver>
class DoorActuation
{

public:
    DoorActuation(std::unique_ptr<MotorSense> motorSense) : motorSense_{std::move(motorSense)}
    {
        ledControl_.disable();
        motorDriver_.freeRun();
    }

    bool open()
    {

        switch (openingState_)
        {
        case OpeningState::WAITING:
            actionTime_ = millis();
            ledControl_.enable(hardware::Led::Colour::GREEN);
            motorDriver_.drive(hardware::Forward{}, 100);
            openingState_ = OpeningState::DRIVING;
            return false;
        case OpeningState::DRIVING:
            if ((millis() - actionTime_) > constants::MOTOR_TIMEOUT)
            {
                openingState_ = OpeningState::FINISH;
            }
            else if (motorSense_->stalled())
            {
                openingState_ = OpeningState::BRAKING;
            }
            return false;
        case OpeningState::BRAKING:
            motorDriver_.dynamicBraking();
            if ((millis() - actionTime_) > constants::MOTOR_TIMEOUT)
            {
                openingState_ = OpeningState::FINISH;
            }
            return false;
        case OpeningState::FINISH:
            motorDriver_.freeRun();
            ledControl_.disable();
            openingState_ = OpeningState::WAITING;
            return true;
        }
        return true;
    }

    bool rejectAccess()
    {
        switch (rejectedState_)
        {

        case RejectedState::WAITING:
            ledControl_.enable(hardware::Led::Colour::RED);
            actionTime_ = millis();
            rejectedState_ = RejectedState::LED_ON;
            return false;

        case RejectedState::LED_ON:
            if ((millis() - actionTime_) > constants::REJECT_LIGHT_TIMEOUT)
            {
                rejectedState_ = RejectedState::RESET_LED;
            }
            return false;

        case RejectedState::RESET_LED:
            ledControl_.disable();
            rejectedState_ = RejectedState::WAITING;
            return true;
        }
        return true;
    }

private:
    enum class OpeningState
    {
        WAITING = 0,
        DRIVING,
        BRAKING,
        FINISH
    };

    enum class RejectedState
    {
        WAITING = 0,
        LED_ON,
        RESET_LED
    };

    OpeningState openingState_{OpeningState::WAITING};
    TLedControl ledControl_{};
    TMotorDriver motorDriver_{};
    std::unique_ptr<MotorSense> motorSense_;
    unsigned long actionTime_ = 0;
    RejectedState rejectedState_{RejectedState::WAITING};
};