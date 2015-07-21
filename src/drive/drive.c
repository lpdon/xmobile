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

static const uint16_t VEHICLE_LENGTH         = 50U;
static const uint16_t VEHICLE_WIDTH          = 30U;

static const uint8_t STEERING_CENTER         = 127U;
static const uint8_t STEERING_RANGE          = 20U;

static const uint8_t WHEEL_RANGE             = 255U;
static const int8_t  WHEEL_DEADZONE          = 5;
static const int8_t  WHEEL_MAX               = 80;

static void drive_master(void);
static void drive_slave(const eId arg_id);
static tMessageSteeringData drive_getSteeringData(const int8_t arg_speed, const int8_t arg_angle);

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
	uint8_t loc_requiredSteering;
	int8_t loc_wheel;

	/* Read and decode data sent by control */
	comm_getData(E_MSG_ID_CONTROL, &loc_controlData, sizeof(tMessageControlData));
	loc_x = loc_controlData.joystickData.joystickX;
	loc_y = loc_controlData.joystickData.joystickY;

//	loc_steering = (uint8_t)((int8_t)STEERING_CENTER + (int8_t)((loc_x * STEERING_RANGE) / 100));
//	loc_steeringData.steering[E_ID_S1] = loc_steering;
//	loc_steeringData.steering[E_ID_S2] = loc_steering;
//	loc_steeringData.steering[E_ID_S3] = loc_steering;
//	loc_steeringData.steering[E_ID_S4] = loc_steering;

//	loc_wheel = (uint8_t)((int8_t)((loc_y * WHEEL_RANGE) / 100));
	loc_wheel = ((loc_y > WHEEL_DEADZONE) || (loc_y < WHEEL_DEADZONE)) ? loc_y : 0;

	if (loc_wheel < -WHEEL_MAX)
	{
		loc_wheel = -WHEEL_MAX;
	}
	else if (loc_wheel > WHEEL_MAX)
	{
		loc_wheel = WHEEL_MAX;
	}

	loc_wheelData.wheel[E_ID_S1] = loc_wheel;
	loc_wheelData.wheel[E_ID_S2] = loc_wheel;
	loc_wheelData.wheel[E_ID_S3] = loc_wheel;
	loc_wheelData.wheel[E_ID_S4] = loc_wheel;
	comm_setData(E_MSG_ID_WHEEL, &loc_wheelData, sizeof(tMessageWheelData));

	loc_requiredSteering = (int8_t)((loc_x * STEERING_RANGE) / 100);
	loc_steeringData = drive_getSteeringData(loc_wheel, loc_requiredSteering);
	comm_setData(E_MSG_ID_STEERING, &loc_steeringData, sizeof(tMessageSteeringData));
}

void drive_slave(const eId arg_id)
{
	const eId loc_id = arg_id;
	const uint16_t loc_sensorCurrent = sensor[E_SENSOR_CURRENT];
	const uint16_t loc_sensorSteering = sensor[E_SENSOR_STEERING];
	const uint16_t loc_sensorJoint = sensor[E_SENSOR_SUSPENSION_JOINT];
	const uint16_t loc_sensorSpring = sensor[E_SENSOR_SUSPENSION_SPRING];

	tMessageSteeringData loc_steeringData;
	tMessageSuspensionData loc_suspensionData;
	tMessageWheelData loc_wheelData;
	int16_t loc_steering = 0;
	int16_t loc_suspension = 0;
	int16_t loc_wheel = 0;
	int16_t loc_pwmSteering = 0;
	int16_t loc_pwmSuspension = 0;
	int16_t loc_pwmWheel = 0;

	comm_getData(E_MSG_ID_STEERING, &loc_steeringData, sizeof(tMessageSteeringData));
	comm_getData(E_MSG_ID_SUSP, &loc_suspensionData, sizeof(tMessageSuspensionData));
	comm_getData(E_MSG_ID_WHEEL, &loc_wheelData, sizeof(tMessageWheelData));

	loc_steering = loc_steeringData.steering[(uint8_t)loc_id];
	loc_suspension = loc_suspensionData.suspension[(uint8_t)loc_id];
	loc_wheel = loc_wheelData.wheel[(uint8_t)loc_id];

	pid[E_PID_MOTOR_STEERING].input.setpoint = loc_steering;
	pid[E_PID_MOTOR_SUSPENSION].input.setpoint = loc_suspension;
//	pid[E_PID_MOTOR_WHEEL].input.setpoint = loc_wheel;

	pid[E_PID_MOTOR_STEERING].input.actualValue = (int16_t)loc_sensorSteering;
	pid[E_PID_MOTOR_SUSPENSION].input.actualValue = (int16_t)loc_sensorSpring;
//	pid[E_PID_MOTOR_WHEEL].input.actualValue = (int16_t)loc_sensorCurrent;

	loc_pwmSteering = pid[E_PID_MOTOR_STEERING].output;
	loc_pwmSuspension = pid[E_PID_MOTOR_SUSPENSION].output;
//	loc_pwmWheel = pid[E_PID_MOTOR_WHEEL].output;
	loc_pwmWheel = loc_wheel * 2;

	pwm_setSignal(E_PWM_MOTOR_STEERING, loc_pwmSteering);
	pwm_setSignal(E_PWM_MOTOR_SUSPENSION, loc_pwmSuspension);
	pwm_setSignal(E_PWM_MOTOR_WHEEL, loc_pwmWheel);

	pid[E_PID_MOTOR_STEERING].pFactor = 60;
	pid[E_PID_MOTOR_STEERING].iFactor = 1;
	pid[E_PID_MOTOR_STEERING].dFactor = 3;
}

