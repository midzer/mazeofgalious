#include "SDL.h"
#include "SDL_mixer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define AUDIO_BUFFER 4096
#define MAX_MUSIC_TRACKS 8

// Music state
extern int music_volume;
extern char *s_path;
extern char *default_s_path;

bool sound_enabled = false;
int music_position = 0;
bool playing_music = false;
bool music_loaded[MAX_MUSIC_TRACKS] = {false};
Mix_Music *music_sound[MAX_MUSIC_TRACKS] = {NULL};
char music_files[MAX_MUSIC_TRACKS][128];
char music_realfiles[MAX_MUSIC_TRACKS][128];

// Utility: check if a file exists and is non-empty
int file_check(const char *fname) {
    FILE *fp = fopen(fname, "r");
    if (fp) {
        if (fseek(fp, 0L, SEEK_END) == 0 && ftell(fp) > 0) {
            fclose(fp);
            return 1;
        }
        fclose(fp);
        fprintf(stderr, "ERROR in file_check(): the file %s is corrupted.\n", fname);
        exit(1);
    }
    return 0;
}

// Utility: get full file name with extension and path
void Sound_obtain_file_name(const char *file, char *fullname) {
    const char *ext[6] = {".WAV", ".OGG", ".MP3", ".wav", ".ogg", ".mp3"};
    char name[256], name2[256];
    for (int i = 0; i < 6; i++) {
        snprintf(name, sizeof(name), "%s%s", file, ext[i]);
        snprintf(name2, sizeof(name2), "%s%s", s_path, name);
        if (file_check(name2)) {
            strcpy(fullname, name2);
            return;
        }
        snprintf(name2, sizeof(name2), "%s%s", default_s_path, name);
        if (file_check(name2)) {
            strcpy(fullname, name2);
            return;
        }
    }
    fprintf(stderr, "ERROR in Sound_obtain_file_name(): Could not find sound file: %s\n", file);
    fullname[0] = 0;
    exit(1);
}

// Sound effect type
typedef Mix_Chunk* SOUNDT;

// Sound effect loading
SOUNDT Sound_create_sound(const char *file) {
    const char *ext[6] = {".WAV", ".OGG", ".MP3", ".wav", ".ogg", ".mp3"};
    char name[256], name2[256];
    if (sound_enabled) {
        for (int i = 0; i < 6; i++) {
            snprintf(name, sizeof(name), "%s%s", file, ext[i]);
            snprintf(name2, sizeof(name2), "%s%s", s_path, name);
            if (file_check(name2)) return Mix_LoadWAV(name2);
        }
        for (int i = 0; i < 6; i++) {
            snprintf(name, sizeof(name), "%s%s", file, ext[i]);
            snprintf(name2, sizeof(name2), "%s%s", default_s_path, name);
            if (file_check(name2)) return Mix_LoadWAV(name2);
        }
        fprintf(stderr, "ERROR in Sound_create_sound(): Could not load sound file: %s\n", file);
        exit(1);
    }
    return NULL;
}

// Free sound effect
void Delete_sound(SOUNDT s) {
    if (sound_enabled && s) Mix_FreeChunk(s);
}

// Play sound effect
void Sound_play(SOUNDT s) {
    if (sound_enabled && s) Mix_PlayChannel(-1, s, 0);
}

// Music loading
Mix_Music *Sound_create_stream(const char *file) {
    const char *ext[6] = {".WAV", ".OGG", ".MP3", ".wav", ".ogg", ".mp3"};
    char name[256], name2[256];
    if (sound_enabled) {
        for (int i = 0; i < 6; i++) {
            snprintf(name, sizeof(name), "%s%s", file, ext[i]);
            snprintf(name2, sizeof(name2), "%s%s", s_path, name);
            if (file_check(name2)) return Mix_LoadMUS(name2);
        }
        for (int i = 0; i < 6; i++) {
            snprintf(name, sizeof(name), "%s%s", file, ext[i]);
            snprintf(name2, sizeof(name2), "%s%s", default_s_path, name);
            if (file_check(name2)) return Mix_LoadMUS(name2);
        }
        fprintf(stderr, "ERROR in Sound_create_stream(): Could not load music file: %s\n", file);
        exit(1);
    }
    return NULL;
}

// Music sequencing callback
void music_finished_callback(void);

