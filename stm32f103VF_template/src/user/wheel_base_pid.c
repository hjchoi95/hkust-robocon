#include "wheel_base_pid.h"
#include "special_char_handler.h"
#include "stdbool.h"

#define STEADY_STATE_SPEED_THRESHOLD 70
#define STEADY_STATE_TIME_THRESHOLD 150

#define ERROR_THRESHOLD 800
#define INTEGRAL_THRESHOLD 1000
#define MOTOR_MAX_SPEED 1000

static int C_PR = 124;
static int C_IN = 1;
static int C_DE = 1;

typedef struct {
	s32 current_error, p_error, i_error, d_error;
} PID_OUTPUT;

static POSITION end_pos = {0, 0, 0};

static PID_OUTPUT x_coord_pid = {0, 0, 0, 0}, 
									y_coord_pid = {0, 0, 0, 0}, 
									t_coord_pid = {0, 0, 0, 0},
									// for resetting PID to zero
									reset = {0, 0, 0, 0};

static s32 x_speed = 0, y_speed = 0, t_speed = 0;

static s32 steady_state_timer = 0;

static bool pid_locked = false;

static u8 der_count_up = 0;
static u8 der_count_down = 0;
static u8 int_count_up = 0;
static u8 int_count_down = 0;
static u8 pro_count_up = 0;
static u8 pro_count_down = 0;

static void decrease_der(void)
{
	if (der_count_down == 9) {
		--C_DE;
		der_count_down = 0;
	} else {
		++der_count_down;
	}
}

static void increase_der(void)
{
	if (der_count_up == 9) {
		++C_DE;
		der_count_up = 0;
	} else {
		++der_count_up;
	}
}
									
static void decrease_int(void)
{
	if (int_count_down == 9) {
		--C_IN;
		int_count_down = 0;
	} else {
		++int_count_down;
	}
}

static void increase_int(void)
{
	if (int_count_up == 9) {
		++C_IN;
		int_count_up = 0;
	} else {
		++int_count_up;
	}
}
									
static void decrease_prop(void)
{
	if (pro_count_down == 9) {
		--C_PR;
		pro_count_down = 0;
	} else {
		++pro_count_down;
	}
}

static void increase_prop(void)
{
	if (pro_count_up == 9) {
		++C_PR;
		pro_count_up = 0;
	} else {
		++pro_count_up;
	}
}

static void reset_gyro(void)
{
	gyro_pos_set(0, 0, 0);
}

void wheel_base_pid_init(void)
{
	register_special_char_function('r', decrease_prop);
	register_special_char_function('t', increase_prop);
	register_special_char_function('f', decrease_int);
	register_special_char_function('g', increase_int);
	register_special_char_function('x', decrease_der);
	register_special_char_function('v', increase_der);
	register_special_char_function(' ', reset_gyro);
}

u8 get_pid_stat(void)
{
	return pid_locked;
}

int get_x_speed(void)
{
	return x_speed;
}

int get_y_speed(void)
{
	return y_speed;
}

int get_t_speed(void)
{
	return t_speed;
}

int get_prop(void)
{
	return C_PR;
}

int get_int(void)
{
	return C_IN;
}

int get_der(void)
{
	return C_DE;
}

void update_pid(s32 current_error, PID_OUTPUT* input_pid)
{
	// Calculate proportional
	input_pid->p_error = current_error;
	
	// Calculate integral
	input_pid->i_error += current_error / 10;
	
	// prevent integral from growing too large
	if (input_pid->i_error > INTEGRAL_THRESHOLD) {
		input_pid->i_error = INTEGRAL_THRESHOLD;
	}
	
	// Calculate derivative
	input_pid->d_error = current_error - input_pid->current_error;
	
	// Update error stored in PID
	input_pid->current_error = current_error;
}

// Reset all error calculations
void reset_pid(void)
{
	x_coord_pid = reset;
	y_coord_pid = reset;
	t_coord_pid = reset;
	pid_locked = false;
}

