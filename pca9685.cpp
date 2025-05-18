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

    int curMode = read_reg(0x00);

    write_reg( 0,  0x01 );
    curMode = read_reg(0x00);

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

void
PCA9685::set_pwm_map( uint16_t min, uint16_t max)
{
    // 140 to 720 with range = 4096
    PWM_MIN = min;
    PWM_MAX = max;
}

#define I2Cdelay   75
void
PCA9685::set_pwm(uint8_t channel, uint16_t on, uint16_t off)
{
    int channel_offset = 6 + 4 * channel;

    if( on < 10 ) on = 10;
    if( on != curPwmOn[channel] )
    {
        curPwmOn[channel] = on;
        write_reg(channel_offset + 1,  (on >> 8 ) & 0x0F );
        usleep( I2Cdelay );
        write_reg(channel_offset, on & 0xFF);
        usleep( I2Cdelay );
    }
    
    if( off != curPwmOff[channel] )
    {
        curPwmOff[channel] = off;
        write_reg(channel_offset + 3, ( off >> 8 ) & 0x0F );
        usleep( I2Cdelay );
        write_reg(channel_offset + 2, off & 0xFF);
        usleep( I2Cdelay );
    }

}

void
PCA9685::set_pwm_freq(uint16_t freq_hz)
{
    float prescaleval = 25000000; // 25MHz
          prescaleval /= 4096; // 12-bit
          prescaleval /= freq_hz;
          prescaleval -= 1;

    int prescale = (int)(prescaleval + 0.5);

    int oldmode = read_reg(0x00);
    int newmode = (oldmode & 0x7F) | 0x10;
    write_reg(0x00, newmode);
    write_reg(0xFE, prescale);
    write_reg(0x00, oldmode);
    sleep(0.005);
    write_reg(0x00, oldmode | 0x80);
}


void
PCA9685::write_reg(int reg, int value)
{
    int ret = -1;
    do
    {
        ret = wiringPiI2CWriteReg8(file_descriptor_, reg, value);
    } while( 0 != ret );
}

int8_t
PCA9685::read_reg(int reg)
{
    return wiringPiI2CReadReg8(file_descriptor_, reg);
}
