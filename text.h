#ifndef TEXT_H
#define TEXT_H

#include <SDL2/SDL.h>

typedef enum
{
    FONT_NORMAL,
    FONT_TITLE,
    FONT_ITALIC
} FontChoice;

void initFont(void);

void drawText(int x, int y, int r, int g, int b, FontChoice fontToUse, const char *text);

void cleanupFont(void);

#endif // TEXT_H