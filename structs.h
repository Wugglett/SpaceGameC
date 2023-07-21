#include "include.h"

#ifndef structs
#define structs

#define WIDTH 1280
#define HEIGHT 720

#define PLAYER_SPEED 4
#define PLAYER_BULLET_SPEED 16
#define ENEMY_BULLET_SPEED -12
#define ENEMY_SPEED -5
#define GROUND_SPEED -4
#define PLAYER_SIDE 0
#define ENEMY_SIDE 1

#define MAX_STARS 500
#define MAX_SOUND_CHANNELS 8

#define NUM_HIGHSCORES 8
#define BUBBLE_TYPES 6

enum CHANNEL {CH_ANY = -1, CH_PLAYER, CH_ENEMY};
enum SOUND {SND_PLAYER_SHOOT, SND_ENEMY_SHOOT, SND_PLAYER_DIE, SND_ENEMY_DIE};

enum BUBBLE {NONE = -1, BONUS, RAPID, INSTA, GIANT, TRIPLE, BOMB};

typedef struct {
    int x;
    int y;
    int dx;
    int dy;
    int w;
    int h;
    int health;
    int reload;
    int reloadCount;
    int reloadMax;
    int ammo;
    int ammoMax;
    int side;
    int inv;
    SDL_Texture* texture;
    void* next;
} Entity;

typedef struct {
    Entity* e;
    int reload;
    int reloadCount;
    int reloadMax;
    int ammo;
    int ammoMax;
    int inv;
    int dead;
    int kills;
    enum BUBBLE powerUp;
    int powerUpTimer;
    SDL_Texture* hurtTexture;
} Player;

typedef struct {
    Entity* e;
    int reload;
    int reloadCount;
    int reloadMax;
    int ammo;
    int ammoMax;
} Enemy;

typedef struct {
    Entity* e;
    enum BUBBLE b;
    int life;
} Bubble;

typedef struct {
	int x;
	int y;
	int speed;
} Star;

typedef struct Debris {
	float x;
	float y;
	float dx;
	float dy;
	SDL_Rect rect;
	SDL_Texture *texture;
	int life;
	void* next;
} Debris;

typedef struct {
    Entity* bandolierHead;
    Entity* bandolierTail;
    Entity* waveHead;
    Entity* waveTail;
    Debris* debrisHead;
    Debris* debrisTail;
    Enemy* enemyHead;
    Enemy* enemyTail;
    Bubble* bubbleHead;
    Bubble* bubbleTail;
    int wait;
    int alienWait;
} Stage;

typedef struct {
    SDL_Renderer* renderer;
    SDL_Window* window;
    int up;
    int down;
    int left;
    int right;
    int fire;
    int points;
    int gameover;
    int pause;
} App;
#endif