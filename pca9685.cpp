#include "pca9685.h"

#define RESET_BIT 0x80
#define SLEEP_BIT 0x10

PCA9685::PCA9685(uint8_t address) : address_(address)
{
    // Initialize the interface by giving it an external device ID.
     file_descriptor_ = wiringPiI2CSetup( address_ );

    if (file_descriptor_ == -1)
    {
        std::cout << "Failed to init I2C communication to Motor Controller PCA9685 Joystick.\n";
    }

    int deviceInitialMode = read_reg(0x00);

    // Set the device mode to awake and out of reset
    if( 0 != write_reg( 0x00,  0x01 ) )
    {
        std::cout << "PCA9685 Failed to set PCA9685 to awake mode.\n";
    }

    // check the bits, to be certain we are in a good state
    int curMode = read_reg(0x00);

    if( ! (curMode & RESET_BIT ) )
    {
        std::cout << "PCA9685 is not in reset mode.\n";
    }

    if( ! (curMode & SLEEP_BIT ) )
    {
        std::cout << "PCA9685 is not in sleep mode.\n";
    }

    set_pwm_freq(60); // Set frequency to 60Hz
}


int  // No error return since we are saving class variables
PCA9685::set_pwm_map( uint16_t min, uint16_t max)
{
    // Set ranges inside of this class
    PWM_MIN = min;
    PWM_MAX = max;

    return 0;
}


#define I2Cdelay   75
int
PCA9685::set_pwm(uint8_t channel, uint16_t on, uint16_t off)
{
    int retVal  = 0;
    int funcRet = 0;
    int channel_offset = 6 + 4 * channel;

    if( on < 10 )
    { // For this board, clamp to a minimum of 10
        on = 10;
    }

    if( on != curPwmOn[channel] )
    { // update the ON value, if necessary
        curPwmOn[channel] = on;
        if( funcRet = write_reg(channel_offset + 1,  (on >> 8 ) & 0x0F ) )
        {
            std::cout << "PCA9685 Failed to write ON (HI) value for channel " << (int)channel << ".\n";
            retVal = -1;
        }

        usleep( I2Cdelay );

        if( funcRet = write_reg(channel_offset, on & 0xFF) )
        {
            std::cout << "PCA9685 Failed to write ON (LO) value for channel " << (int)channel << ".\n";
            retVal = -1;
        }

        usleep( I2Cdelay );
    }

    if( off != curPwmOff[channel] )
    { // update the OFF value, if necessary
        curPwmOff[channel] = off;
        if( funcRet = write_reg(channel_offset + 3, ( off >> 8 ) & 0x0F ) )
        {
            std::cout << "PCA9685 Failed to write OFF (HI) value for channel " << (int)channel << ".\n";
            retVal = -1;
        }

        usleep( I2Cdelay );

        if( funcRet = write_reg(channel_offset + 2, off & 0xFF) )
        {
            std::cout << "PCA9685 Failed to write OFF (LO) value for channel " << (int)channel << ".\n";
            retVal = -1;
        }

        usleep( I2Cdelay );
    }

    return retVal;
}


int
PCA9685::set_pwm_freq(uint16_t freq_hz)
{
    int retVal = 0;

    float prescaleval = 25000000; // 25MHz
          prescaleval /= 4096; // 12-bit
          prescaleval /= freq_hz;
          prescaleval -= 1;

    int prescale = (int)(prescaleval + 0.5);

    int oldmode = read_reg(0x00);

    int newmode = (oldmode & 0x7F) | 0x10;

    if( retVal = write_reg(0x00, newmode) )
    {
        std::cout << "PCA9685 Failed to set PCA9685 to sleep mode.\n";
    }
    else if( retVal = write_reg(0xFE, prescale) )
    {
        std::cout << "PCA9685 Failed to set PCA9685 prescale value.\n";
    }
    else if( retVal = write_reg(0x00, oldmode) )
    {
        std::cout << "PCA9685 Failed to set PCA9685 to awake mode.\n";
    }
    else if( sleep(0.005), retVal = write_reg(0x00, oldmode | 0x80) )
    {
        std::cout << "PCA9685 Failed to set PCA9685 to reset mode.\n";
    }

    return retVal;

}


int8_t
PCA9685::write_reg(int reg, int value)
{
    return wiringPiI2CWriteReg8(file_descriptor_, reg, value);
}


int8_t
PCA9685::read_reg(int reg)
{
    return wiringPiI2CReadReg8(file_descriptor_, reg);
}
