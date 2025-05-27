#pragma once

#include <iostream>
#include <wiringPiI2C.h>
#include <unistd.h>

class PCA9685 {
public:
    PCA9685(uint8_t address);

    int     set_pwm(uint8_t channel, uint16_t on, uint16_t off);
    int     set_pwm_freq(uint16_t freq_hz);
    int     set_pwm_map( uint16_t min, uint16_t max);

private:
    const uint16_t      PWM_BITS=12;
    const uint16_t      PWM_STEP = 2^PWM_BITS;

    ulong               PWM_MIN = 135;
    ulong               PWM_MAX = 660;

    uint16_t            curPwmOn[16]  = {0};
    uint16_t            curPwmOff[16] = {0};

    int address_;
    int file_descriptor_;
    int deviceInitialMode;

    int8_t  write_reg(int reg, int value);
    int8_t  read_reg(int reg);
};