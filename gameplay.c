#include "gameplay.h"
#include "input.h"
#include "drawing.h"
#include "gameobject.h"
#include "structs.h"
#include "include.h"
#include <stdio.h>

extern App app;
extern Player* player;
extern Stage stage;

extern int FPS;
extern int resetStageTimer;

void getFrames(long* then) {
    static int timeTotal = 0;
    static int frames = 0;

    long now = SDL_GetTicks();
    timeTotal += (now - *then);
    *then = now;

    if (timeTotal >= 1000) {
        FPS = frames;
        printf("INFO: FPS is %d\n", frames);
        timeTotal = 0;
        frames = 0;
        if (player->dead) --resetStageTimer;
    }

    frames++;
}

void capFrameRate(long *then, float *remainder) {
	long wait, frameTime;

	wait = 16 + *remainder;
	*remainder -= (int)*remainder;
	frameTime = SDL_GetTicks() - *then;
	wait -= frameTime;

	if (wait < 1) wait = 1;

	SDL_Delay(wait);
	*remainder += 0.667;
	*then = SDL_GetTicks();
}

void logic() {
    doInput();
    if (!app.pause) doStarfield();
    if (!app.gameover && !app.pause) {
        createAsteroid();
        createAlien();
        if (!player->dead) movePlayer();
        moveBullets();
        moveAsteroids();
        moveDebris();
        moveAliens();
        moveBubbles();
    }
}

void draw() {
    drawStarfield();
    if (!app.gameover) {
        if (!player->dead) drawPlayer();
        drawBubble();
        drawAsteroids();
        drawBullets();
        drawDebris();
        drawAlien();
        // UI drawn after gameobject to override overlap
        drawBackground();
        drawAmmo();
        drawHealth();
        drawPowerUpUI();
    }
    else {
        // Draw title card and "Press SPACE to start"
        drawTitle();
    }
}

void resetLevel() {
    player->e->health = 10;
    player->e->x = 100;
    player->e->y = 100;
    player->ammo = player->ammoMax;
    player->reload = 0;
    player->dead = 0;
    player->kills = 0;

    deleteObjects(stage.bandolierHead);
    stage.bandolierHead = NULL;
    stage.bandolierTail = NULL;
    deleteObjects(stage.waveHead);
    stage.waveHead = NULL;
    stage.waveTail = NULL;
    deleteAliens(stage.enemyHead);
    stage.enemyHead = NULL;
    stage.enemyTail = NULL;
    deleteBubbles();
    stage.bubbleHead = NULL;
    stage.bubbleTail = NULL;

    resetStageTimer = 3;
}

void waitTime(int* seconds, long* then) {
    static int totalTime = 0;
    int now = SDL_GetTicks();
    
    totalTime += (now - *then);
    if (totalTime >= 1000) {
        (*seconds)--;
        totalTime = 0;
    }
}
