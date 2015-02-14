#ifndef __RACKET_H
#define __RACKET_H

#include "main.h"
#include "special_char_handler.h"

#define RACKET MOTOR5

#define RACKET_SWITCH_LINE 	EXTI_Line2
#define RACKET_IRQn 			 	EXTI2_IRQn


#define ROTATE_SWITCH_LINE 	EXTI_Line3
#define ROTATE_IRQn 			 	EXTI3_IRQn

#define RACKET_SWITCH_INTERRUPT_HANDLER void EXTI2_IRQHandler(void)
#define ROTATE_SWITCH_INTERRUPT_HANDLER void EXTI3_IRQHandler(void)	
	

//Controls
#define KEY_STOP_RACKET 'p'
#define KEY_LOCK_RACKET 'o'


void racket_update(void);
void racket_stop(void);
void racket_lock(void);
void racket_set_vel(s32 vel, CLOSE_LOOP_FLAG loop);
s32 racket_get_vel();

#endif
