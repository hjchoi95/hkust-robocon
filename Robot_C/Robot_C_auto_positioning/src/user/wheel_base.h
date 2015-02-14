#ifndef __WHEEL_BASE_H
#define __WHEEL_BASE_H

#include <stdbool.h>
#include "stm32f10x.h"
#include "can_motor.h"
#include "bluetooth.h"
#include "ticks.h"
#include "gyro.h"
#include "tft.h"

// Protocol
#define	BLUETOOTH_WHEEL_BASE_VEL_ID	    			0x40    // RX
#define	BLUETOOTH_WHEEL_BASE_SPEED_MODE_ID		0x41    // RX
#define	BLUETOOTH_WHEEL_BASE_AUTO_POS_ID			0x50    // RX
#define	BLUETOOTH_WHEEL_BASE_AUTO_START_ID		0x51    // RX
#define	BLUETOOTH_WHEEL_BASE_AUTO_STOP_ID			0x52    // RX
#define BLUETOOTH_WHEEL_BASE_POS_ID						0x60    // TX
#define BLUETOOTH_WHEEL_BASE_CHAR_ID           0x70    // RX


#define	BLUETOOTH_WHEEL_BASE_TIMEOUT					100

#define WHEEL_BASE_REGULAR_CAN_TX							2000

// Wheel-base speed related
#define	WHEEL_BASE_XY_VEL_RATIO								707		//  70.7%
#define	WHEEL_BASE_W_VEL_RATIO								-800		//	-80.0%
static const u16 SPEED_MODES[10] =	// In percentage (20 = 20%)
{
	0, 10, 20, 30, 40, 50, 60, 70, 80, 90
};

// Initialized value
#define WHEEL_BASE_DEFAULT_ACC								200
#define	WHEEL_BASE_DEFAULT_SPEED_MODE					3			// from 0 to 9

// Wheel base motors acceleration (CONSTANT, to be configured upon startup)
#define	WHEEL_BASE_BR_ACC											600		// Bottom-right wheel
#define	WHEEL_BASE_BL_ACC										  600		// Bottom-left wheel
#define	WHEEL_BASE_TL_ACC											600		// Top-left wheel
#define	WHEEL_BASE_TR_ACC											600 		// Top-right wheel

typedef struct {
	s32 x;
	s32 y;
	s32 w;		// Angular velocity
} WHEEL_BASE_VEL;

static const MOTOR_ID
	MOTOR_BOTTOM_RIGHT 		= MOTOR3,		//changed from MOTOR1
	MOTOR_BOTTOM_LEFT 		= MOTOR4,		//changed from MOTOR2
	MOTOR_TOP_LEFT 				= MOTOR1,		//changed from MOTOR3
	MOTOR_TOP_RIGHT 			= MOTOR2;		//changed from MOTOR4


typedef	struct {
	s32 Kp, Ki, Kd;
} PID;

void disable_auto_positioning(void);

void wheel_base_init(void);
void wheel_base_set_speed_mode(u8 s);
u8 wheel_base_get_speed_mode(void);
void wheel_base_tx_acc(void);
void wheel_base_set_vel(s32 x, s32 y, s32 w);
WHEEL_BASE_VEL wheel_base_get_vel(void);
char wheel_base_bluetooth_get_last_char(void);
void wheel_base_update(void);
void wheel_base_positioning_auto(void);
void wheel_base_tx_position(void);

bool bluetooth_is_key_release(void);
void wheel_base_set_pid(PID pid);
POSITION wheel_base_get_target_pos(void);
void wheel_base_set_target_pos(POSITION pos);
void wheel_base_pid_on(void);
void wheel_base_pid_off(void);
u8 wheel_base_get_pid_flag(void);
void wheel_base_joyStickCommandFlag_on(void);
void wheel_base_increase_joystick_speed(void);
void wheel_base_decrease_joystick_speed(void);
u8 wheel_base_get_joystick_speed(void);

#endif	/* __WHEEL_BASE_H */
