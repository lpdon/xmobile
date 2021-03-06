/*The MIT License (MIT)

Copyright (c) 2015 Marcelo Chimentao, Leandro Piekarski do Nascimento, Matthias Leiter

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#ifndef DRIVE_H
#include "drive.h"
#endif

#ifndef PID_H
#include "../pid/pid.h"
#endif

#ifndef PWM_H
#include "../pwm/pwm.h"
#endif

#ifndef COMM_H
#include "../comm/comm.h"
#endif

#ifndef ID_H
#include "../id/id.h"
#endif

#ifndef SENSOR_INTERFACE_H
#include "../sensor_interface/sensor_interface.h"
#endif

#ifndef OPMODE_H
#include "../opmode/opmode.h"
#endif

#ifndef UTILS_H
#include "../utils/utils.h"
#endif

static const uint16_t VEHICLE_LENGTH         = 50U;
static const uint16_t VEHICLE_WIDTH          = 30U;

static const uint16_t STEERING_CENTER         = 512U; //2^9 - 1
static const uint8_t STEERING_RANGE          = 15U; //deg

static const uint8_t WHEEL_RANGE             = 255U;
static const int8_t  WHEEL_DEADZONE          = 5;
static const int8_t  WHEEL_MAX               = 70;
static const int8_t  WHEEL_MAX_KINDER        = 30;

static const uint16_t STEERING_S1_P             = 150;
static const uint16_t STERRING_S1_I             = 5;
static const uint16_t STEERING_S1_D             = 0;

static const uint16_t STEERING_S2_P             = 150;
static const uint16_t STERRING_S2_I             = 5;
static const uint16_t STEERING_S2_D             = 0;

static const uint16_t STEERING_S3_P             = 150;
static const uint16_t STERRING_S3_I             = 3;
static const uint16_t STEERING_S3_D             = 0;

static const uint16_t STEERING_S4_P             = 150;
static const uint16_t STERRING_S4_I             = 5;
static const uint16_t STEERING_S4_D             = 0; 



static void drive_master(void);
static void drive_slave(const eId arg_id);
static tMessageSteeringData drive_getSteeringData(const int8_t arg_speed, const int16_t arg_angle);
static int16_t calc_lw(int32_t l, int32_t x_p, int32_t x_i, int32_t y_p, int32_t y_i);
static int8_t drive_changeMode(uint8_t buttons);

void drive_init(void)
{
	pwm_setSignal(E_PWM_MOTOR_STEERING, 0);
	pwm_setSignal(E_PWM_MOTOR_SUSPENSION, 0);
	pwm_setSignal(E_PWM_MOTOR_WHEEL, 0);
}

void drive_cyclic(void)
{
	const eId loc_id = id_getId();

	switch (loc_id)
	{
		case E_ID_MASTER:
		{
			drive_master();
			break;
		}
		case E_ID_S1:
		case E_ID_S2:
		case E_ID_S3:
		case E_ID_S4:
		{
			drive_slave(loc_id);
			break;
		}
		default:
		{
			break;
		}
	}
}

void drive_master(void)
{
	tMessageControlData loc_controlData;
	tMessageSteeringData loc_steeringData;
	tMessageWheelData loc_wheelData;
	
	int8_t loc_x;
	int8_t loc_y;
	uint8_t loc_buttons;
	int16_t loc_requiredSteering;
	int8_t loc_wheel;
	volatile int8_t wheel_max;

	/* Read and decode data sent by control */
	comm_getData(E_MSG_ID_CONTROL, &loc_controlData, sizeof(tMessageControlData));
	loc_x = loc_controlData.joystickData.joystickX;
	loc_y = loc_controlData.joystickData.joystickY;
	loc_buttons = loc_controlData.joystickData.buttons; 

	loc_wheel = ((loc_y > WHEEL_DEADZONE) || (loc_y < -WHEEL_DEADZONE)) ? loc_y : 0;
	
	//proportional to maximum speed        
	wheel_max = drive_changeMode(loc_buttons);
	loc_wheel = (int8_t)((loc_wheel*wheel_max)/100);

	loc_wheelData.wheel[E_ID_S1] = loc_wheel;
	loc_wheelData.wheel[E_ID_S2] = loc_wheel;
	loc_wheelData.wheel[E_ID_S3] = loc_wheel;
	loc_wheelData.wheel[E_ID_S4] = loc_wheel;

	loc_requiredSteering = ((loc_x * STEERING_RANGE) / 10) ; //dDeg
	
	/* TODO
	loc_parameterData.pFactor[E_ID_S1] = STEERING_S1_P / 10;
	loc_parameterData.pFactor[E_ID_S2] = STEERING_S2_P / 10;
	loc_parameterData.pFactor[E_ID_S3] = STEERING_S3_P / 10;
	loc_parameterData.pFactor[E_ID_S4] = STEERING_S4_P / 10;
	
	loc_parameterData.iFactor[E_ID_S1] = STERRING_S1_I;
	loc_parameterData.iFactor[E_ID_S2] = STERRING_S2_I;
	loc_parameterData.iFactor[E_ID_S3] = STERRING_S3_I;
	loc_parameterData.iFactor[E_ID_S4] = STERRING_S4_I;
	*/
	
	switch (opmode_getActiveMode())
	{
		case E_OPMODE_NORMAL:
		{
			loc_wheelData.wheel[E_ID_S1] = loc_wheel;
			loc_wheelData.wheel[E_ID_S2] = loc_wheel;
			loc_wheelData.wheel[E_ID_S3] = loc_wheel;
			loc_wheelData.wheel[E_ID_S4] = loc_wheel;
			loc_steeringData = drive_getSteeringData(loc_wheel, loc_requiredSteering);
			break;
		}
		case E_OPMODE_SHUTDOWN:
		default:
		{
			loc_wheelData.wheel[E_ID_S1] = 0;
			loc_wheelData.wheel[E_ID_S2] = 0;
			loc_wheelData.wheel[E_ID_S3] = 0;
			loc_wheelData.wheel[E_ID_S4] = 0;
			loc_steeringData.steering[E_ID_S1] = 0;
			loc_steeringData.steering[E_ID_S2] = 0;
			loc_steeringData.steering[E_ID_S3] = 0;
			loc_steeringData.steering[E_ID_S4] = 0;
			break;
		}
	}

	comm_setData(E_MSG_ID_WHEEL, &loc_wheelData, sizeof(tMessageWheelData));
	comm_setData(E_MSG_ID_STEERING, &loc_steeringData, sizeof(tMessageSteeringData));
}

