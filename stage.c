#include "structs.h"
#include "defs.h"
#include "draw.h"
#include "stage.h" // <-- CRITICAL: Include its own header
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>

#define TARGET_MS (1000.0 / FPS)

// Global variables (defined in main.c)
extern App app;
extern Entity *player;
extern Stage stage;
extern Star stars[MAX_STARS];
extern int backgroundX;

// Global textures (defined in main.c)
extern SDL_Texture *playerTexture;
extern SDL_Texture *bulletTexture;
extern SDL_Texture *enemyTexture;
extern SDL_Texture *enemyBulletTexture;
extern SDL_Texture *background;
extern SDL_Texture *explosionTexture;

// Timers
int enemySpawnTimer;
int stageResetTimer;

void initStage(void)
{
    stage.explosionTail = &stage.explosionHead;
    stage.debrisTail = &stage.debrisHead;

    playerTexture = loadTexture("C:/Users/Suhani/Downloads/combatjet-highres.png");
    bulletTexture = loadTexture("C:/Users/Suhani/Downloads/firing_bullet.png");
    enemyTexture = loadTexture("C:/Users/Suhani/Downloads/villain.png");
    enemyBulletTexture = loadTexture("C:/Users/Suhani/Downloads/alienBullet.png");
    background = loadTexture("C:/Users/Suhani/Downloads/bg.png");
    explosionTexture = loadTexture("C:/Users/Suhani/Downloads/explosion.png");

    memset(&stage, 0, sizeof(Stage));
    stage.fighterTail = &stage.fighterHead;
    stage.bulletTail = &stage.bulletHead;

    resetStage();
}

void initStarfield(void)
{
    int i;
    for (i = 0; i < MAX_STARS; i++)
    {
        stars[i].x = rand() % SCREEN_WIDTH;
        stars[i].y = rand() % SCREEN_HEIGHT;
        stars[i].speed = 1 + rand() % 8;
    }
}

void initPlayer()
{
    player = (Entity *)malloc(sizeof(Entity)); // <-- Cast already here, but good to check
    memset(player, 0, sizeof(Entity));

    stage.fighterTail->next = player;
    stage.fighterTail = player;

    player->side = SIDE_PLAYER;
    player->x = 100.0f;
    player->y = 100.0f;
    player->health = 1;
    player->texture = playerTexture;
    SDL_QueryTexture(player->texture, NULL, NULL, &player->w, &player->h);
}

void resetStage(void)
{
    Entity *e;
    Explosion *ex;
    Debris *d;

    while (stage.fighterHead.next)
    {
        e = stage.fighterHead.next;
        stage.fighterHead.next = e->next;
        free(e);
    }
    while (stage.bulletHead.next)
    {
        e = stage.bulletHead.next;
        stage.bulletHead.next = e->next;
        free(e);
    }
    while (stage.explosionHead.next)
    {
        ex = stage.explosionHead.next;
        stage.explosionHead.next = ex->next;
        free(ex);
    }
    while (stage.debrisHead.next)
    {
        d = stage.debrisHead.next;
        stage.debrisHead.next = d->next;
        free(d);
    }

    memset(&stage, 0, sizeof(Stage));
    stage.fighterTail = &stage.fighterHead;
    stage.bulletTail = &stage.bulletHead;
    stage.explosionTail = &stage.explosionHead;
    stage.debrisTail = &stage.debrisHead;

    initPlayer();
    initStarfield();

    enemySpawnTimer = 0;
    stageResetTimer = FPS * 3;
}

void logic(void)
{
    doBackground();
    doStarfield();

    if (player != NULL)
    {
        doPlayer();
        doEnemies();
        spawnEnemies();
        clipPlayer();
    }

    doBullets();
    doFighters();
    doExplosions();
    doDebris();

    if (player == NULL && --stageResetTimer <= 0)
    {
        currentGameState = STATE_TITLE_SCREEN;
    }
}

void clipPlayer(void)
{
    if (player == NULL) return;

    float scaled_w = player->w * ENTITY_SCALE_FACTOR;
    float scaled_h = player->h * ENTITY_SCALE_FACTOR;

    if (player->x < 0) player->x = 0;
    if (player->y < 0) player->y = 0;
    if (player->x > SCREEN_WIDTH / 2 - scaled_w)
    {
        player->x = SCREEN_WIDTH / 2 - scaled_w;
    }
    if (player->y > SCREEN_HEIGHT - scaled_h)
    {
        player->y = SCREEN_HEIGHT - scaled_h;
    }
}

