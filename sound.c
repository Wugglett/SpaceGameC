#include "include.h"
#include "structs.h"
#include <string.h>

extern Mix_Chunk* sounds[MAX_SOUND_CHANNELS];

extern Mix_Music* music;

void loadSounds() {
    sounds[SND_PLAYER_SHOOT] = Mix_LoadWAV("sound/player_shoot.wav");
    sounds[SND_ENEMY_SHOOT] = Mix_LoadWAV("sound/alien_shoot.wav");
    sounds[SND_PLAYER_DIE] = Mix_LoadWAV("sound/player_death.wav");
    sounds[SND_ENEMY_DIE] = Mix_LoadWAV("sound/alien_death.wav");
}

void loadMusic(char* filename) {
    if (music != NULL) {
        Mix_HaltMusic();
        Mix_FreeMusic(music);
        music = NULL;
    }
    music = Mix_LoadMUS(filename);
}

void initSounds() {
    memset(&sounds, 0, sizeof(Mix_Chunk) * MAX_SOUND_CHANNELS);

    music = NULL;

    loadSounds();

    loadMusic("sound/game_music.mp3");
}

void playMusic(int loop) {
    Mix_PlayMusic(music, loop ? -1 : 0);
}

void playSound(int id, int channel) {
    Mix_PlayChannel(channel, sounds[id], 0);
}