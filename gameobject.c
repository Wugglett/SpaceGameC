#include "structs.h"
#include "drawing.h"
#include "gameplay.h"
#include "sound.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

extern App app;
extern Player* player;
extern Stage stage;
extern Star stars[500];

extern int FPS;

SDL_Texture* bulletTexture;
SDL_Texture* bulletGiantTexture;
SDL_Texture* healthTexture;
SDL_Texture* ammoTexture;
SDL_Texture* ammoEmptyTexture;
SDL_Texture* enemyTexture;
SDL_Texture* groundTexture;
SDL_Texture* alienTexture;
SDL_Texture* alienBulletTexture;
SDL_Texture* UITexture;
SDL_Texture* titleTexture;
SDL_Texture* startTexture;
SDL_Texture* bubbleTextures[BUBBLE_TYPES];

int MAX(int x, int y) {return x > y ? x : y;}

int MIN(int x, int y) {return x < y ? x : y;}

int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    return (MAX(x1, x2) < MIN(x1 + w1, x2 + w2)) && (MAX(y1, y2) < MIN(y1 + h1, y2 + h2));
}

void createBubble(Entity* e) {
    Bubble* bubble = (Bubble*)malloc(sizeof(Bubble) + sizeof(Entity));
    bubble->e = (Entity*)malloc(sizeof(Entity));
    bubble->e->x = e->x;
    bubble->e->y = e->y;
    do {
        bubble->e->dx = (rand()%6)-3;
        bubble->e->dy = (rand()%6)-3;
    } while(bubble->e->dx != 0 && bubble->e->dy != 0);
    bubble->e->health = 1;
    bubble->b = rand()%6;
    bubble->e->texture = bubbleTextures[bubble->b];
    SDL_QueryTexture(bubble->e->texture, NULL, NULL, &bubble->e->w, &bubble->e->h);
    bubble->e->next = NULL;
    bubble->e->side = PLAYER_SIDE;
    bubble->life = 30*FPS;

    if (stage.bubbleHead == NULL) {
        stage.bubbleHead = bubble;
        stage.bubbleTail = bubble;
    }
    else {
        stage.bubbleTail->e->next = bubble;
        stage.bubbleTail = bubble;
    }

    printf("INFO: Created bubble\n");
}

void addDebris(Entity* e) {
    Debris* d;
    int w, h;
    w = e->w / 2;
    h = e->h / 2;
    for (int y = 0; y <= h/2; y++) {
        for(int x = 0; x <= w/2; x++) {
            d = (Debris*)malloc(sizeof(Debris));
            memset(d, 0, sizeof(Debris));
            d->x = e->x + (e->w/2);
            d->y = e->y + (e->h/2);
            d->dx = (rand()%5) - (rand()%5);
            d->dy = (5 + rand()%12);
            d->life = rand()%30-rand()%10;
            d->texture = e->texture;

            d->rect.x = x;
            d->rect.y = y;
            d->rect.w = w;
            d->rect.h = h;

            if (stage.debrisHead == NULL) {
                stage.debrisHead = d;
                d->next = stage.debrisTail;
            }
            else {
                if (stage.debrisTail == NULL) {
                    stage.debrisTail = d;
                    stage.debrisHead->next = stage.debrisTail;
                }
                else {
                    stage.debrisTail->next = d;
                    stage.debrisTail = d;
                }
            }
        }
    }
}

void initStarfield() {
	int i;

	for (i = 0 ; i < MAX_STARS ; i++) {
		stars[i].x = rand() % WIDTH;
		stars[i].y = rand() % 720;
		stars[i].speed = 1 + rand() % 8;
	}
}

