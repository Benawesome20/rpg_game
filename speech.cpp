#include "speech.h"

#include "globals.h"
#include "hardware.h"

/**
 * Draw the speech bubble background.
 */
#define YELLOW 0xFFFF00
#define BLACK 0x00000000
static void draw_speech_bubble();

/**
 * Erase the speech bubble.
 */
static void erase_speech_bubble();

/**
 * Draw a single line of the speech bubble.
 * @param line The text to display
 * @param which If TOP, the first line; if BOTTOM, the second line.
 */
#define TOP    0
#define BOTTOM 1
static void draw_speech_line(const char* line, int which);

/**
 * Delay until it is time to scroll.
 */
static void speech_bubble_wait();

void draw_speech_bubble()
{
    uLCD.rectangle(0, 93, 127, 115, YELLOW);
    uLCD.filled_rectangle(1, 94, 126, 114, BLACK);
}

void erase_speech_bubble()
{
    uLCD.filled_rectangle(0, 93, 127, 115, BLACK);
}

void draw_speech_line(const char* line, int which)
{
    // who decided that text_string should have arbitrary rows and cols??
    uLCD.text_string((char*) line, 1, 12 + which, FONT_7X8, YELLOW);
}

void speech_bubble_wait()
{
}

void speech(const char* line1, const char* line2)
{
    draw_speech_bubble();
    draw_speech_line(line1, TOP);
    draw_speech_line(line2, BOTTOM);
   // speech_bubble_wait();
   // erase_speech_bubble();
}

void long_speech(const char* lines[], int n)
{
}
