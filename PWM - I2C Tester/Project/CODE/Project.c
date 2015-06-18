/** ###################################################################
**     Filename  : Project.c
**     Project   : Project
**     Processor : MC9S12C32MFA25
**     Version   : Driver 01.14
**     Compiler  : CodeWarrior HC12 C Compiler
**     Date/Time : 18.06.2015, 19:31
**     Abstract  :
**         Main module.
**         This module contains user's application code.
**     Settings  :
**     Contents  :
**         No public methods
**
** ###################################################################*/
/* MODULE Project */

/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "EI2C1.h"
#include "Inhr1.h"
#include "Inhr2.h"
#include "PWM6.h"
#include "PWM7.h"
#include "AD1.h"
/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* User includes (#include below this line is not maintained by Processor Expert) */
#include "nunchuk.h"
#include "pid.h"

uint8_t error_code = 0;
uint16_t ad1Values[8];

void main(void)
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  error_code = initJoystick();
  
  pid[0].pFactor = 100;
  pid[0].iFactor = 10;
  pid[0].dFactor = 40;
  pid[0].input.actualValue = 0;
  pid[0].input.setpoint = 0;

  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){
     updateJoystickInput();
     AD1_Measure(1);
     AD1_GetValue16(ad1Values);
//AD vem em 12 bits (0 - 3v3)
//a diferenca é um int16_t (-4095 <-> 4095)
//multiplica por 4 e temos um valor possivel int16_t (-32767 <-> 32767)     
     pid[0].input.actualValue = (int16_t)(((ad1Values[0]) - (ad1Values[1]))*4);     
     pid[0].input.setpoint = (int16_t)(joystick.axisX*300); 
     pid_cyclic();
     
     if( pid[0].output > 0 ){
        PWM6_SetRatio16( (pid[0].output << 1) & 0xFFFF );  
        PWM7_SetRatio16( 0 );
     }else if( pid[0].output < 0 ){
        PWM7_SetRatio16( (((uint16_t)(-pid[0].output)) << 1) & 0xFFFF );  
        PWM6_SetRatio16( 0 );
     }else{
        PWM6_SetRatio16( 0 );  
        PWM7_SetRatio16( 0 );
     }
     
     Cpu_Delay100US(1000);
  }
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END Project */
/*
** ###################################################################
**
**     This file was created by Processor Expert 3.02 [04.44]
**     for the Freescale HCS12 series of microcontrollers.
**
** ###################################################################
*/