void drive_slave(const eId arg_id)
{
	const eId loc_id = arg_id;
	const uint16_t loc_sensorCurrent = sensor[E_SENSOR_CURRENT];
	const uint16_t loc_sensorSteering = sensor[E_SENSOR_STEERING];
	const uint16_t loc_sensorJoint = sensor[E_SENSOR_SUSPENSION_JOINT];
	const uint16_t loc_sensorSpring = sensor[E_SENSOR_SUSPENSION_SPRING];

#if NODE==SLAVE1
	const uint16_t STEERING_P             = STEERING_S1_P;
	const uint16_t STERRING_I             = STERRING_S1_I;
	const uint16_t STEERING_D             = STEERING_S1_D;
#elif NODE==SLAVE2
	const uint16_t STEERING_P             = STEERING_S2_P;
	const uint16_t STERRING_I             = STERRING_S2_I;
	const uint16_t STEERING_D             = STEERING_S2_D;
#elif NODE==SLAVE3
	const uint16_t STEERING_P             = STEERING_S3_P;
	const uint16_t STERRING_I             = STERRING_S3_I;
	const uint16_t STEERING_D             = STEERING_S3_D;
#elif NODE==SLAVE4
	const uint16_t STEERING_P             = STEERING_S4_P;
	const uint16_t STERRING_I             = STERRING_S4_I;
	const uint16_t STEERING_D             = STEERING_S4_D;
#endif

	tMessageSteeringData loc_steeringData;
	tMessageSuspensionData loc_suspensionData;
	tMessageWheelData loc_wheelData;
	tMessageParameterData loc_parameterData;
	
	int16_t loc_steering = 0;
	int16_t loc_suspension = 0;
	int16_t loc_wheel = 0;
	int16_t loc_pFactor = 0;
	int16_t loc_iFactor = 0;
	int16_t loc_dFactor = 0;
	int32_t loc_pwmSteering = 0;
	int16_t loc_pwmSuspension = 0;
	int16_t loc_pwmWheel = 0;         	

	comm_getData(E_MSG_ID_STEERING, &loc_steeringData, sizeof(tMessageSteeringData));
	comm_getData(E_MSG_ID_SUSP, &loc_suspensionData, sizeof(tMessageSuspensionData));
	comm_getData(E_MSG_ID_WHEEL, &loc_wheelData, sizeof(tMessageWheelData));

	loc_steering = loc_steeringData.steering[(uint8_t)loc_id];
	loc_suspension = loc_suspensionData.suspension[(uint8_t)loc_id];
	loc_wheel = loc_wheelData.wheel[(uint8_t)loc_id];	  

	loc_pFactor = STEERING_P;
	loc_iFactor = STERRING_I;
	loc_dFactor = STEERING_D;

	pid[E_PID_MOTOR_STEERING].pFactor = loc_pFactor;
	pid[E_PID_MOTOR_STEERING].iFactor = loc_iFactor;
	pid[E_PID_MOTOR_STEERING].dFactor = loc_dFactor;

	pid[E_PID_MOTOR_STEERING].input.setpoint = loc_steering * 4;
	pid[E_PID_MOTOR_SUSPENSION].input.setpoint = loc_suspension;
	//	pid[E_PID_MOTOR_WHEEL].input.setpoint = loc_wheel;

	pid[E_PID_MOTOR_STEERING].input.actualValue = (int16_t)(((loc_sensorSteering - STEERING_CENTER) * 10)); //dDeg
	pid[E_PID_MOTOR_SUSPENSION].input.actualValue = (int16_t)loc_sensorSpring;
	//	pid[E_PID_MOTOR_WHEEL].input.actualValue = (int16_t)loc_sensorCurrent;

	loc_pwmSteering = pid[E_PID_MOTOR_STEERING].output;
	loc_pwmSuspension = pid[E_PID_MOTOR_SUSPENSION].output;
	//	loc_pwmWheel = pid[E_PID_MOTOR_WHEEL].output;
	loc_pwmWheel = loc_wheel * 2;

	pwm_setSignal(E_PWM_MOTOR_STEERING, loc_pwmSteering);
	pwm_setSignal(E_PWM_MOTOR_SUSPENSION, loc_pwmSuspension);
	pwm_setSignal(E_PWM_MOTOR_WHEEL, loc_pwmWheel);
}

