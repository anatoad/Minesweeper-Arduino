#include "utils.h"
#include "buzzer.h"

/* Initialize volatile variables used with button interrupts */
volatile bool blueButtonFlag = false;
volatile bool redButtonFlag = false;

volatile unsigned long lastPressRed;
volatile unsigned long lastPressBlue;

volatile bool joystickButtonFlag = false;

/* Short period of time used for button debouncing */
const unsigned long debounceTime = 200;

/* Initialize volatile variable for time limit to be used in timer interrupt */
volatile unsigned long timer = 300;
volatile bool timerFlag = false;

unsigned int minutes, seconds;
unsigned long long buttonPressTime;

/* Cursor position */
coordinate currentPosition = {0, 0};
coordinate lastPosition = {0, 0};

/* Joystick controls */
unsigned long eps = 300;
unsigned long origin_x = 500;
unsigned long origin_y = 500;
unsigned long ox = ORIGIN, oy = ORIGIN;
unsigned long lastOX = ORIGIN, lastOY = ORIGIN;

/* Game board parameters */
bool hidden[8][16];
unsigned int table[8][16] = {0};
unsigned int board[8][16] = {0};

unsigned int num_bombs;
unsigned int hidden_cells = ROWS * COLUMNS;

bool gameWon = false;
bool gameOver = false;

/* Create instance of the Adafruit_ILI9341 display */
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS1, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

/* Initialize registers for Timer 1 to generate interrupts each second */
void init_timer1() {
    /* Reset control registers for Timer 1 */
    TCCR1A = 0;
    TCCR1B = 0;

    /* The prescaler value is 256 -> set the CS12 bit */
    TCCR1B |= (1 << CS12);

    /* Make the comparator trigger Timer/Counter1 input capture interrupt
    -> set the OCIE1A bit in the timer interrupt mask register (TIMSK1). */
    TIMSK1 |= (1 << OCIE1A);

    /* Set the threshold value (timer_count) for Timer 1 */
    OCR1A = TIMER_COUNT;
}

void setup()
{
    Serial.begin(9600);

    randomSeed(analogRead(A3));
    
    /* initialize button pins */
    pinMode(BLUE_BUTTON, INPUT);
    pinMode(RED_BUTTON, INPUT);

    pinMode(BUTTON_INTERRUPT, INPUT);
    attachInterrupt(digitalPinToInterrupt(BUTTON_INTERRUPT), ISR_button, RISING);

    /* initialize joystick pins */
    pinMode(JOYSTICK_X, INPUT);
    pinMode(JOYSTICK_Y, INPUT);

    pinMode(JOYSTICK_INTERRUPT, INPUT);
    digitalWrite(JOYSTICK_INTERRUPT, HIGH);
    attachInterrupt(digitalPinToInterrupt(JOYSTICK_INTERRUPT), ISR_joystick, RISING);

    /* calibrate display */
    tft.begin();
    tft.setRotation(1);

    /* display header text */
    init_display_text();

    /* initialize game board */
    init_game();

    // for debugging
    Serial.println("---------table----------");
    print_board(table);

    /* display board */
    draw_board(board);

    move_cursor();

    // for debugging
    // draw_cells();

    /* Deactivate interrupts */
    cli();

    /* Initialize Timer1 registers */
    init_timer1();

    /* Activate interrupts */
    sei();

    /* VERY IMPORTANT */
    joystickButtonFlag = false;
}

/* Joystick cursor control */
void update_cursor_position() {
    /* Check for movement on the OX axis first */
    if (abs(ox - origin_x) > eps) {
        if (ox < origin_x && currentPosition.y > 0 && lastOY != LEFT) {  /* move left */
            lastPosition = {currentPosition.x, currentPosition.y};
            currentPosition.y--;

            move_cursor();

            lastOY = LEFT;

        } else if (ox > origin_x && currentPosition.y < COLUMNS - 1 && lastOY != RIGHT) {  /* move right */
            lastPosition = {currentPosition.x, currentPosition.y};
            currentPosition.y++;

            move_cursor();

            lastOY = RIGHT;
        }

    } else if (abs(oy - origin_y) > eps) {
        if (oy < origin_y && currentPosition.x > 0 && lastOX != UP) {  /* move up */
            lastPosition = {currentPosition.x, currentPosition.y};
            currentPosition.x--;

            move_cursor();

            lastOX = UP;

        } else if (oy > origin_y && currentPosition.x < ROWS - 1 && lastOX != DOWN) {  /* move down */
            lastPosition = {currentPosition.x, currentPosition.y};
            currentPosition.x++;

            move_cursor();

            lastOX = DOWN;
        }
    }

    /* Update last joystick position - origin */
    if (abs(ox - origin_x) <= eps) {
        lastOY = ORIGIN;
    }

    if (abs(oy - origin_y) <= eps) {
        lastOX = ORIGIN;
    }
}

void loop(void)
{
    /* Restart game */
    if (joystickButtonFlag) {
        /* reset timer */

        /* reset game parameters */
        init_game();

        /* render blank board */
        draw_board(board);

        draw_cursor();

        // for debugging
        Serial.println("---------table----------");
        print_board(table);

        joystickButtonFlag = false;
    
        timer = 300;

        hidden_cells = ROWS * COLUMNS;

        gameWon = false;
        gameOver = false;
    }

    /* if game ended - freeze display */
    if (gameWon) {
        // play_song_game_won();
        display_congrats_message();
        return;
    }

    if (gameOver) {
        play_song_game_over();
        display_game_over_message();
        return;
    }

    /* decrease timer once timerFlag was set in the interrupt */
    if (timerFlag) {
        /* render new timer values */
        display_timer();

        timerFlag = false;
    }

    /* buttons events */
    if (blueButtonFlag) {
        clear_cell({currentPosition.x, currentPosition.y});

        blueButtonFlag = false;
    }

    if (redButtonFlag) {
        flag_cell({currentPosition.x, currentPosition.y});

        redButtonFlag = false;
    }

    /* get joystick position */
    ox = analogRead(JOYSTICK_X);
    oy = analogRead(JOYSTICK_Y);

    if (abs(ox - origin_x) <= eps || abs(oy - origin_y) <= eps) {
        update_cursor_position();
    }
}

/* Interrupt routine for the joystick switch button. */
void ISR_joystick() {
    joystickButtonFlag = true;
}

/* Interrupt routine for the two push buttons. */
void ISR_button() {
    buttonPressTime = millis();
    if (digitalRead(BLUE_BUTTON) && buttonPressTime - lastPressBlue > debounceTime) {
        lastPressBlue = buttonPressTime;
        blueButtonFlag = true;

    } else if (digitalRead(RED_BUTTON) && buttonPressTime - lastPressRed > debounceTime) {
        lastPressRed = buttonPressTime;
        redButtonFlag = true;
    }
}

/* Interrupt routine for Timer 1 - one second fixed time interval. */
ISR(TIMER1_COMPA_vect){
    TCNT1 = 0;   /* Reset counter register */

    timer--;
    timerFlag = true;
}
