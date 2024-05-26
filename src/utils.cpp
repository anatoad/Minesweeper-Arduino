#include "utils.h"
#include "graphics.h"

/* Print the remaining time on the display.  */
void display_timer() {
    /* remove old timer values */
    tft.fillRect(240, 10, 80, 20, BACKGROUND_COLOR);

    /* display new timer values */
    tft.setCursor(240, 10);
    tft.setTextSize(2);
    tft.setTextColor(ILI9341_YELLOW);

    minutes = timer / 60;
    seconds = timer % 60;

    if (minutes < 10)
        tft.print("0");
    tft.print(minutes);
    tft.print(":");
    if(seconds < 10)
        tft.print("0");
    tft.print(seconds);

    /* if time is up - game over! */
    if (timer == 0) {
        gameOver = true;
        game_over();
    }
}

void display_congrats_message() {
    tft.setTextSize(5);

    tft.setTextColor(ILI9341_MAGENTA);
    tft.setCursor(30, 100);
    tft.println("CONGRATS!");    

    tft.setTextColor(ILI9341_MAROON);

    tft.setCursor(32, 102);
    tft.println("CONGRATS!"); 
}

void game_won() {
    display_congrats_message();

    tft.setTextSize(4);

    tft.setTextColor(ILI9341_MAGENTA);
    tft.setCursor(60, 140);
    tft.println("You won!");

    tft.setTextColor(ILI9341_MAROON);
    tft.setCursor(62, 142);
    tft.println("You won!");
}

void display_game_over_message() {
    tft.setTextSize(5);

    tft.setTextColor(ILI9341_MAGENTA);
    tft.setCursor(30, 100);
    tft.println("GAME OVER");

    tft.setTextColor(ILI9341_MAROON);
    tft.setCursor(32, 102);
    tft.println("GAME OVER");
}

void game_over() {
    display_game_over_message();

    tft.setTextSize(4);

    tft.setTextColor(ILI9341_MAGENTA);
    tft.setCursor(60, 140);
    tft.println("You lost!");

    tft.setTextColor(ILI9341_MAROON);
    tft.setCursor(62, 142);
    tft.println("You lost!");
}

void draw_digit(coordinate pos, const uint8_t *bitmap, uint16_t color) {
    tft.fillRect(sy + pos.y * SIZE, sx + pos.x * SIZE, SIZE, SIZE, color);
    tft.drawRect(sy + pos.y * SIZE, sx + pos.x * SIZE, SIZE, SIZE, BACKGROUND_COLOR);
    tft.drawBitmap(sy + pos.y * SIZE, sx + pos.x * SIZE,  bitmap, SIZE, SIZE, ILI9341_LIGHTGREY);
}

