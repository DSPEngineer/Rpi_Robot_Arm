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


using namespace std;

#define WII_NUNCHUK        0x52

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
    ACCELEROMETER  accel;
    JOYSTICK       jstik;
    bool           btn_c;
    bool           btn_z;
} NUNCHUK, *pNUNCHUK;


int main()
{
   int result;

   // Initialize the interface by giving it an external device ID.
   // The Wii Nunchuk Joustick defaults to address 0x52.   
   //
   // It returns a standard file descriptor.
   // 
   cout << " INFO: Open I2C " << WII_NUNCHUK << " address." << endl;
   int fd = wiringPiI2CSetup( WII_NUNCHUK );
   if ( -1 == fd )
   {
      std::cout << "Failed to init I2C communication to Wii NunChuk Joystick.\n";
      return -1;
   }
   std::cout << "I2C communication successfully setup.\n";

   // disable encryption

//   result = wiringPiI2CWriteReg16(fd, 0x40, 0x0000 );
   result = wiringPiI2CWriteReg8(fd, 0xF0, 0x55 );
   result = wiringPiI2CWriteReg8(fd, 0xFB, 0x00 );

   // Read Device ID as 16 bit words:
   int cword = wiringPiI2CReadReg16( fd, 0xFA );
   cout << "REG: " << hex << ( cword ) << endl;
   cword = wiringPiI2CReadReg16( fd, 0xFC );
   cout << "REG: " << hex << ( cword ) << endl;
   cword = wiringPiI2CReadReg16( fd, 0xFE );
   cout << "REG: " << hex << ( cword ) << endl;

   // Read Device ID as 8 bit bytes:
   uint8_t chr = wiringPiI2CReadReg8(fd, 0xFA );
   cout << "REG: " << hex << static_cast<int>( chr );
   chr = wiringPiI2CReadReg8(fd, 0xFB );
   cout << " | " << hex << static_cast<int>(chr);

   chr = wiringPiI2CReadReg8(fd, 0xFC );
   cout << " | " << hex << static_cast<int>(chr);
   chr = wiringPiI2CReadReg8(fd, 0xFD );
   cout << " | " << hex << static_cast<int>(chr);

   chr = wiringPiI2CReadReg8(fd, 0xFE );
   cout << " | " << hex << static_cast<int>(chr);
   chr = wiringPiI2CReadReg8(fd, 0xFF );
   cout << " | " << hex << static_cast<int>(chr) << endl;


   // Read Device Data as 16 bit words:
   cword = wiringPiI2CReadReg16( fd, 0x00 );
   cout << "DAT0: " << hex << ( cword ) << endl;
   cword = wiringPiI2CReadReg16( fd, 0x02 );
   cout << "DAT2: " << hex << ( cword ) << endl;
   cword = wiringPiI2CReadReg16( fd, 0x04 );
   cout << "DAT4: " << hex << ( cword ) << endl;

   // Read Device Data as 8 bit bytes:
   chr = wiringPiI2CReadReg8(fd, 0x00 );
   cout << "DAT: " << hex << static_cast<int>( chr );
   chr = wiringPiI2CReadReg8(fd, 0x01 );
   cout << " | " << hex << static_cast<int>(chr);

   chr = wiringPiI2CReadReg8(fd, 0x02 );
   cout << " | " << hex << static_cast<int>(chr);
   chr = wiringPiI2CReadReg8(fd, 0x03 );
   cout << " | " << hex << static_cast<int>(chr);

   chr = wiringPiI2CReadReg8(fd, 0x04 );
   cout << " | " << hex << static_cast<int>(chr);
   chr = wiringPiI2CReadReg8(fd, 0x05 );
   cout << " | " << hex << static_cast<int>(chr) << endl;

   #define  isButtonC( r )  ( ! ( (r) & BUTTON_C ) )
   #define  isButtonZ( r )  ( ! ( (r) & BUTTON_Z ) )

   NUNCHUK ctrl = { 0 };

   while( true )
   {
      // First read the last byte to check for exit condition Button-C
      chr = wiringPiI2CReadReg8(fd, 0x05 );

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

      if( isButtonC( chr ) && isButtonZ( chr ) )
      {
         cout << "Button C and Z pressed, exit signal." << endl;
         break;
      }
//      result = wiringPiI2CWriteReg16(fd, 0x40, (i & 0xfff) );

      // Need to set high order byte for Accelerometer
      ctrl.accel.Z = ( chr & 0xC0 ) << 8;
      ctrl.accel.Y = ( chr & 0x30 ) << 10;
      ctrl.accel.X = ( chr & 0x0C ) << 12;

      // Need to set high order byte for Accelerometer
      ctrl.accel.X += wiringPiI2CReadReg8(fd, 0x02 );
      ctrl.accel.Y += wiringPiI2CReadReg8(fd, 0x03 );
      ctrl.accel.Z += wiringPiI2CReadReg8(fd, 0x04 );

      // Read Joystick X
      ctrl.jstik.X = wiringPiI2CReadReg8(fd, 0x00 );
      ctrl.jstik.Y = wiringPiI2CReadReg8(fd, 0x01 );

      cout << "REG ==> JX: " << showbase << hex << (int)ctrl.jstik.X;
      cout << " | JY: " << hex << (int)ctrl.jstik.Y;
      cout << " :: AX: " << hex << (int)ctrl.accel.X;
      cout << " :: AY: " << hex << (int)ctrl.accel.Y;
      cout << " :: AZ: " << hex << (int)ctrl.accel.Z << endl;
      usleep( MILISECONDS * 100 );

      if(result == -1)
      {
         cout << "Error.  Errno is: " << errno << endl;
      }
   }

}
