/** ###################################################################
**     Filename  : Project_4.c
**     Project   : Project_4
**     Processor : MC9S12C32CFU16
**     Version   : Driver 01.14
**     Compiler  : CodeWarrior HC12 C Compiler
**     Date/Time : 08.05.2015, 08:42
**     Abstract  :
**         Main module.
**         This module contains user's application code.
**     Settings  :
**     Contents  :
**         No public methods
**
** ###################################################################*/
/* MODULE Project_4 */

/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "EI2C1.h"
#include "Inhr1.h"
#include "Inhr2.h"
#include "CAN1.h"
/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* User includes (#include below this line is not maintained by Processor Expert) */

void main(void)
{
  /* Write your local variable definition here */  
  unsigned char received[6];
  int joystickX, joystickY;
  bool buttonC, buttonZ;
  
  char init_block[4] = {0xf0, 0x55, 0xfb, 0x00};
  unsigned char can_data[8];
  unsigned int length;
  static char error_code = 0x00;

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  
  /*Init Sequence Black Nunchuk*/
  error_code = EI2C1_SendBlock(init_block, 2, &length);
  error_code = EI2C1_SendBlock(init_block+2, 2, &length);
  //if( error_code ) for(;;){} 
  
  for(;;){   
      error_code = EI2C1_SendChar(0x00);
      error_code = EI2C1_RecvBlock(received,6,&length);
      
      if(length>5){
        joystickX = (received[0] - 0x1a)-100;
        joystickY = (received[1] - 0x1d)-100; 
        buttonC = (received[5] & 0x02) ? 0 : 1;
        buttonZ = (received[5] & 0x01) ? 0 : 1;    
      }
      
      can_data[0] = joystickX & 0xff;
      can_data[1] = joystickY & 0xff;
      can_data[2] = buttonC & 0x01;
      can_data[3] = buttonZ & 0x01; 
      
      error_code = CAN1_SendFrameExt(0x01,DATA_FRAME,0x04,can_data);
      
      if (error_code != 0 && error_code != 11) for(;;){}
      
      Cpu_Delay100US(100);  
  }
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END Project_4 */
/*
** ###################################################################
**
**     This file was created by Processor Expert 3.02 [04.44]
**     for the Freescale HCS12 series of microcontrollers.
**
** ###################################################################
*/
