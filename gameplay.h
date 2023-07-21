#ifndef gameplay
#define gameplay
    void logic();

    void draw();

    void resetLevel();  

    void waitTime(int* seconds, long* then);

    void playerDeath();

    void getFrames(long* then);

    void capFrameRate(long* then, float* remainder);
#endif