void doPlayer(void)
{
    player->dx = player->dy = 0;
    if (player->reload > 0) player->reload--;
    if (app.keyboard[SDL_SCANCODE_UP]) player->dy = -PLAYER_SPEED;
    if (app.keyboard[SDL_SCANCODE_DOWN]) player->dy = PLAYER_SPEED;
    if (app.keyboard[SDL_SCANCODE_LEFT]) player->dx = -PLAYER_SPEED;
    if (app.keyboard[SDL_SCANCODE_RIGHT]) player->dx = PLAYER_SPEED;
    if (app.keyboard[SDL_SCANCODE_SPACE] && player->reload == 0)
    {
        fireBullet();
    }
    player->x += player->dx;
    player->y += player->dy;
}

void fireBullet(void)
{
    Entity *bullet;
    float scaled_player_w = player->w * ENTITY_SCALE_FACTOR;
    float scaled_player_h = player->h * ENTITY_SCALE_FACTOR;

    bullet = (Entity *)malloc(sizeof(Entity));
    memset(bullet, 0, sizeof(Entity));

    stage.bulletTail->next = bullet;
    stage.bulletTail = bullet;

    bullet->dx = PLAYER_BULLLET_SPEED; // Player bullet always moves right
    bullet->health = 1;
    bullet->side = SIDE_PLAYER;
    bullet->texture = bulletTexture;

    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);
    
    // Get scaled size of the bullet
    float scaled_bullet_w = bullet->w * ENTITY_SCALE_FACTOR;
    float scaled_bullet_h = bullet->h * ENTITY_SCALE_FACTOR;
    
    // --- FIXED ---
    // Spawn from the player's scaled center
    
    // 1. Find player's scaled center
    float player_center_x = player->x + (scaled_player_w / 2.0f);
    float player_center_y = player->y + (scaled_player_h / 2.0f);

    // 2. Set bullet's top-left so its center aligns with player's center
    bullet->x = player_center_x - (scaled_bullet_w / 2.0f);
    bullet->y = player_center_y - (scaled_bullet_h / 2.0f);

    player->reload = 8;
}
int bulletHitFighter(Entity *b)
{
    Entity *e;

    // CHANGED: No more ternary operators. All entities use the same scale.
    float b_w = b->w * ENTITY_SCALE_FACTOR;
    float b_h = b->h * ENTITY_SCALE_FACTOR; 

    for(e = stage.fighterHead.next; e != NULL; e = e->next)
    {
        if(e->side != b->side)
        {
            // CHANGED: No more ternary operators.
            float e_w = e->w * ENTITY_SCALE_FACTOR;
            float e_h = e->h * ENTITY_SCALE_FACTOR;

            if (collision(b->x, b->y, b_w, b_h, e->x, e->y, e_w, e_h))
            {
                b->health = 0;
                e->health = 0;
                addExplosions(b->x, b->y, 10);
                if (e != player) addDebris(e);
                return 1;
            }
        }
    }
    return 0;
}

int bulletHitBullet(Entity *b)
{
    Entity *b2;
    
    // Check this bullet (b) against all other bullets (b2)
    for (b2 = stage.bulletHead.next; b2 != NULL; b2 = b2->next)
    {
        // Don't check against yourself
        if (b == b2)
        {
            continue;
        }

        // Only check if they are on different sides
        if (b->side != b2->side)
        {
            float b_w = b->w * ENTITY_SCALE_FACTOR;
            float b_h = b->h * ENTITY_SCALE_FACTOR;
            float b2_w = b2->w * ENTITY_SCALE_FACTOR;
            float b2_h = b2->h * ENTITY_SCALE_FACTOR;

            if (collision(b->x, b->y, b_w, b_h, b2->x, b2->y, b2_w, b2_h))
            {
                // Hit! Destroy the *other* bullet (b2)
                b2->health = 0; 
                // Return 1 to destroy *this* bullet (b)
                return 1;
            }
        }
    }
    
    return 0; // No collision
}