tMessageSteeringData drive_getSteeringData(const int8_t arg_speed, const int16_t arg_angle)
{
	tMessageSteeringData loc_steeringData;

	volatile int32_t y_p;           // Torque pole Y-Direction
	volatile int32_t x_p;           // Torque pole X-Direciton

	volatile int32_t x_i;           // Position wheel
	volatile int32_t y_i;           // Position wheel

	int32_t lw;            // vehicle steering angle

	int32_t v;             // Vehicle speed

	int32_t v_max;         // Maximum speed
	int32_t v_min;         // Minimum speed

	int32_t l;             // vehicle length
	int32_t b;             // vehicle width

	v = arg_speed;             // get vehicle speed in temp variable
	v_max = (WHEEL_MAX * 3) / 4; // v_max for the steering mode selection is 75% of the max allowed speed
	v_min = WHEEL_MAX / 4;
	l = VEHICLE_LENGTH * 10;
	b = VEHICLE_WIDTH * 10;
	lw = (int32_t)arg_angle * 3142 / 1800 ;     // steering angle mrad

	if(v<0)
	{
		v = -v;
	}

	if(lw > 5 || lw < -5)              // lw != 0
	{
		// calculate Torque pole
		// x-Axis
		if(v > v_min && v < v_max)
		{
			y_p = (int32_t) ((l * (1000 - (v-v_min)*1000 / (v_max-v_min))) / 2000); // Offset for x-Axis
		}

		if(v <= v_min)
		{
			y_p = (int32_t) l/2;                     // Torque pole in vehicle center
		}

		if(v >= v_max)
		{
			y_p = (int32_t) 0;                       // Torque pole at rear axis
		}

		// x-Axis
		x_p = ((l-y_p)*1000 / utils_tan(lw));

		// Calculate steering angle
		if (v <= v_min)                  // steer front back reverse
		{
			// calculate front angle VL
			y_i = l;
			x_i = -b/2;
			loc_steeringData.steering[E_ID_S1] = calc_lw(l,x_p,x_i,y_p,y_i);

			// calculate front angle VR
			y_i = l;
			x_i = b/2;
			loc_steeringData.steering[E_ID_S2] = calc_lw(l,x_p,x_i,y_p,y_i);

			// calculate rear angle HL
			y_i = 0;
			x_i = -b/2;
			loc_steeringData.steering[E_ID_S3] = calc_lw(l,x_p,x_i,y_p,y_i);

			// calculate rear angle HR
			y_i = 0;
			x_i = b/2;
			loc_steeringData.steering[E_ID_S4] = calc_lw(l,x_p,x_i,y_p,y_i);
		}
		if (v > v_min && v < v_max)      // steer together
		{

			// calculate front angle VL
			y_i = l;
			x_i = -b/2;
			loc_steeringData.steering[E_ID_S1] = calc_lw(l,x_p,x_i,y_p,y_i);

			// calculate front angle VR
			y_i = l;
			x_i = b/2;
			loc_steeringData.steering[E_ID_S2] = calc_lw(l,x_p,x_i,y_p,y_i);

			// calculate rear angle HL
			y_i = 0;
			x_i = -b/2;
			loc_steeringData.steering[E_ID_S3] = calc_lw(l,x_p,x_i,y_p,y_i);

			// calculate rear angle HR
			y_i = 0;
			x_i = b/2;
			loc_steeringData.steering[E_ID_S4] = calc_lw(l,x_p,x_i,y_p,y_i);

		}


		if(v >= v_max)                  // steer only front axis
		{

			// calculate front angle VL
			y_i = l;
			x_i = -b/2;
			loc_steeringData.steering[E_ID_S1] = calc_lw(l,x_p,x_i,y_p,y_i);

			// calculate front angle VR
			y_i = l;
			x_i = b/2;
			loc_steeringData.steering[E_ID_S2] = calc_lw(l,x_p,x_i,y_p,y_i);

			loc_steeringData.steering[E_ID_S3] = 0;
			loc_steeringData.steering[E_ID_S4] = 0;
		}
	}
	else
	{
		//		vehiclesetpoint.steer_angle_vl = 0;     // if commanded steer angle is belwo 5dDeg set angle to 0
		loc_steeringData.steering[E_ID_S1] = 0;
		loc_steeringData.steering[E_ID_S2] = 0;
		loc_steeringData.steering[E_ID_S3] = 0;
		loc_steeringData.steering[E_ID_S4] = 0;
	}

	return loc_steeringData;
}

