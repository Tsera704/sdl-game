#ifndef STAGE_H
#define STAGE_H

#include "structs.h" // Needed for Entity*

// Main init/logic functions
void initStage(void);
void resetStage(void);
void logic(void);
void capFrameRate(long *then, float *remainder);

// Entity initialization
void initPlayer(void);
void initStarfield(void);

// Player logic
void doPlayer(void);
void clipPlayer(void);
void fireBullet(void);

// Enemy logic
void doEnemies(void);
void spawnEnemies(void);
void fireAlienBullet(Entity *e);

// General entity updates
void doBullets(void);
void doFighters(void);
int bulletHitFighter(Entity *b);
int bulletHitBullet(Entity *b);

// Effects logic
void doBackground(void);
void doStarfield(void);
void doExplosions(void);
void doDebris(void);
void addExplosions(int x, int y, int num);
void addDebris(Entity *e);

#endif // STAGE_H