tMessageSteeringData drive_getSteeringData(const int8_t arg_speed, const int8_t arg_angle)
{
	tMessageSteeringData loc_steeringData;

	int32_t y_p;           // Torque pole Y-Direction
	int32_t x_p;           // Torque pole X-Direciton

	int32_t x_i;           // Position wheel
	int32_t y_i;           // Position wheel

	int32_t cos_lw;        // Cosinus steering angle
	int32_t lw;            // vehicle steering angle

	int32_t v;             // Vehicle speed

	int32_t v_max;         // Maximum speed
	int32_t v_min;         // Minimum speed

	int32_t l;             // vehicle length
	int32_t b;             // vehicle width

	int16_t angle;         // calculated angle (temp-variable).
	int16_t vzlw=1;        // sign requested steering angle.

	// Get current values out of vehicle-struct.

	v = arg_speed;             // get vehicle speed in temp variable
	v_max = (WHEEL_MAX * 3) >> 2; // v_max for the steering mode selection is 75% of the max allowed speed
	v_min = 0;
	l = VEHICLE_LENGTH;
	b = VEHICLE_WIDTH;
	lw = (int32_t)arg_angle * 10 * 3142 / 1800 ;     // steering angle mrad

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
			x_p = (int32_t) ((l * (1000 - (v-v_min)*1000 / (v_max-v_min))) / 2000); // Offset for x-Axis
		}

		if(v <= v_min)
		{
			x_p = (int32_t) l/2;                     // Torque pole in vehicle center
		}

		if(v >= v_max)
		{
			x_p = (int32_t) 0;                       // Torque pole at rear axis
		}

		// y-Axis
		y_p = ((l-x_p)*1000 / utils_tan(lw));

		// Calculate steering angle
		if (v <= v_min)                  // steer front back reverse
		{
			// calculate front angle VL
			x_i = l;
			y_i = b/2;
//			vehiclesetpoint.steer_angle_vl = calc_lw(l,x_p,x_i,y_p,y_i);
//			loc_steeringData.steering[E_ID_S1] = calc_lw(l,x_p,x_i,y_p,y_i);

			// calculate front angle VR
			x_i = l;
			y_i =-b/2;
//			vehiclesetpoint.steer_angle_vr = calc_lw(l,x_p,x_i,y_p,y_i);
//			loc_steeringData.steering[E_ID_S2] = calc_lw(l,x_p,x_i,y_p,y_i);

			// calculate rear angle HL
			x_i = 0;
			y_i = b/2;
//			vehiclesetpoint.steer_angle_hl = calc_lw(l,x_p,x_i,y_p,y_i);
//			loc_steeringData.steering[E_ID_S3] = calc_lw(l,x_p,x_i,y_p,y_i);

			// calculate rear angle HR
			x_i = 0;
			y_i =-b/2;
//			vehiclesetpoint.steer_angle_hr = calc_lw(l,x_p,x_i,y_p,y_i);
//			loc_steeringData.steering[E_ID_S4] = calc_lw(l,x_p,x_i,y_p,y_i);

		}
		if (v > v_min && v < v_max)      // steer together
		{

			// calculate front angle VL
			x_i = l;
			y_i = b/2;
//			vehiclesetpoint.steer_angle_vl = calc_lw(l,x_p,x_i,y_p,y_i);
//			loc_steeringData.steering[E_ID_S1] = calc_lw(l,x_p,x_i,y_p,y_i);

			// calculate front angle VL
			x_i = l;
			y_i =-b/2;
//			vehiclesetpoint.steer_angle_vr = calc_lw(l,x_p,x_i,y_p,y_i);
//			loc_steeringData.steering[E_ID_S2] = calc_lw(l,x_p,x_i,y_p,y_i);

			// calculate rear angle HL
			x_i = 0;
			y_i = b/2;
//			vehiclesetpoint.steer_angle_hl = calc_lw(l,x_p,x_i,y_p,y_i);
//			loc_steeringData.steering[E_ID_S3] = calc_lw(l,x_p,x_i,y_p,y_i);

			// calculate rear angle HR
			x_i = 0;
			y_i = b/2;
//			vehiclesetpoint.steer_angle_hr = calc_lw(l,x_p,x_i,y_p,y_i);
//			loc_steeringData.steering[E_ID_S4] = calc_lw(l,x_p,x_i,y_p,y_i);

		}


		if(v >= v_max)                  // steer only front axis
		{

			// calculate front angle VL
			x_i = l;
			y_i = b/2;
//			vehiclesetpoint.steer_angle_vl = calc_lw(l,x_p,x_i,y_p,y_i);
//			loc_steeringData.steering[E_ID_S1] = calc_lw(l,x_p,x_i,y_p,y_i);

			// calculate front angle VR
			x_i = l;
			y_i =-b/2;
//			vehiclesetpoint.steer_angle_vr = calc_lw(l,x_p,x_i,y_p,y_i);
//			loc_steeringData.steering[E_ID_S2] = calc_lw(l,x_p,x_i,y_p,y_i);

//			vehiclesetpoint.steer_angle_hl = 0;
//			loc_steeringData.steering[E_ID_S3] = 0;

//			vehiclesetpoint.steer_angle_hr = 0;
//			loc_steeringData.steering[E_ID_S4] = 0;
		}
	}
	else
	{
//		vehiclesetpoint.steer_angle_vl = 0;     // if commanded steer angle is belwo 5dDeg set angle to 0
//		vehiclesetpoint.steer_angle_vr = 0;
//		vehiclesetpoint.steer_angle_hl = 0;
//		vehiclesetpoint.steer_angle_hr = 0;
		loc_steeringData.steering[E_ID_S1] = 0;
		loc_steeringData.steering[E_ID_S2] = 0;
		loc_steeringData.steering[E_ID_S3] = 0;
		loc_steeringData.steering[E_ID_S4] = 0;
	}

	return loc_steeringData;
}