//                    [mm]          [mm]         [mm]           [mm]          [mm]
int16_t calc_lw(int32_t l, int32_t x_p, int32_t x_i, int32_t y_p, int32_t y_i)
{
	int32_t lw=0;
	int32_t x;

	x = (y_i*1000 - y_p*1000) / (x_p - x_i); // calculate argument

	//  approximation tan(lw)
	lw = utils_atan(x);

	return((int16_t) (((int32_t)lw*1800/3142)));                               // Return lw in dDeg
}

int8_t drive_changeMode(uint8_t buttons) 
{
  static uint8_t sequence = 0U;
  static uint8_t mode = 0U;
  static uint8_t is_pressed = 0U;
  uint8_t bit = 0U;
  
  if( !is_pressed && (buttons == 1 || buttons == 2)){
    is_pressed = 1;
    
    bit = (buttons == 1) ? 0 : 1;
    sequence = (sequence << 1) | (bit & 0x01);
    
  }else if( buttons == 0 ){
    is_pressed = 0;
  }
  
  if( sequence == 0b11110000 ){
    mode = 0;
#if !defined(WIN32)
    PWM_WHEEL_FORWARDS_SetRatio8(0x00);
#endif
    sequence = 0;
  }
  if( sequence == 0b10101010 ){
    mode = 1;   
#if !defined(WIN32)
    PWM_WHEEL_FORWARDS_SetRatio8(0xbf);
#endif
    sequence = 0;
  }
  
  if (mode != 1) mode = 0; 
  return (mode==1)?WHEEL_MAX:WHEEL_MAX_KINDER;
}