void initGameObjects() {    
    app.points = 0;
    app.gameover = 1;
    app.pause = 0;

    stage.wait = 100;
    stage.alienWait = 200;

    player->e = (Entity*)malloc(sizeof(Entity));
    player->e->x = 100;
    player->e->y = 100;
    player->e->dx = PLAYER_SPEED;
    player->e->dy = PLAYER_SPEED;
    player->e->texture = loadTexture("gfx/player.png");
    SDL_QueryTexture(player->e->texture, NULL, NULL, &player->e->w, &player->e->h);
    player->e->health = 10;
    player->e->side = PLAYER_SIDE;
    player->e->next = NULL;
    player->kills = 0;

    player->reload = 0;
    player->reloadCount = 0;
    player->reloadMax = 50;
    player->ammoMax = 10;
    player->ammo = 10;
    player->inv = 0;
    player->dead = 0;
    player->powerUp = NONE;
    player->powerUpTimer = 0;

    bulletTexture = loadTexture("gfx/bullet.png");
    bulletGiantTexture = loadTexture("gfx/giant-bullet.png");
    healthTexture = loadTexture("gfx/health.png");
    ammoTexture = loadTexture("gfx/ground.png");
    ammoEmptyTexture = loadTexture("gfx/ammoEmpty.png");
    enemyTexture = loadTexture("gfx/asteroid.png");
    UITexture = loadTexture("gfx/UIBackground.png");
    alienTexture = loadTexture("gfx/enemy.png");
    alienBulletTexture = loadTexture("gfx/alienbullet.png");
    titleTexture = loadTexture("gfx/title.png");
    startTexture = loadTexture("gfx/start.png");

    bubbleTextures[BONUS] = loadTexture("gfx/bonus-bubble.png"); 
    bubbleTextures[RAPID] = loadTexture("gfx/rapid-bubble.png");
    bubbleTextures[INSTA] = loadTexture("gfx/insta-bubble.png");
    bubbleTextures[GIANT] = loadTexture("gfx/giant-bubble.png");
    bubbleTextures[TRIPLE] = loadTexture("gfx/triple-bubble.png");
    bubbleTextures[BOMB] = loadTexture("gfx/bomb-bubble.png");

    initStarfield();

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
        printf("Could not initialize SDL Mixer\n");
        exit(1);
    }
    Mix_AllocateChannels(MAX_SOUND_CHANNELS);
}

Entity* enemyCollide(Entity* player) {
    for(Entity* enemy = stage.waveHead; enemy != NULL; enemy = (Entity*)enemy->next) {
        if (collision(player->x, player->y, player->w, player->h, enemy->x, enemy->y, enemy->w, enemy->h)) return enemy;
    }
    for(Enemy* enemy = stage.enemyHead; enemy != NULL; enemy = (Enemy*)enemy->e->next) {
        if (collision(player->x, player->y, player->w, player->h, enemy->e->x, enemy->e->y, enemy->e->w, enemy->e->h)) return enemy->e;
    }
    return NULL;
}

Bubble* bubbleCollide(Entity* e) {
    for(Bubble* b = stage.bubbleHead; b != NULL; b = (Bubble*)b->e->next) {
        if (collision(e->x, e->y, e->w, e->h, b->e->x, b->e->y, b->e->w, b->e->h)) return b;
    }
    return NULL;
}

void damagePlayer() {
    player->e->health--;
    if (player->e->health == 0) {
        addDebris(player->e);
        player->dead = 1;
        playSound(SND_PLAYER_DIE, CH_PLAYER);
    }
    else player->inv = 5;
}

void movePlayer() {
    if (app.up && player->e->y > 0) player->e->y -= player->e->dy;
    if (app.down && player->e->y < HEIGHT-125) player->e->y += player->e->dy;
    if (app.left && player->e->x > 0) player->e->x -= player->e->dx;
    if (app.right && player->e->x < WIDTH-50) player->e->x += player->e->dx;

    Entity* enemy;
    if ((enemy = enemyCollide(player->e)) && player->inv == 0) {
        damagePlayer();
        enemy->health = 0;
        addDebris(enemy);
    }
    else if (player->inv != 0) player->inv--;
    
    if (player->powerUpTimer > 0) player->powerUpTimer--;
    else if (player->powerUp != NONE) player->powerUp = NONE;

    Bubble* bubble;
    if ((bubble = bubbleCollide(player->e))) {
        bubble->e->health = 0;
        if (bubble->b == BONUS) app.points += 500;
        else if (bubble->b == BOMB) {
            // Blow up powerup and damage player
            player->e->health -= 5;
        }
        else {
            player->powerUp = bubble->b;
            player->powerUpTimer = 30*FPS;
        }
    }

    if (player->reload) {
        player->reloadCount++;
        if(player->reloadCount == player->reloadMax) {
            player->reload = 0;
            player->ammo = player->ammoMax;
            player->reloadCount = 0;
        }
    }
}

