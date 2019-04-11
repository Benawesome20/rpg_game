#include "graphics.h"

#include "globals.h"

#include "frames.h"


void draw_player(int u, int v, int key)
{
    if(!key)
        uLCD.BLIT(u, v, 11, 11, sprite_frames[2]);
    else
        uLCD.BLIT(u, v, 11, 11, sprite_frames[3]);
}

#define YELLOW 0xFFFF00
#define BROWN  0xD2691E
#define DIRT   BROWN
void draw_img(int u, int v, const char* img)
{
    int colors[11*11];
    for (int i = 0; i < 11*11; i++)
    {
        if (img[i] == 'R') colors[i] = RED;
        else if (img[i] == 'Y') colors[i] = YELLOW;
        else if (img[i] == 'G') colors[i] = GREEN;
        else if (img[i] == 'D') colors[i] = DIRT;
        else if (img[i] == '5') colors[i] = LGREY;
        else if (img[i] == '3') colors[i] = DGREY;
        else colors[i] = BLACK;
    }
    uLCD.BLIT(u, v, 11, 11, colors);
    wait_us(250); // Recovery time!
}

void draw_nothing(int u, int v)
{
    // Fill a tile with blackness
    uLCD.filled_rectangle(u, v, u+10, v+10, BLACK);
}

void draw_wall(int u, int v)
{
    uLCD.BLIT(u, v, 11, 11, sprite_frames[4]);
}

void draw_plant(int u, int v)
{
    uLCD.BLIT(u , v, 11, 11, sprite_frames[0]);
}

void draw_NPC(int u, int v)
{
    uLCD.filled_rectangle(u, v, u+10, v+10, RED); // change to sprite
}

void draw_key(int u, int v)
{
    uLCD.BLIT(u, v, 11, 11, sprite_frames[1]);
}

void draw_door(int u, int v)
{
    uLCD.filled_rectangle(u, v, u+10, v+10, BROWN); // change to sprite
}

void draw_upper_status(int player_x, int player_y)
{
    // Draw bottom border of status bar
    uLCD.line(0, 9, 127, 9, GREEN);

    // Add other status info drawing code here
    char* posString;
    sprintf(posString, "Position: %u, %u", player_x, player_y);
    uLCD.text_string(posString, 0, 0, FONT_5X7, YELLOW)
}

void draw_lower_status(int key);
{
    // Draw top border of status bar
    uLCD.line(0, 118, 127, 118, GREEN);

    // Add other status info drawing code here
    if(key)
        draw_key(0, 110);
}

void draw_border()
{
    uLCD.filled_rectangle(0,     9, 127,  14, WHITE); // Top
    uLCD.filled_rectangle(0,    13,   2, 114, WHITE); // Left
    uLCD.filled_rectangle(0,   114, 127, 117, WHITE); // Bottom
    uLCD.filled_rectangle(124,  14, 127, 117, WHITE); // Right
}


