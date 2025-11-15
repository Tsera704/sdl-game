#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "structs.h"
#include "defs.h"
#include "text.h"
#include "draw.h" // <-- CRITICAL: Include its own header

// Global variables
extern App app;
extern Stage stage;
extern Entity *player;
extern SDL_Texture *background;
extern SDL_Texture *explosionTexture;
extern Star stars[MAX_STARS];
extern int backgroundX;

// --- HELPER FUNCTIONS ---

void blit(SDL_Texture* texture, int x, int y)
{
    SDL_Rect dest;
    dest.x = x;
    dest.y = y; 
    SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
    SDL_RenderCopy(app.renderer, texture, NULL, &dest);
}

void blitRect(SDL_Texture *texture, SDL_Rect *src, int x, int y)
{
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    dest.w = src->w;
    dest.h = src->h;
    SDL_RenderCopy(app.renderer, texture, src, &dest);
}

void blit_scaled(SDL_Texture* texture, SDL_Rect *src, int x, int y, float scale_factor)
{
    SDL_Rect dest;
    int original_w, original_h;

    if (src != NULL) {
        original_w = src->w;
        original_h = src->h;
    } else {
        SDL_QueryTexture(texture, NULL, NULL, &original_w, &original_h);
    }

    dest.x = x;
    dest.y = y;
    dest.w = (int)(original_w * scale_factor);
    dest.h = (int)(original_h * scale_factor);
    SDL_RenderCopy(app.renderer, texture, src, &dest);
}

// --- NEW DRAWING FUNCTIONS ---

void drawFighters(void)
{
    Entity *e;
    for (e = stage.fighterHead.next; e != NULL; e = e->next)
    {
        
            blit_scaled(e->texture, NULL, e->x, e->y, ENTITY_SCALE_FACTOR);
        
    }
}

void drawBullets(void)
{
    Entity *b;
    for (b = stage.bulletHead.next; b != NULL; b = b->next)
    {
        blit_scaled(b->texture,NULL, b->x, b->y,ENTITY_SCALE_FACTOR);
    }
}

// --- CORE DRAWING FUNCTIONS ---

void prepareScene(void)
{
    SDL_SetRenderDrawColor(app.renderer, 96, 128, 255, 255);
    SDL_RenderClear(app.renderer);
}

void presentScene(void)
{
    SDL_RenderPresent(app.renderer);
}

void draw(void)
{
    drawBackground();
    drawStarfield();
    drawDebris();
    drawFighters();
    drawBullets();
    drawExplosions();

    char scoreText[32];
    sprintf(scoreText, "Score: %d", score);
    drawText(10,10,255,255,255,FONT_NORMAL,scoreText);

    char highScoreText[32];
    sprintf(highScoreText, "High Score: %d", highScore);
    drawText(1000,10,255,255,255,FONT_NORMAL,highScoreText);
}

void drawBackground(void)
{
    SDL_Rect dest;
    int x;
    for (x = backgroundX; x < SCREEN_WIDTH; x += SCREEN_WIDTH)
    {
        dest.x = x;
        dest.y = 0;
        dest.w = SCREEN_WIDTH;
        dest.h = SCREEN_HEIGHT;
        SDL_RenderCopy(app.renderer, background, NULL, &dest);
    }
}

void drawStarfield(void)
{
    int i, c;
    for (i = 0; i < MAX_STARS; i++)
    {
        c = 32 * stars[i].speed;
        SDL_SetRenderDrawColor(app.renderer, c, c, c, 255);
        SDL_RenderDrawLine(app.renderer, stars[i].x, stars[i].y, stars[i].x + 3, stars[i].y);
    }
}

void drawDebris(void)
{    Debris *d;

    for (d = stage.debrisHead.next; d != NULL; d = d->next)
    {
        // OLD: blitRect(d->texture, &d->rect, d->x, d->y);
        
        // NEW: Use blit_scaled to draw the debris at the correct scale
        blit_scaled(d->texture, &d->rect, d->x, d->y, ENTITY_SCALE_FACTOR);
    }
}

void drawExplosions(void)
{
    Explosion *e;
    SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_ADD);
    SDL_SetTextureBlendMode(explosionTexture, SDL_BLENDMODE_ADD);
    for (e = stage.explosionHead.next; e != NULL; e = e->next)
    {
        SDL_SetTextureColorMod(explosionTexture, e->r, e->g, e->b);
        SDL_SetTextureAlphaMod(explosionTexture, e->a);
        blit(explosionTexture, e->x, e->y);
    }
    SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_NONE);
}

SDL_Texture *loadTexture(const char *filename)
{
    SDL_Texture* texture;
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);
    texture = IMG_LoadTexture(app.renderer, filename);
    if (texture == NULL) {
        SDL_Log("Failed to load texture %s! SDL_image Error: %s", filename, IMG_GetError());
    }
    return texture;
}