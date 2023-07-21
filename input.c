#include "input.h"
#include "structs.h"
#include "include.h"
#include "gameobject.h"
#include "gameplay.h"

extern App app;
extern Player* player;

void doKeyDown(SDL_KeyboardEvent* event) {
    if (!event->repeat) {
        if (event->keysym.scancode == SDL_SCANCODE_UP) app.up = 1;

        if (event->keysym.scancode == SDL_SCANCODE_DOWN) app.down = 1;

        if (event->keysym.scancode == SDL_SCANCODE_LEFT) app.left = 1;

        if (event->keysym.scancode == SDL_SCANCODE_RIGHT) app.right = 1;

        if (event->keysym.scancode == SDL_SCANCODE_SPACE) {
            if (app.gameover) app.gameover = 0;
            else if (player->reload == 0 && !player->dead) {
                if (player->powerUp == TRIPLE) fireTripleBullet();
                else fireBullet();
            }
        }

        if (event->keysym.scancode == SDL_SCANCODE_R) resetLevel();

        if (event->keysym.scancode == SDL_SCANCODE_ESCAPE && !app.gameover) app.pause = ~app.pause;
    }
}

void doKeyUp(SDL_KeyboardEvent* event) {
    if (!event->repeat) {
        if (event->keysym.scancode == SDL_SCANCODE_UP) app.up = 0;

        if (event->keysym.scancode == SDL_SCANCODE_DOWN) app.down = 0;

        if (event->keysym.scancode == SDL_SCANCODE_LEFT) app.left = 0;

        if (event->keysym.scancode == SDL_SCANCODE_RIGHT) app.right = 0; 
    }
}

void doInput() {
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                exit(0);
                break;
            case SDL_KEYDOWN:
                doKeyDown(&event.key);
                break;
            case SDL_KEYUP:
                doKeyUp(&event.key);
                break;
            default:
                break;
        }
    }
}