void draw_cell(coordinate pos, unsigned int type) {
    switch (type) {
        case BLANK:
            tft.fillRect(sy + pos.y * SIZE, sx + pos.x * SIZE, SIZE, SIZE, ILI9341_LIGHTGREY);

            tft.fillRect(sy + 1 + pos.y * SIZE, sx + 1 + pos.x * SIZE, 18, 2, ILI9341_WHITE);
            tft.fillRect(sy + 1 + pos.y * SIZE, sx + 1 + pos.x * SIZE, 2, 18, ILI9341_WHITE);

            tft.fillRect(sy + 1 + pos.y * SIZE, sx + 18 + pos.x * SIZE, 18, 2, ILI9341_BLACK);
            tft.fillRect(sy + 18 + pos.y * SIZE, sx + 1 + pos.x * SIZE, 2, 18, ILI9341_BLACK);           

            tft.drawRect(sy + pos.y * SIZE, sx + pos.x * SIZE, SIZE, SIZE, BACKGROUND_COLOR);
            break;
        
        case EMPTY:
            tft.fillRect(sy + pos.y * SIZE, sx + pos.x * SIZE, SIZE, SIZE, ILI9341_LIGHTGREY);
            tft.drawRect(sy + pos.y * SIZE, sx + pos.x * SIZE, SIZE, SIZE, BACKGROUND_COLOR);
            break;

        case BOMB:
            tft.fillRect(sy + pos.y * SIZE, sx + pos.x * SIZE, SIZE, SIZE, ILI9341_LIGHTGREY);
            tft.drawRect(sy + pos.y * SIZE, sx + pos.x * SIZE, SIZE, SIZE, BACKGROUND_COLOR);
            tft.drawBitmap(sy + pos.y * SIZE, sx + pos.x * SIZE, bomb, SIZE, SIZE, ILI9341_BLACK);
            break;

        case RED_BOMB:
            tft.fillRect(sy + pos.y * SIZE, sx + pos.x * SIZE, SIZE, SIZE, ILI9341_RED);
            tft.drawRect(sy + pos.y * SIZE, sx + pos.x * SIZE, SIZE, SIZE, BACKGROUND_COLOR);
            tft.drawBitmap(sy + pos.y * SIZE, sx + pos.x * SIZE, bomb, SIZE, SIZE, ILI9341_BLACK);
            break;

        case FLAG:
            tft.fillRect(sy + pos.y * SIZE, sx + pos.x * SIZE, SIZE, 11, ILI9341_RED);
            tft.fillRect(sy + pos.y * SIZE, sx + 11 + pos.x * SIZE, SIZE, 8, ILI9341_BLACK);
            tft.drawRect(sy + pos.y * SIZE, sx + pos.x * SIZE, SIZE, SIZE, BACKGROUND_COLOR);
            tft.drawBitmap(sy + pos.y * SIZE, sx + pos.x * SIZE, flag, SIZE, SIZE, ILI9341_LIGHTGREY);
            break;
        
        default:
            draw_digit(pos, digits[type], digits_color[type]);
            break;
    }
}

/* Render all board cells. */
void draw_board(unsigned int (&board)[ROWS][COLUMNS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            draw_cell({i, j}, board[i][j]);
        }
    }
}

/* Highlight cursor position on playing board. */
void draw_cursor() {
    tft.drawRect(sy + 1 + currentPosition.y * SIZE, sx + 1 + currentPosition.x * SIZE, 18, 18, CURSOR_COLOR);
    tft.drawRect(sy + 1 + currentPosition.y * SIZE, sx + 1 + currentPosition.x * SIZE, 18, 18, CURSOR_COLOR);

    tft.drawRect(sy + 2 + currentPosition.y * SIZE, sx + 2 + currentPosition.x * SIZE, 16, 18, CURSOR_COLOR);
    tft.drawRect(sy + 2 + currentPosition.y * SIZE, sx + 2 + currentPosition.x * SIZE, 16, 18, CURSOR_COLOR);
}

/* Highlight cursor after position change. */
void move_cursor() {
    /* restore last cell */
    draw_cell({lastPosition.x, lastPosition.y}, board[lastPosition.x][lastPosition.y]);

    /* draw cursor in new cell */
    draw_cursor();
}

/* Initialize a random number of bombs on the playing board. */
void init_random_bombs() {
    unsigned int i, x, y;

    /* pick a random number of bombs between 15 - 25 */
    num_bombs = random(15, 25);

    /* fix bombs coordinates */
    for (i = 0; i < num_bombs; i++) {
        /* generate random coordinates until an empty cell is found */
        while (true) {
            x = random(ROWS);
            y = random(COLUMNS);

            if (table[x][y] == BLANK) {
                table[x][y] = BOMB;
                break;
            }
        }
    }
}

/* Initialize numbers on the playing board - each cell will contain
a digit indicating the number of bombs in adjacent cells. */
void init_numbers() {
    int x, y, i, j, k;

    int dx[] = {-1, -1, -1, 0, 1, 1, 1, 0};
    int dy[] = {-1, 0, 1, 1, 1, 0, -1, -1};

    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLUMNS; j++) {
            if (table[i][j] != BOMB) {
                for (k = 0; k < 8; k++) {
                    x = i + dx[k];
                    y = j + dy[k];

                    if (x >= 0 && x < ROWS && y >= 0 && y < COLUMNS && table[x][y] == BOMB) {
                        table[i][j]++;
                    }
                }
            }

        }
    }
}

