#include "buzzer_song.h"

const MUSIC_NOTE SPEED_0[] = {{NOTE_G, 5}, {NOTE_END}};
const MUSIC_NOTE SPEED_1[] = {{NOTE_A, 6}, {NOTE_END}};
const MUSIC_NOTE SPEED_2[] = {{NOTE_B, 6}, {NOTE_END}};
const MUSIC_NOTE SPEED_3[] = {{NOTE_C, 6}, {NOTE_END}};	// Do
const MUSIC_NOTE SPEED_4[] = {{NOTE_D, 6}, {NOTE_END}};	// Re
const MUSIC_NOTE SPEED_5[] = {{NOTE_E, 6}, {NOTE_END}};	// Mi
const MUSIC_NOTE SPEED_6[] = {{NOTE_F, 6}, {NOTE_END}};	// Fa
const MUSIC_NOTE SPEED_7[] = {{NOTE_G, 6}, {NOTE_END}};	// So
const MUSIC_NOTE SPEED_8[] = {{NOTE_A, 7}, {NOTE_END}};	// La
const MUSIC_NOTE SPEED_9[] = {{NOTE_B, 7}, {NOTE_END}};	// Ti
const MUSIC_NOTE SPEED_10[] = {{NOTE_C, 7}, {NOTE_END}}; // Do

const MUSIC_NOTE START_UP[] = {{NOTE_G,6},{NOTE_B,6},{NOTE_D,7},{NOTE_G,7},{NOTE_G,7},{NOTE_END}};
const MUSIC_NOTE SUCCESSFUL_SOUND[] = {{NOTE_C,7}, {NOTE_D,7}, {NOTE_E,7}, {NOTE_END}};
const MUSIC_NOTE FAIL_SOUND[] = {{NOTE_C, 4}, {NOTE_C, 4}, {NOTE_END}};
const MUSIC_NOTE CLICK[] = {{NOTE_G, 7}, {NOTE_END}};

const MUSIC_NOTE SIDE_CONTROL_ON_SOUND[] = {{NOTE_G, 6}, {NOTE_A, 7}, {NOTE_B, 7}, {NOTE_C, 7}, {NOTE_END}};
const MUSIC_NOTE SIDE_CONTROL_OFF_SOUND[] = {{NOTE_F, 6}, {NOTE_E, 6}, {NOTE_D, 6}, {NOTE_C, 6}, {NOTE_END}};

const MUSIC_NOTE BIRTHDAY_SONG[] = {
	{NOTE_D,6},{NOTE_REST},{NOTE_D,6},
	{NOTE_E,6},{NOTE_E,6},{NOTE_E,6},
	{NOTE_D,6},{NOTE_D,6},{NOTE_D,6},
	{NOTE_G,6},{NOTE_G,6},{NOTE_G,6},
	{NOTE_Fs,6},{NOTE_Fs,6},{NOTE_Fs,6},
	{NOTE_Fs,6},{NOTE_Fs,6},{NOTE_REST},
	
	{NOTE_D,6},{NOTE_REST},{NOTE_D,6},
	{NOTE_E,6},{NOTE_E,6},{NOTE_E,6},
	{NOTE_D,6},{NOTE_D,6},{NOTE_D,6},
	{NOTE_A,6},{NOTE_A,6},{NOTE_A,6},
	{NOTE_G,6},{NOTE_G,6},{NOTE_G,6},
	{NOTE_G,6},{NOTE_G,6},{NOTE_REST},
	
	{NOTE_D,6},{NOTE_REST},{NOTE_D,6},
	{NOTE_D,7},{NOTE_D,7},{NOTE_D,7},	
	{NOTE_B,6},{NOTE_B,6},{NOTE_B,6},	
	{NOTE_G,6},{NOTE_G,6},{NOTE_G,6},	
	{NOTE_Fs,6},{NOTE_Fs,6},{NOTE_Fs,6},
	{NOTE_E,6},{NOTE_E,6},{NOTE_E,6},
	
	{NOTE_C,7},{NOTE_REST},{NOTE_C,7},
	{NOTE_B,6},{NOTE_B,6},{NOTE_B,6},	
	{NOTE_G,6},{NOTE_G,6},{NOTE_G,6},	
	{NOTE_A,6},{NOTE_A,6},{NOTE_A,6},	
	{NOTE_G,6},{NOTE_G,6},{NOTE_G,6},	
	{NOTE_G,6},{NOTE_G,6},{NOTE_END}
};
