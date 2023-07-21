#include "include.h"

#ifndef drawing
#define drawing
    void initSDL();

    SDL_Texture* loadTexture(char* filename);

    void blit(SDL_Texture* texture, int x, int y);

    void prepareScene();

    void presentScene();

    void drawPlayer();

    void drawBullets();

    void drawHealth();

    void drawAmmo();

    void drawAsteroids();

    void drawGround();

    void drawBackground();

    void drawStarfield();

    void drawDebris();

    void drawAlien();

    void blitRect(SDL_Texture* texture, SDL_Rect* src, int x, int y);

    void drawBubble();

    void drawTitle();

    void drawPowerUpUI();
#endif