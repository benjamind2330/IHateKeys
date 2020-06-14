#pragma once

#include <Arduino.h>
#include <MPR121.h>
#include <Streaming.h>

#include <memory>
#include <chrono>

namespace constants
{

    const MPR121::DeviceAddress device_address = MPR121::ADDRESS_5A;
    const bool fast_mode = true;

    const size_t loop_delay = 500;

    const uint8_t channel = 0;

    const uint8_t physical_channel_count = 10;
    const MPR121::ProximityMode proximity_mode = MPR121::ProximityMode::DISABLED;

    const uint8_t touch_threshold = 40;
    const uint8_t release_threshold = 20;
    const uint8_t touch_debounce = 1;
    const uint8_t release_debounce = 1;

    const MPR121::BaselineTracking baseline_tracking = MPR121::BASELINE_TRACKING_INIT_10BIT;
    const uint8_t charge_discharge_current = 63;
    const MPR121::ChargeDischargeTime charge_discharge_time = MPR121::CHARGE_DISCHARGE_TIME_HALF_US;
    const MPR121::FirstFilterIterations first_filter_iterations = MPR121::FIRST_FILTER_ITERATIONS_34;
    const MPR121::SecondFilterIterations second_filter_iterations = MPR121::SECOND_FILTER_ITERATIONS_10;
    const MPR121::SamplePeriod sample_period = MPR121::SAMPLE_PERIOD_1MS;
} // namespace constants

class Keypad
{

public:
    using Key = int8_t;
    static constexpr Key INVALID_KEY = -1;

    static std::unique_ptr<Keypad> create(TwoWire &i2cWire)
    {
        std::unique_ptr<Keypad> output{new Keypad(i2cWire)};
        if (output->valid_)
        {
            return output;
        }
        return nullptr;
    }

    uint16_t touchStatus()
    {
        return mpr121_.getTouchStatus(constants::device_address);
    }

    Key currentPress()
    {
        const auto status = touchStatus();

        if (!status)
        {
            return INVALID_KEY;
        }

        bool foundBit = false;
        Key output = 0;
        for (unsigned i = 0; i < constants::physical_channel_count; ++i)
        {
            if (bitRead(status, i))
            {
                if (foundBit)
                {
                    return INVALID_KEY;
                }
                foundBit = true;
                output = i;
            }
        }
        return output;
    }

    void printDetails()
    {

        uint8_t channel_count = mpr121_.getChannelCount();
        Serial << "channel_count: " << channel_count << "\n";
        uint8_t running_channel_count = mpr121_.getRunningChannelCount();
        Serial << "running_channel_count: " << running_channel_count << "\n";

        uint16_t touch_status = mpr121_.getTouchStatus(constants::device_address);
        Serial << "touch_status: " << _BIN(touch_status) << "\n";
        if (mpr121_.overCurrentDetected(touch_status))
        {
            Serial << "Over current detected!\n\n";
            mpr121_.startChannels(constants::physical_channel_count,
                                  constants::proximity_mode);
            return;
        }
        bool any_touched = mpr121_.anyTouched(touch_status);
        Serial << "any_touched: " << any_touched << "\n";
        bool device_channel_touched = mpr121_.deviceChannelTouched(touch_status,
                                                                   constants::channel);
        Serial << "device_channel_touched: " << device_channel_touched << "\n";
    }

private:
    Keypad(TwoWire &i2cWire)
    {
        mpr121_.setupSingleDevice(i2cWire,
                                  constants::device_address,
                                  constants::fast_mode);

        mpr121_.setAllChannelsThresholds(constants::touch_threshold,
                                         constants::release_threshold);
        mpr121_.setDebounce(constants::device_address,
                            constants::touch_debounce,
                            constants::release_debounce);
        mpr121_.setBaselineTracking(constants::device_address,
                                    constants::baseline_tracking);
        mpr121_.setChargeDischargeCurrent(constants::device_address,
                                          constants::charge_discharge_current);
        mpr121_.setChargeDischargeTime(constants::device_address,
                                       constants::charge_discharge_time);
        mpr121_.setFirstFilterIterations(constants::device_address,
                                         constants::first_filter_iterations);
        mpr121_.setSecondFilterIterations(constants::device_address,
                                          constants::second_filter_iterations);
        mpr121_.setSamplePeriod(constants::device_address,
                                constants::sample_period);

        mpr121_.startChannels(constants::physical_channel_count,
                              constants::proximity_mode);

        if (mpr121_.communicating(constants::device_address))
        {
            valid_ = true;
        }
    }

