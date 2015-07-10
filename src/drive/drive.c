/*The MIT License (MIT)

Copyright (c) 2015 Marcelo Chimentao, Leandro Piekarski do Nascimento

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

void drive_init(void)
{
	pwm_setSignal(E_PWM_MOTOR_STEERING, 0);
	pwm_setSignal(E_PWM_MOTOR_SUSPENSION, 0);
	pwm_setSignal(E_PWM_MOTOR_WHEEL, 0);
}

void drive_cyclic(void)
{
	const eId loc_id = id_getId();
	const uint16_t loc_sensorCurrent = sensor[E_SENSOR_CURRENT];
	const uint16_t loc_sensorSteering = sensor[E_SENSOR_STEERING];
	const uint16_t loc_sensorJoint = sensor[E_SENSOR_SUSPENSION_JOINT];
	const uint16_t loc_sensorSpring = sensor[E_SENSOR_SUSPENSION_SPRING];

	tMessageSteeringData steeringData;
	tMessageSuspensionData suspensionData;
	tMessageWheelData wheelData;
	int16_t loc_steering = 0;
	int16_t loc_suspension = 0;
	int16_t loc_wheel = 0;
	int16_t loc_pwmSteering = 0;
	int16_t loc_pwmSuspension = 0;
	int16_t loc_pwmWheel = 0;

	comm_getData(E_MSG_STEERING_ID, &steeringData);
	comm_getData(E_MSG_SUSP_ID, &suspensionData);
	comm_getData(E_MSG_WHEEL_ID, &wheelData);

	if ((loc_id >= E_ID_L1) && (loc_id <= E_ID_L2))
	{
		loc_steering = steeringData.steering[(uint8_t)loc_id];
		loc_suspension = suspensionData.suspension[(uint8_t)loc_id];
		loc_wheel = wheelData.wheel[(uint8_t)loc_id];

		pid[E_PID_MOTOR_STEERING].input.setpoint = loc_steering;
		pid[E_PID_MOTOR_SUSPENSION].input.setpoint = loc_suspension;
		pid[E_PID_MOTOR_WHEEL].input.setpoint = loc_wheel;

		pid[E_PID_MOTOR_STEERING].input.actualValue = (int16_t)loc_sensorSteering;
		pid[E_PID_MOTOR_SUSPENSION].input.actualValue = (int16_t)loc_sensorSpring;
		pid[E_PID_MOTOR_WHEEL].input.actualValue = (int16_t)loc_sensorCurrent;

		loc_pwmSteering = pid[E_PID_MOTOR_STEERING].output;
		loc_pwmSuspension = pid[E_PID_MOTOR_SUSPENSION].output;
		loc_pwmWheel = pid[E_PID_MOTOR_WHEEL].output;

		pwm_setSignal(E_PWM_MOTOR_STEERING, loc_pwmSteering);
		pwm_setSignal(E_PWM_MOTOR_SUSPENSION, loc_pwmSuspension);
		pwm_setSignal(E_PWM_MOTOR_WHEEL, loc_pwmWheel);
	}
}
