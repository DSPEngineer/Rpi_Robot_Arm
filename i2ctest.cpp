/******************************************************************************
i2ctest.cpp
Raspberry Pi I2C interface to Wii-Nunchuk Controller
Jose Pagan
05/06/201425
Github:  https://github.com/DSPEngineer/Rpi_Robot_Arm

A brief demonstration of the Raspberry Pi I2C interface, using the SparkFun
Pi Wedge breakout board.

Resources:

This example makes use of the Wiring Pi library, which streamlines the interface
the the I/O pins on the Raspberry Pi, providing an API that is similar to the
Arduino.  You can learn about installing Wiring Pi here:
https://github.com/WiringPi/WiringPi/releases

The I2C API is documented here:
https://projects.drogon.net/raspberry-pi/wiringpi/i2c-library/

The init call returns a standard file descriptor.  More detailed configuration
of the interface can be performed using ioctl calls on that descriptor.
See the wiringPi I2C implementation (wiringPi/wiringPiI2C.c) for some examples.
Parameters configurable with ioctl are documented here:
http://git.kernel.org/cgit/linux/kernel/git/torvalds/linux.git/tree/Documentation/i2c/dev-interface

Hardware connections:

This file interfaces with the SparkFun MCP4725 breakout board:
https://www.sparkfun.com/products/8736

The board was connected as follows:
(Raspberry Pi)(MCP4725)
GND  -> GND
3.3V -> Vcc
SCL  -> SCL
SDA  -> SDA

An oscilloscope probe was connected to the analog output pin of the MCP4725.

To build this file, I use the command:
>  g++ i2ctest.cpp -lwiringPi

Then to run it, first the I2C kernel module needs to be loaded.  This can be 
done using the GPIO utility.
> gpio load i2c 400
> ./a.out

This will run the MCP through its output range several times.  A rising 
sawtooth will be seen on the analog output.

Development environment specifics:
Tested on Raspberry Pi V2 hardware, running Raspbian.
Building with GCC 4.6.3 (Debian 4.6.3-14+rpi1)

This code is beerware; if you see me (or any other SparkFun employee) at the
local, and you've found our code helpful, please buy us a round!

Distributed as-is; no warranty is given.
******************************************************************************/

#include <iostream>
#include <errno.h>
#include <wiringPiI2C.h>
#include <unistd.h>
#include <iomanip>

#include "pca9685.h"
#include "pca9685.cpp"

using namespace std;

#define WII_NUNCHUK_I2C    0x52
#define PCA9685_I2C        0x40

#define PWM_FREQUENCY      60

#define MILISECONDS        1000

#define BUTTON_Z     0x01
#define BUTTON_C     0x02

typedef struct __Accelerometer
{ // Struct to save Accelerometer Data
    uint16_t X;
    uint16_t Y;
    uint16_t Z;
} ACCELEROMETER, *pACCELEROMETER;

typedef struct __Joystick
{ // Struct to save Accelerometer Data
    uint16_t X;
    uint16_t Y;
 } JOYSTICK, *pJOYSTICK;

typedef struct __Nunchuk
{
    uint32_t       id;
    ACCELEROMETER  accel;
    JOYSTICK       jstik;
    bool           btn_c;
    bool           btn_z;
} NUNCHUK, *pNUNCHUK;


#define  isButtonC( r )  ( ! ( (r) & BUTTON_C ) )
#define  isButtonZ( r )  ( ! ( (r) & BUTTON_Z ) )