void fireAlienBullet(Entity *e)
{
    Entity *bullet;
    bullet = (Entity *)malloc(sizeof(Entity));
    memset(bullet, 0, sizeof(Entity));

    stage.bulletTail->next = bullet;
    stage.bulletTail = bullet;

    bullet->health = 1;
    bullet->texture = enemyBulletTexture;
    bullet->side = SIDE_AILEN;

    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

    // --- Get scaled sizes ---
    float e_scaled_w = e->w * ENTITY_SCALE_FACTOR;
    float e_scaled_h = e->h * ENTITY_SCALE_FACTOR;
    float b_scaled_w = bullet->w * ENTITY_SCALE_FACTOR;
    float b_scaled_h = bullet->h * ENTITY_SCALE_FACTOR;

    // 1. Find the alien's scaled center (the spawn point)
    float e_center_x = e->x + (e_scaled_w / 2.0f);
    float e_center_y = e->y + (e_scaled_h / 2.0f);

    // 2. Set the bullet's position (centered on the alien's center)
    bullet->x = e_center_x - (b_scaled_w / 2.0f);
    bullet->y = e_center_y - (b_scaled_h / 2.0f);

    // --- SAFER AIMING LOGIC ---
    
    // Default aim: straight left
    bullet->dx = -ALIEN_BULLET_SPEED;
    bullet->dy = 0;

    // Only aim if the player is valid AND has valid dimensions
    if (player != NULL && player->w > 0 && player->h > 0)
    {
        float p_scaled_w = player->w * ENTITY_SCALE_FACTOR;
        float p_scaled_h = player->h * ENTITY_SCALE_FACTOR;
        
        float p_center_x = player->x + (p_scaled_w / 2.0f);
        float p_center_y = player->y + (p_scaled_h / 2.0f);

        // Calculate slope FROM the alien's center TO the player's center
        calcSlope(p_center_x, p_center_y, e_center_x, e_center_y, &bullet->dx, &bullet->dy);

        bullet->dx *= ALIEN_BULLET_SPEED;
        bullet->dy *= ALIEN_BULLET_SPEED;
    }

    // Add the enemy's own horizontal speed to the bullet's speed
    bullet->dx += e->dx;

    e->reload = (FPS * 2) + (rand() % (FPS * 3));
}

void doBullets(void)
{
    Entity *b, *prev;
    prev = &stage.bulletHead;
    for (b = stage.bulletHead.next; b != NULL; b = b->next)
    {
        b->x += b->dx;
        b->y += b->dy;

        // --- MODIFIED LINE ---
        // Check if it hit a bullet, a fighter, OR is off-screen
        if (bulletHitBullet(b) || bulletHitFighter(b) || b->x < -b->w || b->y < -b->h || b->x > SCREEN_WIDTH || b->y > SCREEN_HEIGHT)
        {
            if (b == stage.bulletTail)
            {
                stage.bulletTail = prev;
            }
            prev->next = b->next;
            free(b);
            b = prev;
        }
        prev = b;
    }
}

void doEnemies(void)
{
    Entity *e;
    if (player == NULL) return;
    for (e = stage.fighterHead.next; e != NULL; e = e->next)
    {
        if (e->side == SIDE_AILEN && --e->reload <= 0 && e->x > player->x )
        {
            fireAlienBullet(e);
        }
    }
}

void spawnEnemies(void)
{
    Entity *enemy;

    if(--enemySpawnTimer <= 0)
    {
        enemy = (Entity*)malloc(sizeof(Entity));
        memset( enemy, 0, sizeof(Entity));

        stage.fighterTail->next = enemy;
        stage.fighterTail = enemy;
        
        enemy->x = SCREEN_WIDTH; // Spawn at the right edge
        enemy->side = SIDE_AILEN;
        enemy->texture = enemyTexture;
        enemy->health = 1;
        enemy->reload = FPS + (rand() % (FPS * 2));
        enemy->dx = -( 2+ (rand() % 4 ));

        SDL_QueryTexture( enemy->texture, NULL,NULL, &enemy->w, &enemy->h);

        int ui_buffer = 50; // 50 pixels of space
        
        float scaled_h = enemy->h * ENTITY_SCALE_FACTOR;
        
        int spawn_range = (int)(SCREEN_HEIGHT - scaled_h - ui_buffer);
       
        enemy->y = ui_buffer + (rand() % spawn_range);
        
        enemySpawnTimer = (FPS * 2) + (rand() % (FPS * 2));
    }
}

void doFighters(void)
{
    Entity *e, *prev;
    prev = &stage.fighterHead;
    for (e = stage.fighterHead.next; e != NULL; e = prev->next)
    {
        e->x += e->dx;
        e->y += e->dy;
        
        float scaled_w = e->w * ENTITY_SCALE_FACTOR;

        if (e->health == 0 || (e != player && e->x < -scaled_w))
        {
            if (e == player)
            {
                player = NULL; // Player is dead
                addExplosions(e->x, e->y, 30); // Big explosion for player

                //check foe new high score
                if( score > highScore)
                {
                    highScore = score;
                    FILE *file = fopen(HIGHSCORE_FILE, "w");
                    if( file != NULL){
                        fprintf(file, "%d", highScore);
                        fclose(file);
                    }
                }
            }
            // --- ADD THIS ELSE BLOCK ---
            else
            {
                // Create a smaller explosion for the enemy
                addExplosions(e->x, e->y, 10);
                score += 100;
            }
            
            if (e == stage.fighterTail)
            {
                stage.fighterTail = prev;
            }

            prev->next = e->next;
            free(e);
        }
        else
        {
            prev = e;
        }
    }
}

