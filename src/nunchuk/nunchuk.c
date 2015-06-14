#ifndef NUNCHUK
	#include "nunchuk.h"
#endif

//don't forget to initialize the I2C before using this function
uint8_t initJoystick()
{
  uint8_t init_block[4] = {0xf0, 0x55, 0xfb, 0x00};
  uint8_t length;
  uint8_t error_code = 0;
  
  error_code = EI2C1_SendBlock(init_block, 2, &length);
  error_code = EI2C1_SendBlock(init_block+2, 2, &length);
  
  return error_code;
}

unit8_t updateJoystickInput()
{
  uint8_t received[6];
  uint8_t length;
  uint8_t error_code = 0;
  
  error_code = EI2C1_SendChar(0x00);
  error_code = EI2C1_RecvBlock(received,6,&length);
      
  if(length>5){
	nunchuk_status.joystickX = (received[0] - 0x1a)-100;
    nunchuk_status.joystickY = (received[1] - 0x1d)-100; 
    nunchuk_status.buttons = ((received[5] & 0x02) ? 0 : 1)<<1;
    nunchuk_status.buttons += (received[5] & 0x01) ? 0 : 1;    
  }
  
  return error_code;
}