int main()
{
   int result = 0;

   NUNCHUK ctrl = { 0 };

//   wiringPiSetup();

   // Initialize the interface by giving it an external device ID.
   // The Wii Nunchuk Joustick defaults to address 0x52.   
   //
   // It returns a standard file descriptor.
   // 
   cout << " INFO: Open I2C 0x" << hex << WII_NUNCHUK_I2C << " address." << endl;
   int fd_ctrl = wiringPiI2CSetup( WII_NUNCHUK_I2C );
   if ( -1 == fd_ctrl )
   {
      std::cout << "Failed to init I2C communication to Wii NunChuk Joystick.\n";
      return -1;
   }

    cout << " INFO: Open I2C 0x" << hex << PCA9685_I2C << " address." << endl;
    // Using class
    PCA9685 pwm(0x40); // PCA9685 I2C address is 0x40

    // Example: Set channel 0 to 90 degrees (assuming 1ms-2ms pulse range for 0-180 degrees)
    // 1.5ms pulse is centered, which is (1.5/20) * 4096 = 307
    pwm.set_pwm(0, 0, (uint16_t)307); 
    sleep(1);

     // Example: Set channel 0 to 180 degrees
    // 2ms pulse is (2/20) * 4096 = 409
    pwm.set_pwm(0, 0, (uint16_t)670);
    sleep(1);

    // Example: Set channel 0 to 0 degrees
    // 1ms pulse is (1/20) * 4096 = 204
   pwm.set_pwm(0, 0, (uint16_t)204);
    sleep(1);

   if( false  )
   {
      for( int i = 135; i < 730; i+=20 )
      {
         cout << "i = [" << dec << i << "]" << endl;
         pwm.set_pwm(0, 0, i);
         sleep(1);
      }
   }

   // disable encryption
//   result = wiringPiI2CWriteReg16(fd_ctrl, 0x40, 0x0000 );
   result = wiringPiI2CWriteReg8(fd_ctrl, 0xF0, 0x55 );
   result = wiringPiI2CWriteReg8(fd_ctrl, 0xFB, 0x00 );


   // Read Device ID as 16 bit words:
   int cword = wiringPiI2CReadReg16( fd_ctrl, 0xFA );
   cout << "REG: " << hex << ( cword ) << endl;
   cword = wiringPiI2CReadReg16( fd_ctrl, 0xFC );
   cout << "REG: " << hex << ( cword ) << endl;
   cword = wiringPiI2CReadReg16( fd_ctrl, 0xFE );
   cout << "REG: " << hex << ( cword ) << endl;

   // Read Device ID as 8 bit bytes:
   uint8_t chr = wiringPiI2CReadReg8(fd_ctrl, 0xFA );
   cout << "REG: " << hex << static_cast<int>( chr );

   chr = wiringPiI2CReadReg8(fd_ctrl, 0xFB );
   cout << " | " << hex << static_cast<int>(chr);

   chr = wiringPiI2CReadReg8(fd_ctrl, 0xFC );
   ctrl.id |= (uint32_t)chr << 24;
   cout << " | " << hex << static_cast<int>(chr) << "(" << ctrl.id << ")" ;

   chr = wiringPiI2CReadReg8(fd_ctrl, 0xFD );
   ctrl.id |= (uint32_t)chr << 16;
   cout << " | " << hex << static_cast<int>(chr) << "(" << ctrl.id << ")" ;

   chr = wiringPiI2CReadReg8(fd_ctrl, 0xFE );
   ctrl.id |= (uint32_t)chr << 8;
   cout << " | " << hex << static_cast<int>(chr) << "(" << ctrl.id << ")" ;

   chr = wiringPiI2CReadReg8(fd_ctrl, 0xFF );
   ctrl.id |= (uint32_t)chr << 0;
   cout << " | " << hex << static_cast<int>(chr)   << "(" << ctrl.id << ")" << endl;

   cout << "Device ID: " << showbase << hex << (ctrl.id) << endl << endl;

   // Read Device Data as 16 bit words:
   cword = wiringPiI2CReadReg16( fd_ctrl, 0x00 );
   cout << "DAT0: " << hex << ( cword ) << endl;
   cword = wiringPiI2CReadReg16( fd_ctrl, 0x02 );
   cout << "DAT2: " << hex << ( cword ) << endl;
   cword = wiringPiI2CReadReg16( fd_ctrl, 0x04 );
   cout << "DAT4: " << hex << ( cword ) << endl;

   // Read Device Data as 8 bit bytes:
   chr = wiringPiI2CReadReg8(fd_ctrl, 0x00 );
   cout << "DAT: " << hex << static_cast<int>( chr );
   chr = wiringPiI2CReadReg8(fd_ctrl, 0x01 );
   cout << " | " << hex << static_cast<int>(chr);

   chr = wiringPiI2CReadReg8(fd_ctrl, 0x02 );
   cout << " | " << hex << static_cast<int>(chr);
   chr = wiringPiI2CReadReg8(fd_ctrl, 0x03 );
   cout << " | " << hex << static_cast<int>(chr);

   chr = wiringPiI2CReadReg8(fd_ctrl, 0x04 );
   cout << " | " << hex << static_cast<int>(chr);
   chr = wiringPiI2CReadReg8(fd_ctrl, 0x05 );
   cout << " | " << hex << static_cast<int>(chr) << endl;


   while( true )
   {
      // First read the last byte to check for Button-C
      chr = wiringPiI2CReadReg8(fd_ctrl, 0x05 );

      if( isButtonC( chr ) && !ctrl.btn_c )
      {
         ctrl.btn_c = true;
         cout << "Button C pressed" << endl;
      }
      else if( !isButtonC( chr ) && ctrl.btn_c )
      {
         ctrl.btn_c = false;
         cout << "Button C released" << endl;
      }

      // First read the last byte to check for Button-Z
      if( isButtonZ( chr ) && !ctrl.btn_z )
      {
         ctrl.btn_z = true;
         cout << "Button Z pressed" << endl;
      }
      else if( !isButtonZ( chr ) && ctrl.btn_z )
      {
         ctrl.btn_z = false;
         cout << "Button Z released" << endl;
      }

      // First read the last byte to check for exit condition,
      //   both buttons, Button-C and Button-Z
      if( isButtonC( chr ) && isButtonZ( chr ) )
      {
         cout << "Button C and Z pressed, exit signal." << endl;
         break;
      }
//      result = wiringPiI2CWriteReg16(fd_ctrl, 0x40, (i & 0xfff) );

      // Need to set low order 2-bits for Accelerometer
      ctrl.accel.Z = ( chr & 0xC0 ) >> 6;
      ctrl.accel.Y = ( chr & 0x30 ) >> 4;
      ctrl.accel.X = ( chr & 0x0C ) >> 2;

      // Need to set high order 8-bits for Accelerometer
      ctrl.accel.X |= (uint16_t)( wiringPiI2CReadReg8(fd_ctrl, 0x02 ) & 0xFF ) << 2;
      ctrl.accel.Y |= (uint16_t)( wiringPiI2CReadReg8(fd_ctrl, 0x03 ) & 0xFF ) << 2;
      ctrl.accel.Z |= (uint16_t)( wiringPiI2CReadReg8(fd_ctrl, 0x04 ) & 0xFF ) << 2;

      // Read Joystick X
      ctrl.jstik.X = wiringPiI2CReadReg8(fd_ctrl, 0x00 );
      ctrl.jstik.Y = wiringPiI2CReadReg8(fd_ctrl, 0x01 );

      uint16_t jsMin  = 135;
      uint16_t jsMax  = 820;
      uint16_t jsStep = 1 + ( ( jsMax - jsMin ) / 4096 );
      
      uint16_t pwmX = ( ( ( jsMax - jsMin ) * ctrl.jstik.X / 256)  ) + ( jsMin / 2 );
      uint16_t pwmY = ( ( ( jsMax - jsMin ) * ctrl.jstik.Y / 256 ) ) + ( jsMin / 2 );


      pwm.set_pwm( 0, 0, pwmX );
      pwm.set_pwm( 1, 0, pwmY-29 );
      pwm.set_pwm( 2, 0, 404 );

      cout << noshowbase << "REG ==> JX: 0x" << setw(4) << setfill('0') << (int)ctrl.jstik.X;
      cout << dec << " (" << pwmX << ")";
      cout << " | JY: 0x"  << setw(4) << setfill('0') << hex << (int)ctrl.jstik.Y;
      cout << dec << " (" << pwmY << ")";
      cout << " :: AX: " << setw(4) << setfill('0') << hex << (uint16_t)ctrl.accel.X;
      cout << " :: AY: " << setw(4) << setfill('0') << hex << (uint16_t)ctrl.accel.Y;
      cout << " :: AZ: " << setw(4) << setfill('0') << hex << (uint16_t)ctrl.accel.Z << endl;
//      usleep( MILISECONDS * 100 );
      usleep( MILISECONDS * 100 );

   }

   exit( result );
}