void doBackground(void)
{
    if (--backgroundX < -SCREEN_WIDTH)
    {
        backgroundX = 0;
    }
}

void doStarfield(void)
{
    int i;
    for (i = 0; i < MAX_STARS; i++)
    {
        stars[i].x -= stars[i].speed;
        if (stars[i].x < 0)
        {
            stars[i].x = SCREEN_WIDTH + stars[i].x;
        }
    }
}

void doExplosions(void)
{
    Explosion *e, *prev;
    prev = &stage.explosionHead;
    for (e = stage.explosionHead.next; e != NULL; e = prev->next)
    {
        e->x += e->dx;
        e->y += e->dy;

        //apply friction to slow particled down
        e->dx *= 0.98f;
        e->dy *= 0.98f;

        //ADDED: Fade color from White/Yellow to Red
        if (e->g > 0)
        {
            e->g -= 3; // Fade green
        }
        if (e->b > 0)
        {
            e->b -= 3; // Fade blue
        }

        if (--e->a <= 0)
        {
            if (e == stage.explosionTail)
            {
                stage.explosionTail = prev;
            }
            prev->next = e->next;
            free(e);
        }
        else
        {
            prev = e;
        }
    }
}

void doDebris(void)
{
    Debris *d, *prev;
    prev = &stage.debrisHead;
    for (d = stage.debrisHead.next; d != NULL; d = prev->next)
    {
        d->x += d->dx;
        d->y += d->dy;
        d->dy += 0.5;
        if (--d->life <= 0)
        {
            if (d == stage.debrisTail)
            {
                stage.debrisTail = prev;
            }
            prev->next = d->next;
            free(d);
        }
        else
        {
            prev = d;
        }
    }
}

void addExplosions(int x, int y, int num)
{
    Explosion *e;
    int i;
    for (i = 0; i < num; i++)
    {
        e = (Explosion *)malloc(sizeof(Explosion)); // Add cast
        memset(e, 0, sizeof(Explosion));
        stage.explosionTail->next = e;
        stage.explosionTail = e;

        e->x = x + (rand() % 32) - 16;
        e->y = y + (rand() % 32) - 16;
        e->dx = (rand() % 300 - 150) / 10.0f;
        e->dy = (rand() % 300 - 150) / 10.0f;
        e->r = 255;
        e->g = 255;
        if (rand() % 2 == 0)
        {
            e->b = 0; // 50% chance to be Yellow
        }
        else
        {
            e->b = 255; // 50% chance to be White
        }

        // Shorter, more punchy lifespan (0.5 - 1.0 seconds)
        e->a = (FPS / 2) + (rand() % (FPS / 2));
    }
}

void addDebris(Entity *e)
{
    Debris *d;
    int x, y, w, h;
    w = e->w / 2;
    h = e->h / 2;
    float center_x = e->x + (e->w * ENTITY_SCALE_FACTOR) / 2.0f;
    float center_y = e->y + (e->h * ENTITY_SCALE_FACTOR) / 2.0f;

    for (y = 0; y < e->h; y += h)
    {
        for (x = 0; x < e->w; x += w)
        {
            // --- FIXED ---
            d = (Debris *)malloc(sizeof(Debris)); // Add cast
            memset(d, 0, sizeof(Debris));
            stage.debrisTail->next = d;
            stage.debrisTail = d;

            d->x = center_x;
            d->y = center_y;
            d->dx = (rand() % 5) - (rand() % 5);
            d->dy = -(5 + (rand() % 12));
            d->life = FPS * 2;
            d->texture = e->texture;
            d->rect.x = x;
            d->rect.y = y;
            d->rect.w = w;
            d->rect.h = h;
        }
    }
}

void capFrameRate(long *then, float *remainder)
{
    long frameTime, wait;

    // Calculate how long the frame took
    frameTime = SDL_GetTicks() - *then;

    // Calculate how long to wait
    wait = (long)TARGET_MS - frameTime;
    
    // Only delay if we are running too fast
    if (wait > 0)
    {
        SDL_Delay(wait);
    }
    
    // Reset the timer for the next frame
    *then = SDL_GetTicks();
}