void bulletCollision(Entity* bullet) {
    for (Entity* asteroid = stage.waveHead; asteroid != NULL; asteroid = (Entity*)asteroid->next) {
        if (asteroid->side != bullet->side && collision(bullet->x, bullet->y, bullet->w, bullet->h, asteroid->x, asteroid->y, asteroid->w, asteroid->h)) {
            if (player->powerUp == INSTA) asteroid->health = 0;
            else if (player->powerUp == GIANT) asteroid->health -= 2;
            else  asteroid->health--;

            if (asteroid->health <= 0) {
                player->kills++;
                addDebris(asteroid);
                playSound(SND_ENEMY_DIE, CH_ANY);
                app.points += 100;
            }

            bullet->health = 0;
            return;
        }
    }
    for (Enemy* enemy = stage.enemyHead; enemy != NULL; enemy = (Enemy*)enemy->e->next) {
        if (enemy->e->side != bullet->side && collision(bullet->x, bullet->y, bullet->w, bullet->h, enemy->e->x, enemy->e->y, enemy->e->w, enemy->e->h)) {
            if (player->powerUp == INSTA) enemy->e->health = 0;
            else if (player->powerUp == GIANT) enemy->e->health -= 2;
            else enemy->e->health--;

            if (enemy->e->health <= 0) {
                player->kills++;
                addDebris(enemy->e);
                playSound(SND_ENEMY_DIE, CH_ANY);
                app.points += 100;
            }

            bullet->health = 0;
            return;
        }
    }
    if (player->e->side != bullet->side && collision(bullet->x, bullet->y, bullet->w, bullet->h, player->e->x, player->e->y, player->e->w, player->e->h)) {
        damagePlayer();
        bullet->health = 0;
        return;
    }
}

void fireBullet() {
    Entity* bullet = (Entity*)malloc(sizeof(Entity));
    bullet->x = player->e->x;
    bullet->y = player->e->y;
    bullet->dx = PLAYER_BULLET_SPEED;
    if (app.up) bullet->dy = -PLAYER_SPEED/2;
    else if (app.down) bullet->dy = PLAYER_SPEED/2;
    else bullet->dy = 0;
    bullet->health = 1;
    bullet->texture = (player->powerUp == GIANT) ? bulletGiantTexture : bulletTexture;
    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);
    bullet->next = NULL;
    bullet->side = PLAYER_SIDE;

    if (stage.bandolierHead == NULL) {
        stage.bandolierHead = bullet;
        stage.bandolierTail = bullet;
    }
    else {
        stage.bandolierTail->next = (void*)bullet;
        stage.bandolierTail = bullet;
    }

    if (player->powerUp != RAPID) {
        player->ammo--;
        if (player->ammo == 0) {
            player->reload = 1;
        }
    }

    playSound(SND_PLAYER_SHOOT, CH_PLAYER);

    printf("INFO: Created bullet\n");
}

void fireTripleBullet() {
    for (int i = 0; i < 3; i++) {
        Entity* bullet = (Entity*)malloc(sizeof(Entity));
        bullet->x = player->e->x;
        bullet->y = player->e->y;
        bullet->dx = PLAYER_BULLET_SPEED;
        if (i == 0) bullet->dy = -PLAYER_SPEED/2;
        else if (i == 1) bullet->dy = PLAYER_SPEED/2;
        else bullet->dy = 0;
        bullet->health = 1;
        bullet->texture = bulletTexture;
        SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);
        bullet->next = NULL;
        bullet->side = PLAYER_SIDE;

        if (stage.bandolierHead == NULL) {
            stage.bandolierHead = bullet;
            stage.bandolierTail = bullet;
        }
        else {
            stage.bandolierTail->next = (void*)bullet;
            stage.bandolierTail = bullet;
        }
    }

    player->ammo--;
    if (player->ammo == 0) {
        player->reload = 1;
    }

    playSound(SND_PLAYER_SHOOT, CH_PLAYER);

    printf("INFO: Created triple bullet\n");
} 

