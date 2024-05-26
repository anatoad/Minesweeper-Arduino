#ifndef __UTILS_H__
#define __UTILS_H__

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Fonts/FreeMono12pt7b.h"

/* pin configurations */
#define TFT_CLK 13
#define TFT_MISO 12
#define TFT_MOSI 11
#define TFT_DC 9
#define TFT_CS1 10
#define TFT_RST 8

#define BUZZER 6

#define BUTTON_INTERRUPT 3
#define BLUE_BUTTON 4
#define RED_BUTTON 5

#define JOYSTICK_INTERRUPT 2
#define JOYSTICK_X 0
#define JOYSTICK_Y 1

/* timer parameters */
#define CLOCK_FREQUENCY 16000000    /* 16 MHz */
#define INTERRUPT_FREQUENCY 1       /* 1Hz corresponds to 1 second period */
#define PRESCALER 256
#define TIMER_COUNT (CLOCK_FREQUENCY / (PRESCALER * INTERRUPT_FREQUENCY) - 1)

#define BACKGROUND_COLOR ILI9341_MAROON
#define CURSOR_COLOR ILI9341_DARKCYAN

/* board parameters */
#define ROWS 8
#define COLUMNS 16
#define SIZE 20
#define sx 80
#define sy 0

/* joystick control parameters */
#define ORIGIN 0
#define LEFT 500
#define RIGHT 501
#define UP 502
#define DOWN 503

/* cell macros */
#define BLANK 0
#define BOMB 100
#define RED_BOMB 101
#define FLAG 102
#define EMPTY 103
#define ONE 1
#define TWO 2
#define THREE 3
#define FOUR 4
#define FIVE 5
#define SIX 6
#define SEVEN 7
#define EIGHT 8

struct coordinate {
    int x;
    int y;
};

extern volatile bool blueButtonFlag;
extern volatile bool redButtonFlag;
extern volatile bool timerFlag;

extern volatile unsigned long timer;

extern unsigned int minutes, seconds;

extern volatile unsigned long lastPressRed, lastPressBlue;
extern const unsigned long debounceTime;

extern coordinate currentPosition;
extern coordinate lastPosition;

extern unsigned long ox, oy, lastOX, lastOY;

extern bool hidden[ROWS][COLUMNS];
extern unsigned int table[ROWS][COLUMNS];
extern unsigned int board[ROWS][COLUMNS];

extern unsigned int num_bombs;
extern unsigned int hidden_cells;

extern bool gameWon;
extern bool gameOver;

extern Adafruit_ILI9341 tft;

void init_timer1();

void display_timer();

void ISR_joystick();
void ISR_button();

void display_congrats_message();

void display_game_over_message();

void game_won();
void game_over();

void draw_cell(coordinate pos, unsigned int type);
void draw_board(unsigned int (&board)[ROWS][COLUMNS]);
void draw_digit(coordinate pos, const uint8_t *bitmap, uint16_t color);

void draw_cursor();
void move_cursor();
void update_cursor_position();

void init_game();
void init_numbers();
void init_display_text();
void init_random_bombs();

void flag_cell(coordinate pos);
void clear_cell(coordinate pos);

void draw_cells();
void print_board(unsigned int (&board)[ROWS][COLUMNS]);

#endif // __UTILS_H__
