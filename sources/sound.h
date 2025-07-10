#ifndef __BRAIN_SDL_SOUND
#define __BRAIN_SDL_SOUND

#include <stdbool.h>

#define MAX_MUSIC_TRACKS 8

// Global variables (declare as extern here, define in one .c file)
extern bool sound_enabled;
extern int music_position;
extern bool playing_music;
extern bool music_loaded[MAX_MUSIC_TRACKS];
extern Mix_Music *music_sound[MAX_MUSIC_TRACKS];
extern char music_files[MAX_MUSIC_TRACKS][128];
extern char music_realfiles[MAX_MUSIC_TRACKS][128];

// Sound effect type
typedef Mix_Chunk* SOUNDT;

// Sound initialization and cleanup
bool Sound_initialization(void);
void Sound_release(void);

// Sound effect loading/playing
SOUNDT Sound_create_sound(const char *file);
void Delete_sound(SOUNDT s);
void Sound_play(SOUNDT s);

// Music loading, sequencing, and control
void Sound_create_music(const char *f1, const char *f2, const char *f3);
void Sound_subst_music(const char *f);
void Sound_subst_music_now(const char *f);
void Sound_temporary_release_music(void);
void Sound_release_music(void);
void Sound_pause_music(void);
void Sound_unpause_music(void);
void music_recovery(void);

// Volume control
void set_music_volume(int vol);

// Start music sequence (calls Mix_PlayMusic etc.)
void start_music_sequence(void);

#endif // __BRAIN_SDL_SOUND