/* Print board configuration to the Serial monitor. */
void print_board(unsigned int (&board)[ROWS][COLUMNS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            Serial.print(board[i][j]);
            Serial.print(" ");
        }
        Serial.println();
    }
}

void init_game() {
    /* reset board */
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            hidden[i][j] = true;
            board[i][j] = BLANK;
            table[i][j] = BLANK;
        }
    }

    init_random_bombs();

    init_numbers();

    /* reset cursor position */
    lastPosition = {0, 0};
    currentPosition = {0, 0};
}

void draw_cells() {
    /* bomb */
    tft.fillRect(80, 30, SIZE, SIZE, ILI9341_LIGHTGREY);
    tft.drawRect(80, 30, SIZE, SIZE, BACKGROUND_COLOR);
    tft.drawBitmap(80, 30, bomb, SIZE, SIZE, ILI9341_BLACK);

    /* red bomb */
    tft.fillRect(100, 30, SIZE, SIZE, ILI9341_RED);
    tft.drawRect(100, 30, SIZE, SIZE, BACKGROUND_COLOR);
    tft.drawBitmap(100, 30, bomb, SIZE, SIZE, ILI9341_BLACK);

    /* digits */
    tft.fillRect(120, 30, SIZE, SIZE, ILI9341_BLUE);
    tft.drawRect(120, 30, SIZE, SIZE, BACKGROUND_COLOR);
    tft.drawBitmap(120, 30, one, SIZE, SIZE, ILI9341_LIGHTGREY);

    tft.fillRect(140, 30, SIZE, SIZE, ILI9341_GREEN);
    tft.drawRect(140, 30, SIZE, SIZE, BACKGROUND_COLOR);
    tft.drawBitmap(140, 30, two, SIZE, SIZE, ILI9341_LIGHTGREY);

    tft.fillRect(160, 30, SIZE, SIZE, ILI9341_RED);
    tft.drawRect(160, 30, SIZE, SIZE, BACKGROUND_COLOR);
    tft.drawBitmap(160, 30, three, SIZE, SIZE, ILI9341_LIGHTGREY);

    tft.fillRect(180, 30, SIZE, SIZE, ILI9341_PURPLE);
    tft.drawRect(180, 30, SIZE, SIZE, BACKGROUND_COLOR);
    tft.drawBitmap(180, 30, four, SIZE, SIZE, ILI9341_LIGHTGREY);

    tft.fillRect(200, 30, SIZE, SIZE, ILI9341_GREENYELLOW);
    tft.drawRect(200, 30, SIZE, SIZE, BACKGROUND_COLOR);
    tft.drawBitmap(200, 30, five, SIZE, SIZE, ILI9341_LIGHTGREY);

    /* flag */
    tft.fillRect(80, 50, SIZE, 11, ILI9341_RED);
    tft.fillRect(80, 62, SIZE, 8, ILI9341_BLACK);
    tft.drawRect(80, 50, SIZE, SIZE, BACKGROUND_COLOR);
    tft.drawBitmap(80, 50, flag, SIZE, SIZE, ILI9341_LIGHTGREY);

    /* blank cell */
    tft.fillRect(100, 50, SIZE, SIZE, ILI9341_LIGHTGREY);

    tft.fillRect(100 + 1, 50 + 1, 18, 2, ILI9341_WHITE);
    tft.fillRect(100 + 1, 50 + 1, 2, 18, ILI9341_WHITE);

    tft.fillRect(100 + 18, 50 + 1, 2, 18, ILI9341_BLACK);
    tft.fillRect(100 + 1, 50 + 18, 18, 2, ILI9341_BLACK);
    tft.drawRect(100, 50, SIZE, SIZE, BACKGROUND_COLOR);

    tft.fillRect(120, 50, SIZE, SIZE, ILI9341_LIGHTGREY);
    tft.drawRect(120, 50, SIZE, SIZE, BACKGROUND_COLOR);

    /* digits */
    tft.fillRect(140, 50, SIZE, SIZE, ILI9341_CYAN);
    tft.drawRect(140, 50, SIZE, SIZE, BACKGROUND_COLOR);
    tft.drawBitmap(140, 50, six, SIZE, SIZE, ILI9341_LIGHTGREY);

    tft.fillRect(160, 50, SIZE, SIZE, ILI9341_PINK);
    tft.drawRect(160, 50, SIZE, SIZE, BACKGROUND_COLOR);
    tft.drawBitmap(160, 50, seven, SIZE, SIZE, ILI9341_LIGHTGREY);

    tft.fillRect(180, 50, SIZE, SIZE, ILI9341_MAROON);
    tft.drawRect(180, 50, SIZE, SIZE, BACKGROUND_COLOR);
    tft.drawBitmap(180, 50, eight, SIZE, SIZE, ILI9341_LIGHTGREY);
}