    MPR121 mpr121_;
    bool valid_{false};
};

class Code
{
public:
    using value_type = uint64_t;
    Code() = default;
    Code(value_type digits) : digits_{digits}
    {
        do
        {
            digits /= 10;
            codeLength_++;
        } while (digits);
    }

    void addDigit(Keypad::Key digit)
    {
        // TODO: Handle overflow here?
        digits_ = digits_ * 10 + digit;
        codeLength_++;
    }

    value_type code() const { return digits_; }

    uint16_t length() const { return codeLength_; }

private:
    value_type digits_{0};
    uint16_t codeLength_{0};
};

class EntrySequence
{

public:
    using Action = std::function<bool()>;

    EntrySequence(std::unique_ptr<Keypad> keypad, unsigned long timeoutTime, Action correct, Action incorrect) : state_{State::STANDBY},
                                                                                                                 keypad_{std::move(keypad)},
                                                                                                                 correctCode_{2330},
                                                                                                                 lastTime_{millis()},
                                                                                                                 timeoutTime_{timeoutTime},
                                                                                                                 doForIncorrectCode_{incorrect},
                                                                                                                 doForCorrectCode_{correct},
                                                                                                                 lastKeyPress_{Keypad::INVALID_KEY}
    {
    }

    void run()
    {
        switch (state_)
        {
        case State::STANDBY:
            enteredCode_ = Code{};
            lastKeyPress_ = keypad_->currentPress();
            if (lastKeyPress_ != Keypad::INVALID_KEY)
            {
                changeState(State::WAIT_KEY_RELEASED, true);
            }
            break;

        case State::WAIT_KEY_RELEASED:
            if (keypad_->currentPress() == Keypad::INVALID_KEY)
            {
                changeState(State::RECEIVING, true);
            }
            break;

        case State::RECEIVING:
            if (lastKeyPress_ != Keypad::INVALID_KEY)
            {
                enteredCode_.addDigit(lastKeyPress_);
            }

            lastKeyPress_ = keypad_->currentPress();
            if (lastKeyPress_ != Keypad::INVALID_KEY)
            {
                changeState(State::WAIT_KEY_RELEASED, true);
                break;
            }

            if (enteredCode_.length() == correctCode_.length())
            {
                changeState(State::CHECK_SEQUENCE, true);
                break;
            }

            if (millis() - lastTime_ > timeoutTime_)
            {
                changeState(State::CHECK_SEQUENCE, false);
            }
            break;

        case State::CHECK_SEQUENCE:
            if (enteredCode_.code() == correctCode_.code())
            {
                changeState(State::CORRECT_ACTION, false);
            }
            else
            {
                changeState(State::WAIT_TIMEOUT, false);
            }
            break;

        case State::WAIT_TIMEOUT:
            if (keypad_->currentPress() != Keypad::INVALID_KEY)
            {
                lastTime_ = millis();
            }

            if (millis() - lastTime_ > timeoutTime_)
            {
                changeState(State::INCORRECT_ACTION, false);
            }
            break;

        case State::CORRECT_ACTION:
            if (doForCorrectCode_())
            {
                changeState(State::STANDBY, false);
            }
            break;

        case State::INCORRECT_ACTION:
            if (doForIncorrectCode_())
            {
                changeState(State::STANDBY, false);
            }
            break;
        }
    }

private:
    enum class State
    {
        STANDBY = 0,
        WAIT_KEY_RELEASED,
        RECEIVING,
        CHECK_SEQUENCE,
        WAIT_TIMEOUT,
        CORRECT_ACTION,
        INCORRECT_ACTION
    };

    void changeState(State to, bool updateTime)
    {
#define PRINT_STATE_CHANGE
#ifdef PRINT_STATE_CHANGE
        static const std::array<char *, 7> StateString = {
            "STANDBY",
            "WAIT_KEY_RELEASED",
            "RECEIVING",
            "CHECK_SEQUENCE",
            "WAIT_TIMEOUT",
            "CORRECT_ACTION",
            "INCORRECT_ACTION"};

        Serial << "Changing from " << StateString[static_cast<int>(state_)] << " to " << StateString[static_cast<int>(to)] << "\n";
#endif

        state_ = to;
        if (updateTime)
        {
            lastTime_ = millis();
        }
    }

    State state_;
    std::unique_ptr<Keypad> keypad_;
    Code correctCode_;
    Code enteredCode_{};
    unsigned long lastTime_;
    unsigned long timeoutTime_;
    Action doForIncorrectCode_;
    Action doForCorrectCode_;
    Keypad::Key lastKeyPress_;
};
