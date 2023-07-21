#include "drawing.h"
#include "structs.h"
#include "include.h"
#include <stdio.h>

#define UI_WIDTH 32

extern App app;
extern Player* player;
extern Stage stage;
extern Star stars[MAX_STARS];

extern SDL_Texture* healthTexture;
extern SDL_Texture* ammoTexture;
extern SDL_Texture* ammoEmptyTexture;
extern SDL_Texture* UITexture;
extern SDL_Texture* titleTexture;
extern SDL_Texture* startTexture;
extern SDL_Texture* bubbleTextures[BUBBLE_TYPES];

void initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    app.window = SDL_CreateWindow("Shooter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);

    if (app.window == NULL) {
        printf("Failed to open window: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    app.renderer = SDL_CreateRenderer(app.window, -1, SDL_RENDERER_ACCELERATED);

    if (app.renderer == NULL) {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        exit(1);
    }

    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    SDL_ShowCursor(0);
}

SDL_Texture* loadTexture(char* filename) {
    SDL_Texture* texture;

    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);

    texture = IMG_LoadTexture(app.renderer, filename);

    return texture;
}

void blit(SDL_Texture* texture, int x, int y) {
    SDL_Rect dest;

    dest.x = x;
    dest.y = y;
    SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);

    SDL_RenderCopy(app.renderer, texture, NULL, &dest);
}

void prepareScene() {
    SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
    SDL_RenderClear(app.renderer);
}

void presentScene() {
    SDL_RenderPresent(app.renderer);
}

void drawBullets() {
    for (Entity* bullet = stage.bandolierHead; bullet != NULL; bullet = bullet->next) {
        blit(bullet->texture, bullet->x, bullet->y);
    }
}

void drawPlayer() {
    blit(player->e->texture, player->e->x, player->e->y);
}

void drawHealth() {
    int w, h;
    for(int i = 0; i < 10; i++) {
        SDL_QueryTexture(ammoTexture, NULL, NULL, &w, &h);
        blit(ammoEmptyTexture, w+(w*i), HEIGHT-h*2); // Used for red section under green health
    }
    for(int i = 0; i < player->e->health; i++) {
        SDL_QueryTexture(healthTexture, NULL, NULL, &w, &h);
        blit(healthTexture, w+(w*i), HEIGHT-h*2);
    }
}

void drawAmmo() {
    int w, h;
    if (!player->reload) {
        SDL_QueryTexture(ammoTexture, NULL, NULL, &w, &h);
        for (int i = 0; i < player->ammo; i++) {
            blit(ammoTexture, ((WIDTH-player->ammoMax*w)-w) + (w*i), HEIGHT-h*2);
        }
    }
    else {
        SDL_QueryTexture(ammoEmptyTexture, NULL, NULL, &w, &h);
        for (int i = 0; i < player->reloadCount; i++) {
            if (i%(player->reloadMax/player->ammoMax) == 0) blit(ammoEmptyTexture, ((WIDTH-(player->ammoMax*w))-w) + (w*i/(player->reloadMax/player->ammoMax)), HEIGHT-h*2);
        }
    }
}

void drawAsteroids() {
    for (Entity* asteroid = stage.waveHead; asteroid != NULL; asteroid = (Entity*)asteroid->next) {
        blit(asteroid->texture, asteroid->x, asteroid->y);
    }
}

void drawBackground() {
    int totalWidth = 0;
    int w, h;
    SDL_QueryTexture(UITexture, NULL, NULL, &w, &h);
    for (int i = 1; i < 4; i++) {
        while (totalWidth < WIDTH) {
            blit(UITexture, totalWidth, HEIGHT-h*i);
            totalWidth += w;
        }
        totalWidth = 0;
    }
}

void blitRect(SDL_Texture *texture, SDL_Rect *src, int x, int y) {
	SDL_Rect dest;

	dest.x = x;
	dest.y = y;
	dest.w = src->w;
	dest.h = src->h;

	SDL_RenderCopy(app.renderer, texture, src, &dest);
}

void drawStarfield() {
	int c;

	for (int i = 0 ; i < MAX_STARS ; i++) {
		c = 32 * stars[i].speed;

		SDL_SetRenderDrawColor(app.renderer, c, c, c, 255);

		SDL_RenderDrawLine(app.renderer, stars[i].x, stars[i].y, stars[i].x + 3, stars[i].y);
	}
}

void drawDebris() {
    for (Debris* d = stage.debrisHead; d != NULL; d = (Debris*)d->next) {
        blitRect(d->texture, &d->rect, d->x, d->y);
    }
}

void drawAlien() {
    for (Enemy* enemy = stage.enemyHead; enemy != NULL; enemy = (Enemy*)enemy->e->next) {
        blit(enemy->e->texture, enemy->e->x, enemy->e->y);
    }
}

void drawBubble() {
    for (Bubble* bubble = stage.bubbleHead; bubble != NULL; bubble = (Bubble*)bubble->e->next) {
        blit(bubble->e->texture, bubble->e->x, bubble->e->y);
    }
}

void drawTitle() {
    blit(titleTexture, 375, -50);

    blit(startTexture, 375, 200);
}

void drawPowerUpUI() {
    if (player->powerUp > 0 && player->powerUp < 5) blit(bubbleTextures[player->powerUp], WIDTH/2, HEIGHT-75);
}