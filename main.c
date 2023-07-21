#include <stdio.h>
#include <string.h>
#include "structs.h"
#include "drawing.h"
#include "input.h"
#include "gameobject.h"
#include "include.h"
#include "gameplay.h"
#include "sound.h"

// Find what causes the collision bug with triple shot
// Add boss?

// Gamemanager
App app;
Player* player;
Stage stage;
Star stars[500];

Mix_Chunk* sounds[MAX_SOUND_CHANNELS];
Mix_Music* music;

int FPS;
int resetStageTimer = 3;

int WinMain() {

    long then;
    long then2;
    float remainder;

    memset(&app, 0, sizeof(App));
    player = (Player*)malloc(sizeof(Player) + sizeof(Entity));
    memset(&stage, 0, sizeof(Stage));

    initSDL();
    initGameObjects();
    initSounds();

    then = SDL_GetTicks();
    then2 = then;

    while (1) {
        getFrames(&then2);
        prepareScene();
        logic();
        draw();
        presentScene();
        if (player->dead && resetStageTimer <= 0) resetLevel(); 
        capFrameRate(&then, &remainder);
    }
    
    return 0;
}