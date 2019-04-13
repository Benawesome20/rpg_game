#include "graphics.h"

#include "globals.h"

#include "frames.h"

#include "hardware.h"


void draw_player(int u, int v, int key)
{
    if(!key)
        uLCD.BLIT(u, v, 11, 11, sprite_frames[5]);
    else
        uLCD.BLIT(u, v, 11, 11, sprite_frames[6]);
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
    uLCD.BLIT(u, v, 11, 11, sprite_frames[8]);
}

void draw_plant(int u, int v)
{
    uLCD.BLIT(u , v, 11, 11, sprite_frames[0]);
}

void draw_NPC(int u, int v)
{
    uLCD.BLIT(u , v, 11, 11, sprite_frames[4]);
}

void draw_key(int u, int v)
{
    uLCD.BLIT(u, v, 11, 11, sprite_frames[3]);
}

void draw_door_closed(int u, int v)
{
    uLCD.BLIT(u , v, 11, 11, sprite_frames[1]);
}

void draw_door_open(int u, int v)
{
    uLCD.BLIT(u , v, 11, 11, sprite_frames[2]);
}

void draw_stairs(int u, int v)
{
    uLCD.BLIT(u , v, 11, 11, sprite_frames[7]);
}

void draw_win_item(int u, int v)
{
    uLCD.BLIT(u , v, 11, 11, sprite_frames[9]);
}

void draw_upper_status(int player_x, int player_y)
{
    // Draw bottom border of status bar
    uLCD.line(0, 9, 127, 9, GREEN);

    // Add other status info drawing code here
    char posString[16];
    sprintf(posString, "Position: %u, %u ", player_x, player_y);
    uLCD.text_string(posString, 0, 0, FONT_5X7, YELLOW);
}

void draw_lower_status(int key)
{
    // Draw top border of status bar
    uLCD.line(0, 118, 127, 118, GREEN);

    // Add other status info drawing code here
    if(key)
        draw_key(0, 119);
}

void draw_border()
{
    uLCD.filled_rectangle(0,     9, 127,  14, WHITE); // Top
    uLCD.filled_rectangle(0,    13,   2, 114, WHITE); // Left
    uLCD.filled_rectangle(0,   114, 127, 117, WHITE); // Bottom
    uLCD.filled_rectangle(124,  14, 127, 117, WHITE); // Right
}

void draw_game_over(int win)
{
    // Cover map
    uLCD.filled_rectangle(3, 15, 126, 113, BLACK);

    // Write message
    uLCD.text_string("GAME", 7, 5, FONT_12X16, (win) ? YELLOW : RED);
    uLCD.text_string("OVER", 7, 6, FONT_12X16, (win) ? YELLOW : RED);

    // If the player won, give a winning messsage, otherwise, tell them they lost
    if(win) {
        uLCD.text_string("You win!", 6, 8, FONT_5X7, YELLOW);
        uLCD.text_string("Congratulations!", 2, 9, FONT_5X7, YELLOW);
    }
    else
        uLCD.text_string("YOU DIED", 5, 8, FONT_5X7, RED);
}

void draw_start_page()
{
    // fill in borders
    draw_border();

    // Cover map
    uLCD.filled_rectangle(3, 15, 126, 113, BLACK);

    // Write message
    uLCD.text_string("GT LEGEND", 5, 5, FONT_12X16, BLUE);
    uLCD.text_string("by Benjamin", 4, 6, FONT_5X7, BLUE);
    uLCD.text_string("Ventimiglia", 4, 7, FONT_5X7, BLUE);
    uLCD.text_string("PRESS START", 4, 12, FONT_5X7, BLUE);

    GameInputs in;

    while(in.b1) {
        Timer t; t.start();

        in = read_inputs();

        t.stop();
        int dt = t.read_ms();
        if (dt < 100) wait_ms(100 - dt);
    }

}