void wheel_base_pid_update(void)
{
  /** TODO: Code the auto PID **/
  /** Use wheel_base_set_vel(x,y,w) to control wheel base motors */
	
	// PID mode check
	if (wheel_base_get_pid_flag() == 1) {
		
		s32 x_error = end_pos.x - get_pos()->x, y_error = end_pos.y - get_pos()->y, t_error = end_pos.angle - get_pos()->angle;
		
		if (t_error > 1800) {
			t_error -= 3600;
		} else if (t_error < -1800) {
			t_error += 3600;
		}
		
		s32 x_max_speed, y_max_speed;
		if (abs(x_error) > abs(y_error))
		{
			x_max_speed = MOTOR_MAX_SPEED;
			y_max_speed = MOTOR_MAX_SPEED * abs(y_error) / abs(x_error);
		}
		else
		{
			x_max_speed = MOTOR_MAX_SPEED * abs(x_error) / abs(y_error);
			y_max_speed = MOTOR_MAX_SPEED;
		}
		
		// update position variables and reset PID if target position is changed
		if (end_pos.x != wheel_base_get_target_pos().x || end_pos.y != wheel_base_get_target_pos().y || end_pos.angle != wheel_base_get_target_pos().angle) {
			end_pos = wheel_base_get_target_pos();
			reset_pid();
		}
		
		// x coordinate PID update
		if (x_error <= ERROR_THRESHOLD && x_error >= -ERROR_THRESHOLD) {
			// Update PID
			update_pid(end_pos.x - get_pos()->x, &x_coord_pid);
			// Set speed according to PID
			x_speed = C_PR * x_coord_pid.p_error / 100 + C_IN * x_coord_pid.i_error / 100 + C_DE * x_coord_pid.d_error / 100;
		} else if (x_error > ERROR_THRESHOLD) {
			// Error too large, just set motor to positive max
			x_speed = x_max_speed;
		} else {
			// Error too large, just set motor to negative max
			x_speed = -x_max_speed;
		}
		
		// y coordinate PID update
		if (y_error <= ERROR_THRESHOLD && y_error >= -ERROR_THRESHOLD) {
			// Update PID
			update_pid(y_error, &y_coord_pid);
			// Set speed according to PID
			y_speed = C_PR * y_coord_pid.p_error / 100 + C_IN * y_coord_pid.i_error / 100 + C_DE * y_coord_pid.d_error / 100;
		} else if (y_error > ERROR_THRESHOLD) {
			// Error too large, just set motor to positive max
			y_speed = y_max_speed;
		} else {
			// Error too large, just set motor to negative max
			y_speed = -y_max_speed;
		}
		
		// angle PID update
		if (t_error <= ERROR_THRESHOLD && t_error >= -ERROR_THRESHOLD) {
			// Update PID
			update_pid(t_error, &t_coord_pid);
			// Set speed according to PID
			t_speed = C_PR * t_coord_pid.p_error / 100 + C_IN * t_coord_pid.i_error / 100 + C_DE * t_coord_pid.d_error / 100;
			t_speed = t_speed * 30 / 100;
		} else if (t_error > ERROR_THRESHOLD) {
			// Error too large, just set motor to positive max
			t_speed = MOTOR_MAX_SPEED;
		} else {
			// Error too large, just set motor to negative max
			t_speed = -MOTOR_MAX_SPEED;
		}
		
		// scale speeds using speed mode
		u8 speed_ratio = SPEED_MODES[wheel_base_get_speed_mode()];
		
		// disable spinning for now
		t_speed = 0;
		
		// update steady state timer if not in threshold
		if(x_speed > STEADY_STATE_SPEED_THRESHOLD || x_speed < -STEADY_STATE_SPEED_THRESHOLD ||
			 y_speed > STEADY_STATE_SPEED_THRESHOLD || y_speed < -STEADY_STATE_SPEED_THRESHOLD ||
			 t_speed > STEADY_STATE_SPEED_THRESHOLD || t_speed < -STEADY_STATE_SPEED_THRESHOLD)
		{
			steady_state_timer = get_full_ticks();
			pid_locked = false;
		}
		
		// when timer is expired, then we lock the motor
		if (get_full_ticks() - steady_state_timer > STEADY_STATE_TIME_THRESHOLD)
		{
			x_speed = 0;
			y_speed = 0;
			t_speed = 0;
			pid_locked = true;
		}

		// set velocity
		wheel_base_set_vel(x_speed * speed_ratio / 1000, y_speed * speed_ratio / 1000, t_speed * speed_ratio / 1000);
		
	}
}