void moveBullets() {
    Entity* prev = stage.bandolierHead;
    for (Entity* bullet = stage.bandolierHead; bullet != NULL; bullet = (Entity*)bullet->next) {
        bullet->x += bullet->dx;
        bullet->y += bullet->dy;
        bulletCollision(bullet);
        if (bullet->x > WIDTH || bullet->health == 0 || bullet->x < 0) {
            if (bullet == stage.bandolierHead) {
                stage.bandolierHead = bullet->next;
                bullet = prev;
            }
            else {
                prev->next = bullet->next;
                bullet = prev;
            }
        }
        prev = bullet;
    }
}

void moveBubbles() {
    Bubble* prev = stage.bubbleHead;
    for (Bubble* bubble = stage.bubbleHead; bubble != NULL; bubble = (Bubble*)bubble->e->next) {
        bubble->e->x += bubble->e->dx;
        if (bubble->e->x > WIDTH || bubble->e->x < 0) bubble->e->dx = -bubble->e->dx;

        bubble->e->y += bubble->e->dy;
        if (bubble->e->y > 570 || bubble->e->y < 0) bubble->e->dy = -bubble->e->dy;

        if (bubble->e->health == 0 || --bubble->life == 0) {
            if (bubble == stage.bubbleHead) {
                stage.bubbleHead = bubble->e->next;
                bubble = prev;
            }
            else {
                prev->e->next = bubble->e->next;
                bubble = prev;
            }
        }
        prev = bubble;
    }
}

void createAsteroid() {
    srand(time(NULL)/2);
    if (stage.wait == 0) {
        Entity* asteroid = (Entity*)malloc(sizeof(Entity));
        asteroid->x = 1280;
        asteroid->y = (rand()%570); // Random Y POS between 0 and 570
        asteroid->dx = ENEMY_SPEED + ((rand()%4)-2);
        asteroid->dy = 0;
        asteroid->next = NULL;
        asteroid->reload = 0;
        asteroid->ammo = 0;
        asteroid->health = 2;
        asteroid->texture = enemyTexture;
        SDL_QueryTexture(asteroid->texture, NULL, NULL, &asteroid->w, &asteroid->h);
        asteroid->side = ENEMY_SIDE;

        if (stage.waveHead == NULL) {
            stage.waveHead = asteroid;
            stage.waveTail = asteroid;
        }
        else {
            stage.waveTail->next = asteroid;
            stage.waveTail = asteroid;
        }

        printf("INFO: Created Asteroid\n");

        stage.wait = (rand() % 100) + 50;
    }
    else {
        stage.wait--;
    }

}

void moveAsteroids() {
    Entity* prev = stage.waveHead;
    for (Entity* asteroid = stage.waveHead; asteroid != NULL; asteroid = (Entity*)asteroid->next) {
        asteroid->x += asteroid->dx;
        asteroid->y += asteroid->dy;
        if (asteroid->x <= 0 || asteroid->health == 0) {
            if (asteroid->health < 0 && (rand()%100) < (10+player->kills)) {
                player->kills = 0;
                createBubble(asteroid);
            }
            if (asteroid == stage.waveHead) {
                stage.waveHead = (Entity*)asteroid->next;
                asteroid = prev;
            }
            else {
                prev->next = asteroid->next;
                asteroid = prev;
            }
        }
        prev = asteroid;
    }
}

void deleteObjects(Entity* object) {
    if (object == NULL) return;
    deleteObjects((Entity*)object->next);
    object = NULL;
    return;
}

void deleteAliens(Enemy* e) {
    if (e == NULL) return;
    deleteAliens((Enemy*)e->e->next);
    e->e = NULL;
    e = NULL;
}