// Start music sequence
void start_music_sequence(void) {
    music_position = 0;
    Mix_HookMusicFinished(music_finished_callback);
    if (music_loaded[music_position] && music_sound[music_position]) {
        Mix_PlayMusic(music_sound[music_position], 1);
        Mix_VolumeMusic(music_volume);
        playing_music = true;
    }
}

// Callback: play next track when current finishes
void music_finished_callback(void) {
    music_position++;
    if (music_position < MAX_MUSIC_TRACKS &&
        music_loaded[music_position] && music_sound[music_position]) {
        Mix_PlayMusic(music_sound[music_position], 1);
        Mix_VolumeMusic(music_volume);
    } else {
        playing_music = false;
    }
}

// Load up to 3 music tracks for sequencing
void Sound_create_music(const char *f1, const char *f2, const char *f3) {
    char tmp[128];
    for (int i = 0; i < 3; i++) {
        music_loaded[i] = false;
        if (music_sound[i]) {
            Mix_FreeMusic(music_sound[i]);
            music_sound[i] = NULL;
        }
    }
    if (sound_enabled) {
        if (f1) {
            Sound_obtain_file_name(f1, tmp);
            strcpy(music_files[0], f1);
            strcpy(music_realfiles[0], tmp);
            music_loaded[0] = true;
            music_sound[0] = Sound_create_stream(f1);
        }
        if (f2) {
            Sound_obtain_file_name(f2, tmp);
            strcpy(music_files[1], f2);
            strcpy(music_realfiles[1], tmp);
            music_loaded[1] = true;
            music_sound[1] = Sound_create_stream(f2);
        }
        if (f3) {
            Sound_obtain_file_name(f3, tmp);
            strcpy(music_files[2], f3);
            strcpy(music_realfiles[2], tmp);
            music_loaded[2] = true;
            music_sound[2] = Sound_create_stream(f3);
        }
        playing_music = true;
		start_music_sequence();
    }
}

// Release all music
void Sound_release_music(void) {
    for (int i = 0; i < 3; i++) {
        if (music_loaded[i] && music_sound[i]) {
            Mix_FreeMusic(music_sound[i]);
            music_sound[i] = NULL;
        }
        music_loaded[i] = false;
    }
    playing_music = false;
}

// Pause/unpause music
void Sound_pause_music(void) {
    if (sound_enabled) {
        Mix_PauseMusic();
        playing_music = false;
    }
}
void Sound_unpause_music(void) {
    if (sound_enabled) {
        Mix_ResumeMusic();
        playing_music = true;
    }
}

// Set music volume (0-128)
void set_music_volume(int vol) {
    music_volume = vol;
    Mix_VolumeMusic(music_volume);
}

// Initialize SDL2_mixer
bool Sound_initialization(void) {
    int audio_rate = 44100;
    int audio_channels = 2;
    int audio_bufsize = AUDIO_BUFFER;
    Uint16 audio_format = AUDIO_S16;

    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_bufsize)) {
        fprintf(stderr, "Unable to open audio: %s\n", Mix_GetError());
        sound_enabled = false;
        return false;
    }
    sound_enabled = true;
    return true;
}

// Cleanup SDL2_mixer
void Sound_release(void) {
    Sound_release_music();
    Mix_CloseAudio();
    sound_enabled = false;
}

void Sound_subst_music_now(const char *f) {
    Sound_release_music();
    Sound_create_music(f, NULL, NULL);
    start_music_sequence();
}

void Sound_temporary_release_music(void) {
    // If you want to temporarily release music, you can pause it:
    Sound_pause_music();
    // Optionally, you can also free the music tracks if needed.
}

void music_recovery(void)
{
    char tmp[128];
    int seq_len = 0;

    if (sound_enabled) {
        for (int i = 0; i < MAX_MUSIC_TRACKS; i++) {
            if (music_loaded[i]) {
                // Mark as loaded (may not be necessary if already true)
                music_loaded[i] = true;

                // Get the real file name
                Sound_obtain_file_name(music_files[i], tmp);
                strcpy(music_realfiles[i], tmp);

                // Free previously loaded music to avoid leaks
                if (music_sound[i]) {
                    Mix_FreeMusic(music_sound[i]);
                    music_sound[i] = NULL;
                }

                // Load the music file using SDL2_mixer
                music_sound[i] = Mix_LoadMUS(music_files[i]);
                if (!music_sound[i]) {
                    SDL_Log("Failed to load music %s: %s\n", music_files[i], Mix_GetError());
                }

                seq_len = i + 1;
            }
        }
        playing_music = true;
    }
}
