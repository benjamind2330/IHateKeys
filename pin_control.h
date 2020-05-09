#pragma once

#include <cstdint>

#include "traits_extension.h"

namespace hardware
{

    struct Pin
    {
        struct ReadMode
        {
        };

        struct WriteMode
        {
        };

        using Id = uint8_t;
        using AnalogValue = int;
    };

    template <Pin::Id PIN, typename TMode = Pin::ReadMode>
    class PinControl
    {
    public:
        template <typename U = TMode,
                  std::enable_if_t<std::is_same<U, Pin::ReadMode>::value, int> = 0>
        PinControl() noexcept
        {
            pinMode(PIN, INPUT);
        }

        template <typename U = TMode,
                  std::enable_if_t<std::is_same<U, Pin::WriteMode>::value, int> = 0>
        PinControl() noexcept
        {
            pinMode(PIN, OUTPUT);
        }

        template <typename U = TMode,
                  std::enable_if_t<std::is_same<U, Pin::WriteMode>::value, int> = 0>
        void write(Pin::AnalogValue value) noexcept
        {
            analogWrite(PIN, value);
        }

        template <typename U = TMode,
                  std::enable_if_t<std::is_same<U, Pin::WriteMode>::value, int> = 0>
        void high() noexcept
        {
            digitalWrite(PIN, HIGH);
        }

        template <typename U = TMode,
                  std::enable_if_t<std::is_same<U, Pin::WriteMode>::value, int> = 0>
        void low() noexcept
        {
            digitalWrite(PIN, LOW);
        }
    };

} // namespace hardware
