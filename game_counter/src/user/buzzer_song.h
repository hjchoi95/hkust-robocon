#ifndef	__BUZZER_SONG_H
#define	__BUZZER_SONG_H

#include "buzzer.h"

extern const MUSIC_NOTE START_UP[];
extern const MUSIC_NOTE SUCCESSFUL_SOUND[];
extern const MUSIC_NOTE FAIL_SOUND[];
extern const MUSIC_NOTE BIRTHDAY_SONG[];
extern const MUSIC_NOTE CLICK[];
extern const MUSIC_NOTE MARIO_BEGIN[];
extern const MUSIC_NOTE MARIO[];
extern const MUSIC_NOTE MARIO_END[];

#define	SUCCESSFUL_MUSIC			buzzer_play_song(SUCCESSFUL_SOUND, 100, 0);
#define	FAIL_MUSIC 						buzzer_play_song(FAIL_SOUND, 120, 100);
#define	CLICK_MUSIC						buzzer_play_song(CLICK, 20, 0);

#define MARIO_BEGIN_MUSIC     buzzer_play_song(MARIO_BEGIN, 80, 0);
#define MARIO_MUSIC           buzzer_play_song(MARIO, 160, 0);
#define MARIO_END_MUSIC       buzzer_play_song(MARIO_END, 80, 0);
#define BIRTHDAY_MUSIC        buzzer_play_song(BIRTHDAY_SONG, 160, 0);

#endif /* __BUZZER_SONG_H */
