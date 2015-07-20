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
#include "AD1.h"
#include "AS1.h"
#include "PWM_SUSPENSION_UP.h"
#include "PWM_SUSPENSION_DOWN.h"
#include "PWM_STEERING_LEFT.h"
#include "PWM_STEERING_RIGHT.h"
#include "PWM_WHEEL_FORWARDS.h"
#include "PWM_WHEEL_BACKWARDS.h"
#include "BOARD_ID.h"
#include "LED.h"
/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* User includes (#include below this line is not maintained by Processor Expert) */
#include "program.h"
#include "message.h"
#include "CAN1.h"
#include "sensor_interface.h"
#include "AS1.h"
#include "comm.h"
#include "uart_interface.h"

#define CURRENT_CONVERSION 2       
  
  CAN1_TError err;       

void main(void)
{
  /* Write your local variable definition here */    

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  program_init();
  
  for(;;){        
      program_cyclic();      
      //comm_cyclic();
      //ctrlData.joystickData.joystickX = nunchuk.joystickX;
      //ctrlData.joystickData.joystickY = nunchuk.joystickY;
      //comm_setData(E_MSG_ID_CONTROL, &ctrlData, sizeof(tMessageControlData));
      //uart_writeToBuffer(&msgControl.body.data.rawData, 10);
      //comm_cyclic();
      //CAN1_GetError(&err);
      //error_code = AS1_SendChar(can_data[0]);
      
      //AD1_Measure(1);
      //AD1_GetChanValue16(E_SENSOR_STEERING, &hue);
      
      //error_code = AS1_RecvChar(&ler_rec);
      
      //handshake_cyclic();      
      //comm_cyclic();
                               
	    //ctrlData.pwmTest[0] = 0x0000;
	    //comm_setData(E_MSG_W1_ID, &ctrlData);
      //comm_getData(E_MSG_CONTROL_ID, &ctrlData);
      //ctrlData.pwmTest[0] = 0x7FFF;
      //PWM_STEERING_RIGHT_SetRatio16(~(ctrlData.pwmTest[0]));
      
      //nunchuk_cyclic();
      #if NODE==CONTROL
      Cpu_Delay100US(5);  
      #else
      Cpu_Delay100US(100);        
      #endif
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