void flag_cell(coordinate pos) {
    if (hidden[pos.x][pos.y]) {
        /* check if flag is already set, remove if so */
        if (board[pos.x][pos.y] == FLAG) {
            board[pos.x][pos.y] = BLANK;
        } else {
            board[pos.x][pos.y] = FLAG;
        }

        draw_cell({pos.x, pos.y}, board[pos.x][pos.y]);

        draw_cursor();
    }
}

void clear_cell(coordinate pos) {
    if (!hidden[pos.x][pos.y]) {
        return;
    }

    if (table[pos.x][pos.y] == BOMB) {
        board[pos.x][pos.y] = RED_BOMB;

        draw_cell({pos.x, pos.y}, RED_BOMB);

        game_over();

        gameOver = true;

        return;
    }

    if (table[pos.x][pos.y] == BLANK) {
        /* reveal all adjacent blank cells */
        coordinate point;
        coordinate queue[ROWS * COLUMNS];
        int x, y, p = 0, size = 0, i;

        int dx[] = {-1, -1, -1, 0, 1, 1, 1, 0};
        int dy[] = {-1, 0, 1, 1, 1, 0, -1, -1};

        queue[size++] = {pos.x, pos.y};
        hidden[pos.x][pos.y] = false;

        while (p < size) {
            point = {queue[p].x, queue[p].y};

            if (table[point.x][point.y] == BLANK) {
                board[point.x][point.y] = EMPTY;

                hidden_cells--;

                draw_cell({point.x, point.y}, board[point.x][point.y]);

                for (i = 0; i < 8; i++) {
                    x = point.x + dx[i];
                    y = point.y + dy[i];

                    if (x >= 0 && x < ROWS && y >= 0 && y < COLUMNS && hidden[x][y] && table[x][y] <= 8) {
                        queue[size++] = {x, y};
                        hidden[x][y] = false;
                    }
                }

            } else if (table[point.x][point.y] <= 8) {
                board[point.x][point.y] = table[point.x][point.y];
                hidden[point.x][point.y] = false;

                hidden_cells--;

                draw_cell({point.x, point.y}, board[point.x][point.y]);
            }

            p++;
        }

        draw_cursor();

    } else {
        /* reveal a single cell */
        board[pos.x][pos.y] = table[pos.x][pos.y];

        hidden[pos.x][pos.y] = false;

        hidden_cells--;

        draw_cell({pos.x, pos.y}, board[pos.x][pos.y]);

        draw_cursor();
    }

    Serial.println(hidden_cells);

    /* check if game is won */
    if (hidden_cells == num_bombs) {
        gameWon = true;
        game_won();
    }
        
}

void init_display_text() {
    tft.fillScreen(BACKGROUND_COLOR);
    
    tft.setCursor(0, 0);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(3);
    tft.println("Minesweeper");

    tft.fillCircle(10, 35, 5, ILI9341_BLACK);
    tft.setCursor(20, 32);
    tft.setTextColor(ILI9341_BLACK);
    tft.setTextSize(1);
    tft.println("RESET");

    tft.fillCircle(10, 50, 5, ILI9341_RED);
    tft.setCursor(20, 47);
    tft.setTextColor(ILI9341_RED);
    tft.setTextSize(1);
    tft.println("FLAG");

    tft.fillCircle(10, 65, 5, ILI9341_BLUE);
    tft.setCursor(20, 62);
    tft.setTextColor(ILI9341_BLUE);
    tft.setTextSize(1);
    tft.println("CLEAR");
}
