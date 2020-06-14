#pragma once

namespace constants
{

    // constexpr uint8_t RESET_RFID = 0;
    // constexpr uint8_t SS_RFID = 2;

    constexpr uint8_t RED_PIN = D4;
    constexpr uint8_t GREEN_PIN = D3;
    constexpr uint8_t BLUE_PIN = 255;

    constexpr uint8_t MOTOR_ENABLE = D5;
    constexpr uint8_t MOTOR_R1 = D6;
    constexpr uint8_t MOTOR_R2 = D0;

    // I2C bus pins
    // - D1
    // - D2

    constexpr unsigned long KEYPAD_TIMEOUT = 2000;       //ms
    constexpr unsigned long MOTOR_TIMEOUT = 8000;        //ms
    constexpr unsigned long REJECT_LIGHT_TIMEOUT = 1500; //ms

} // namespace constants