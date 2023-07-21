#include "structs.h"

#ifndef gameobject
#define gameobject
    void initGameObjects();

    void movePlayer();

    void moveBullets();

    void fireBullet();

    void fireTripleBullet();

    void createAsteroid();

    void moveAsteroids();

    void initStarfield();

    void doStarfield();

    void addDebris(Entity* e);

    void moveDebris();

    void moveAliens();

    void deleteObjects();

    void deleteAliens();

    void fireAlienBullet();

    void createAlien();

    void deleteBubbles();

    void moveBubbles();
#endif