void deleteBubbles(Bubble* b) {
    if (b == NULL) return;
    deleteBubbles((Bubble*)b->e->next);
    b->e = NULL;
    b = NULL;
}

void doStarfield() {
	int i;

	for (i = 0 ; i < MAX_STARS ; i++) {
		stars[i].x -= stars[i].speed;

		if (stars[i].x < 0) {
			stars[i].x = WIDTH + stars[i].x;
		}
	}
}

void moveDebris() {
    Debris* prev = stage.debrisHead;
    for (Debris* d = stage.debrisHead; d != NULL; d = (Debris*)d->next) {
        d->x += d->dx;
        d->y += d->dy;
        if (--d->life <= 0) {
            if (d == stage.debrisHead) {
                stage.debrisHead = (Debris*)d->next;
                d = prev;
            }
            else {
                prev->next = d->next;
                d = prev;
            }
        }
        prev = d;
    }
}

void alienFireBullet(Entity* e) {
    Entity* bullet = (Entity*)malloc(sizeof(Entity));
    bullet->x = e->x;
    bullet->y = e->y;
    bullet->dx = ENEMY_BULLET_SPEED;
    bullet->dy = 0;
    bullet->health = 1;
    bullet->texture = alienBulletTexture;
    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);
    bullet->next = NULL;
    bullet->side = ENEMY_SIDE;

    if (stage.bandolierHead == NULL) {
        stage.bandolierHead = bullet;
        stage.bandolierTail = bullet;
    }
    else {
        stage.bandolierTail->next = (void*)bullet;
        stage.bandolierTail = bullet;
    }

    playSound(SND_ENEMY_SHOOT, CH_ENEMY);

    printf("INFO: Created Enemy bullet\n");
}

void moveAliens() {
    Enemy* prev = stage.enemyHead;
    for (Enemy* enemy = stage.enemyHead; enemy != NULL; enemy = (Enemy*)enemy->e->next) {
        enemy->e->x += enemy->e->dx;
        enemy->e->y += enemy->e->dy;
        if (enemy->e->x < 0 || enemy->e->health <= 0) {
            if (enemy == stage.enemyHead) {
                stage.enemyHead = (Enemy*)enemy->e->next;
                enemy = prev;
            }
            else {
                prev->e->next = enemy->e->next;
                enemy = prev;
            }
        }
        int i = enemy->e->x;
        if (enemy->reload == 0) {
            while (i > 0) {
                if (collision(player->e->x, player->e->y, player->e->w, player->e->h, i, enemy->e->y, enemy->e->w, enemy->e->h)) {
                    alienFireBullet(enemy->e);
                    enemy->reload = 100;
                    break;
                }
                i -= enemy->e->w;
            }
        }
        else enemy->reload--;
        prev = enemy;
    }
}

void createAlien() {
    srand(time(NULL));
    if (stage.alienWait == 0) {
        Enemy* enemy = (Enemy*)malloc(sizeof(Enemy) + sizeof(Entity));
        enemy->e = (Entity*)malloc(sizeof(Entity));
        enemy->e->x = 1280;
        enemy->e->y = (rand()%570); // Random Y POS between 0 and 570
        enemy->e->dx = ENEMY_SPEED + ((rand()%4)-2);
        enemy->e->dy = 0;
        enemy->e->next = NULL;
        enemy->e->health = 1;
        enemy->e->texture = alienTexture;
        SDL_QueryTexture(enemy->e->texture, NULL, NULL, &enemy->e->w, &enemy->e->h);
        enemy->e->side = ENEMY_SIDE;
        enemy->reload = 0;
        enemy->ammo = 0;

        if (stage.enemyHead == NULL) {
            stage.enemyHead = enemy;
            stage.enemyTail = enemy;
        }
        else {
            stage.enemyTail->e->next = enemy;
            stage.enemyTail = enemy;
        }

        printf("INFO: Created Enemy\n");

        stage.alienWait = (rand() % 200) + 200;
    }
    else {
        stage.alienWait--;
    }
}