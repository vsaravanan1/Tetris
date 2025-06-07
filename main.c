#include "pico/stdlib.h"
#include "hardware/timer.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "ssd1306.h"
#include <math.h>
#include <stdio.h>


#define LEFT 8
#define RIGHT 9
#define ROTATE 10
#define RESET 11


static uint8_t num[8][16];
static bool perm[8][16];
int limit = 0;
int position = 15;
int horizontal = 5;
bool goLeft = false;
bool goRight = false;
int angle = 0;
int lastAngle = 0;
bool rotate = false;
char* pieces[7] = {"l", "j", "z", "s", "t", "o", "i"};
char* currentPiece = "l";
bool gameOver = false;
int score = 0;
absolute_time_t startTime;
absolute_time_t currentTime;



void clearRows() {
    int writeRow = 0;
    for (int readRow = 0; readRow < 16; readRow++) {
        bool fullRow = true;
        for (int col = 0; col < 8; col++) {
            if (!perm[col][readRow]) {
                fullRow = false;
                break;
            }
        }

        if (fullRow) {
            score += 100;
        }


        if (!fullRow) {
            if (readRow != writeRow) {
                for (int col = 0; col < 8; col++) {
                    perm[col][writeRow] = perm[col][readRow];
                    num[col][writeRow] = num[col][readRow];
                }
            }
            writeRow++;
        }
    }

    for (int i = writeRow; i < 16; i++) {
        for (int j = 0; j < 8; j++) {
            perm[j][i] = 0;
            num[j][i] = 0;
        }
    }
}

void resetPosition(char c[]) {
    if (strcmp(c, "l") == 0) {
        horizontal = 5;
        position = 15;
    }
    else if (strcmp(c, "j") == 0 && (angle == 0 || angle == 90)) {
        position = 15;
        horizontal = 3;
    }
    else if (strcmp(c, "j") == 0 && (angle == 180 || angle == 270)) {
        position = 15;
        horizontal = 4;
    }
    else if (strcmp(c, "z") == 0) {
        position = 15;
        horizontal = 3;
    }
    else if (strcmp(c, "s") == 0) {
        position = 15;
        horizontal = 4;
    }
    else if (strcmp(c, "t") == 0) {
        position = 15;
        horizontal = 3;
    }
    else if (strcmp(c, "o") == 0) {
        position = 15;
        horizontal = 4;
    }
    else if (strcmp(c, "i") == 0) {
        position = 15;
        horizontal = 4;
    }
}

int getLimit(int x, int y, char c[]) {
    int lim;
    if (strcmp(c, "j") == 0 || strcmp(c, "l") == 0) {
        if (angle == 90 || angle == 270) {
            lim = 1;
        }
        else {
            lim = 0;
        }
    }
    if (strcmp(c, "z") == 0 && (angle == 0 || angle == 180)) {
        lim = 0;
    }
    if (strcmp(c, "z") == 0 && (angle == 90 || angle == 270)) {
        lim = 1;
    }
    if (strcmp(c, "s") == 0 && (angle == 0 || angle == 180)) {
        lim = 0;
    }
    if (strcmp(c, "s") == 0 && (angle == 90 || angle == 270)) {
        lim = 1;
    }
    if (strcmp(c, "t") == 0 && (angle == 0 || angle == 180)) {
        lim = 0;
    }
    if (strcmp(c, "t") == 0 && (angle == 90 || angle == 270)) {
        lim = 1;
    }
    if (strcmp(c, "o") == 0) {
        lim = 0;
    }
    if (strcmp(c, "i") == 0 && (angle == 0 || angle == 180)) {
        lim = -1;
    }
    if (strcmp(c, "i") == 0 && (angle == 90 || angle == 270)) {
        lim = 2;
    }

    if (strcmp(c, "l") == 0 && angle == 0) {
        for (int i = y; i >= 1; i--) {
            if (x >= 2 && i >= 1 && i < 16) {
                if ((num[x][i] == 0xFF && perm[x][i]) ||
                    (num[x-1][i] == 0xFF && perm[x-1][i]) ||
                    (num[x-2][i] == 0xFF && perm[x-2][i]) ||
                    (num[x-2][i-1] == 0xFF && perm[x-2][i-1])) {
                    lim = i;
                    break;
                    }
            }
        }
    }
    else if (strcmp(c, "l") == 0 && angle == 90) {
        for (int i = y; i >= 2; i--) {
            if (x >= 2 && i >= 2 && i < 15) {
                if ((num[x-2][i-1] == 0xFF && perm[x-2][i-1]) ||
                    (num[x-2][i-2] == 0xFF && perm[x-2][i-2]) ||
                    (num[x-2][i] == 0xFF && perm[x-2][i]) ||
                    (num[x-1][i-2] == 0xFF && perm[x-1][i-2])) {
                    lim = i;
                    break;
                    }
            }
        }
    }
    else if (strcmp(c, "l") == 0 && angle == 180) {
        for (int i = y; i >= 1; i--) {
            if (x >= 2 && i >= 1 && i < 16) {
                if ((num[x][i] == 0xFF && perm[x][i]) ||
                    (num[x][i-1] == 0xFF && perm[x][i-1]) ||
                    (num[x-1][i-1] == 0xFF && perm[x-1][i-1]) ||
                    (num[x-2][i-1] == 0xFF && perm[x-2][i-1])) {
                    lim = i;
                    break;
                    }
            }
        }
    }
    else if (strcmp(c, "l") == 0 && angle == 270) {
        for (int i = y; i >= 2; i--) {
            if (x >= 2 && i >= 2 && i < 16) {
                if ((num[x-2][i] == 0xFF && perm[x-2][i]) ||
                    (num[x-1][i] == 0xFF && perm[x-1][i]) ||
                    (num[x-1][i-1] == 0xFF && perm[x-1][i-1]) ||
                    (num[x-1][i-2] == 0xFF && perm[x-1][i-2])) {
                    lim = i;
                    break;
                    }
            }
        }
    }
    else if (strcmp(c, "j") == 0 && angle == 0) {
        for (int i = y; i >= 1; i--) {
            if (x >= 0 && i < 16 && i >= 1) {
                if ((num[x+1][i-1] == 0xFF && perm[x+1][i-1]) ||
                    (num[x][i] == 0xFF && perm[x][i]) ||
                    (num[x][i-1] == 0xFF && perm[x][i-1]) ||
                    (num[x+2][i-1] == 0xFF && perm[x+2][i-1])) {
                    lim = i;
                    break;
                }
            }
        }
    }
    else if (strcmp(c, "j") == 0 && angle == 90) {
        for (int i = y; i >= 2; i--) {
            if ((num[x][i] == 0xFF && perm[x][i]) ||
                (num[x+1][i] == 0xFF && perm[x+1][i]) ||
                (num[x][i-1] == 0xFF && perm[x][i-1]) ||
                (num[x][i-2] == 0xFF && perm[x][i-2])) {
                lim = i;
                break;
            }
        }
    }
    else if (strcmp(c, "j") == 0 && angle == 180) {
        for (int i = y; i >= 1; i--) {
            if ((num[x][i] == 0xFF && perm[x][i]) ||
                (num[x][i-1] == 0xFF && perm[x][i-1]) ||
                (num[x-1][i] == 0xFF && perm[x-1][i]) ||
                (num[x-2][i] == 0xFF && perm[x-2][i])) {
                lim = i;
                break;
            }
        }
    }
    else if (strcmp(c, "j") == 0 && angle == 270) {
        for (int i = y; i >= 2; i--) {
            if ((num[x][i] == 0xFF && perm[x][i]) ||
                (num[x][i-1] == 0xFF && perm[x][i-1]) ||
                (num[x][i-2] == 0xFF && perm[x][i-2]) ||
                (num[x-1][i-2] == 0xFF && perm[x-1][i-2])) {
                lim = i;
                break;
            }
        }
    }
    else if (strcmp(c, "z") == 0 && (angle == 0 || angle == 180)) {
        for (int i = y; i >= 1; i--) {
            if ((num[x][i] == 0xFF && perm[x][i]) ||
                (num[x+1][i] == 0xFF && perm[x+1][i]) ||
                (num[x+1][i-1] == 0xFF && perm[x+1][i-1]) ||
                (num[x+2][i-1] == 0xFF && perm[x+2][i-1])) {
                lim = i;
                break;
            }
        }
    }
    else if (strcmp(c, "z") == 0 && (angle == 90 || angle == 270)) {
        for (int i = y; i >= 2; i--) {
            if ((num[x][i] == 0xFF && perm[x][i]) ||
                (num[x][i-1] == 0xFF && perm[x][i-1]) ||
                (num[x-1][i-1] == 0xFF && perm[x-1][i-1]) ||
                (num[x-1][i-2] == 0xFF && perm[x-1][i-2])) {
                lim = i;
                break;
            }
        }
    }
    else if (strcmp(c, "s") == 0 && (angle == 0 || angle == 180)) {
        for (int i = y; i >= 1; i--) {
            if ((num[x][i] == 0xFF && perm[x][i]) ||
                (num[x-1][i] == 0xFF && perm[x-1][i]) ||
                (num[x-1][i-1] == 0xFF && perm[x-1][i-1]) ||
                (num[x-2][i-1] == 0xFF && perm[x-2][i-1])) {
                lim = i;
                break;
            }
        }
    }
    else if (strcmp(c, "s") == 0 && (angle == 90 || angle == 270)) {
        for (int i = y; i >= 2; i--) {
            if ((num[x][i] == 0xFF && perm[x][i]) ||
                (num[x][i-1] == 0xFF && perm[x][i-1]) ||
                (num[x+1][i-1] == 0xFF && perm[x+1][i-1]) ||
                (num[x+1][i-2] == 0xFF && perm[x+1][i-2])) {
                lim = i;
                break;
            }
        }
    }
    else if (strcmp(c, "t") == 0 && angle == 0) {
        for (int i = y; i >= 1; i--) {
            if ((num[x][i] == 0xFF && perm[x][i]) ||
                (num[x+1][i] == 0xFF && perm[x+1][i]) ||
                (num[x-1][i] == 0xFF && perm[x-1][i]) ||
                (num[x][i-1] == 0xFF && perm[x][i-1])) {
                lim = i;
                break;
            }
        }
    }
    else if (strcmp(c, "t") == 0 && angle == 90) {
        for (int i = y; i >= 2; i--) {
            if ((num[x][i] == 0xFF && perm[x][i]) ||
                (num[x][i-1] == 0xFF && perm[x][i-1]) ||
                (num[x][i-2] == 0xFF && perm[x][i-2]) ||
                (num[x-1][i-1] == 0xFF && perm[x-1][i-1])) {
                lim = i;
                break;
            }
        }
    }
    else if (strcmp(c, "t") == 0 && angle == 180) {
        for (int i = y; i >= 1; i--) {
            if ((num[x][i] == 0xFF && perm[x][i]) ||
                (num[x][i-1] == 0xFF && perm[x][i-1]) ||
                (num[x+1][i-1] == 0xFF && perm[x+1][i-1]) ||
                (num[x-1][i-1] == 0xFF && perm[x-1][i-1])) {
                lim = i;
                break;
            }
        }
    }
    else if (strcmp(c, "t") == 0 && angle == 270) {
        for (int i = y; i >= 2; i--) {
            if ((num[x][i] == 0xFF && perm[x][i]) ||
                (num[x][i-1] == 0xFF && perm[x][i-1]) ||
                (num[x][i-2] == 0xFF && perm[x][i-2]) ||
                (num[x+1][i-1] == 0xFF && perm[x+1][i-1])) {
                lim = i;
                break;
            }
        }
    }
    else if (strcmp(c, "o") == 0) {
        for (int i = y; i >= 1; i--) {
            if ((num[x][i] == 0xFF && perm[x][i]) ||
                (num[x][i-1] == 0xFF && perm[x][i-1]) ||
                (num[x-1][i] == 0xFF && perm[x-1][i]) ||
                (num[x-1][i-1] == 0xFF && perm[x-1][i-1])) {
                lim = i;
                break;
            }
        }
    }
    else if (strcmp(c, "i") == 0 && (angle == 0 || angle == 180)) {
        for (int i = y; i >= 0; i--) {
            if ((num[x][i] == 0xFF && perm[x][i]) ||
                (num[x+1][i] == 0xFF && perm[x+1][i]) ||
                (num[x-1][i] == 0xFF && perm[x-1][i]) ||
                (num[x-2][i] == 0xFF && perm[x-2][i])) {
                lim = i;
                break;
            }
        }
    }
    else if (strcmp(c, "i") == 0 && (angle == 90 || angle == 270)) {
        for (int i = y; i >= 3; i--) {
            if ((num[x][i] == 0xFF && perm[x][i]) ||
                (num[x][i-1] == 0xFF && perm[x][i-1]) ||
                (num[x][i-2] == 0xFF && perm[x][i-2]) ||
                (num[x][i-3] == 0xFF && perm[x][i-3])) {
                lim = i;
                break;
            }
        }
    }


    return lim;
}

void restart() {
    ssd1306_clear();
    resetPosition(currentPiece);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 16; j++) {
            num[i][j] = 0;
            perm[i][j] = 0;
        }
    }
}


void initializePiece(char c[]) {
    if (angle == 0 && strcmp(c, "l") == 0) {
        if (!(horizontal >= 2 && position >= 1 && position < 16 && num[horizontal][position] == 0 && num[horizontal-1][position] == 0 &&
        num[horizontal-2][position] == 0 && num[horizontal-2][position-1] == 0)) {
            angle = lastAngle;
        }
    }
    else if (angle == 90 && strcmp(c, "l") == 0) {
        if (!(horizontal >= 2 && position >= 2 && position < 15 && num[horizontal-2][position-1] == 0 && num[horizontal-2][position-2] == 0 &&
        num[horizontal-2][position] == 0 && num[horizontal-1][position-2] == 0)) {
            angle = lastAngle;
        }
    }
    else if (angle == 180 && strcmp(c, "l") == 0) {
        if (!(horizontal >= 2 && position >= 1 && position < 16 && num[horizontal][position] == 0 && num[horizontal][position-1] == 0 &&
                num[horizontal-1][position-1] == 0 && num[horizontal-2][position-1] == 0)) {
            angle = lastAngle;
        }
    }
    else if (angle == 270 && strcmp(c, "l") == 0) {
        if (!(horizontal >= 2 && position >= 2 && position < 16 && num[horizontal-1][position] == 0 && num[horizontal-2][position] == 0 &&
                num[horizontal-1][position-1] == 0 && num[horizontal-1][position-2] == 0)) {
            angle = lastAngle;
        }
    }
    else if (angle == 0 && strcmp(c, "j") == 0) {
        if (!(horizontal <= 5 && position >= 1 && position < 16 &&num[horizontal][position] == 0 && num[horizontal][position - 1] == 0 &&
                num[horizontal+1][position - 1] == 0 && num[horizontal + 2][position - 1] == 0)) {
            angle = lastAngle;
        }
    }
    else if (angle == 90 && strcmp(c, "j") == 0) {
        if (!(num[horizontal][position] == 0 && num[horizontal+1][position] == 0 && num[horizontal][position-1] == 0
            && num[horizontal][position-2] == 0)) {
            angle = lastAngle;
        }
    }
    else if (angle == 180 && strcmp(c, "j") == 0) {
        if (!(num[horizontal][position] == 0 && num[horizontal][position-1] == 0 &&
            num[horizontal-1][position] == 0 && num[horizontal-2][position] == 0)) {
            angle = lastAngle;
        }
    }
    else if (angle == 270 && strcmp(c, "j") == 0) {
        if (!(num[horizontal][position] == 0 && num[horizontal][position-1] == 0 &&
            num[horizontal][position-2] == 0 && num[horizontal-1][position-2] == 0)) {
            angle = lastAngle;
        }
    }
    else if ((angle == 0 || angle == 180) && strcmp(c, "z") == 0) {
        if (!(horizontal >= 0 && horizontal < 6 && num[horizontal][position] == 0 && num[horizontal+1][position] == 0 && num[horizontal+1][position-1] == 0 && num[horizontal+2][position-1] == 0)) {
            angle = lastAngle;
        }
    }
    else if ((angle == 90 || angle == 270) && strcmp(c, "z") == 0) {
        if (!(horizontal >= 1 && horizontal < 8 && num[horizontal][position] == 0 && num[horizontal][position-1] == 0 && num[horizontal-1][position-1] == 0 && num[horizontal-1][position-2] == 0)) {
            angle = lastAngle;
        }
    }
    else if ((angle == 0 || angle == 180) && strcmp(c, "s") == 0) {
        if (!(horizontal >= 2 && horizontal < 8 && num[horizontal][position] == 0 && num[horizontal-1][position] == 0 && num[horizontal-1][position-1] == 0 && num[horizontal-2][position-1] == 0)) {
            angle = lastAngle;
        }
    }
    else if ((angle == 90 || angle == 270) && strcmp(c, "s") == 0) {
        if (!(horizontal >= 1 && horizontal < 7 && position >= 2 && num[horizontal][position] == 0 && num[horizontal][position-1] == 0 && num[horizontal+1][position-1] == 0 && num[horizontal+1][position-2] == 0)) {
            angle = lastAngle;
        }
    }
    else if (angle == 0 && strcmp(c, "t") == 0) {
        if (!(horizontal >= 1 && horizontal < 7 && position >= 1 && num[horizontal][position] == 0 && num[horizontal-1][position] == 0 && num[horizontal+1][position] == 0 && num[horizontal][position-1] == 0 )) {
            angle = lastAngle;
        }
    }
    else if (angle == 90 && strcmp(c, "t") == 0) {
        if (!(horizontal >= 1 && horizontal < 8 && position >= 2 && num[horizontal-1][position-1] == 0 && num[horizontal][position] == 0 && num[horizontal][position-1] == 0 && num[horizontal][position-2] == 0)) {
            angle = lastAngle;
        }

    }
    else if (angle == 180 && strcmp(c, "t") == 0) {
        if (!(horizontal >= 1 && horizontal < 7 && position >= 1 && num[horizontal][position] == 0 && num[horizontal][position-1] == 0 && num[horizontal+1][position-1] == 0 && num[horizontal-1][position-1] == 0)) {
            angle = lastAngle;
        }
    }
    else if (angle == 270 && strcmp(c, "t") == 0) {
        if (!(horizontal < 7 && position >= 2 && num[horizontal][position] == 0 && num[horizontal][position-1] == 0 && num[horizontal][position-2] == 0 && num[horizontal+1][position-1] == 0)) {
            angle = lastAngle;
        }
    }
    else if (strcmp(c, "o") == 0) {
        if (!(horizontal <= 7 && horizontal >= 1 && position >= 1 && num[horizontal][position] == 0 && num[horizontal-1][position] == 0 && num[horizontal][position-1] == 0 && num[horizontal-1][position-1] == 0)) {
            angle = lastAngle;
        }
    }
    else if ((angle == 0 || angle == 180) && strcmp(c, "i") == 0) {
        if (!(horizontal < 7 && horizontal >= 2 && position >= 0 && num[horizontal][position] == 0 && num[horizontal-1][position] == 0 && num[horizontal-2][position] == 0 && num[horizontal+1][position] == 0)) {
            angle = lastAngle;
        }
    }
    else if ((angle == 90 || angle == 270) && strcmp(c, "i") == 0) {
        if (!(horizontal <= 7 && horizontal >= 0 && position >= 3 && num[horizontal][position] == 0 && num[horizontal][position-1] == 0 && num[horizontal][position-2] == 0 && num[horizontal][position-3] == 0)) {
            angle = lastAngle;
        }
    }

    if (angle == 0 && strcmp(c, "l") == 0) {
        if (horizontal >= 2 && position >= 1 && position < 16) {
            if (num[horizontal][position] == 0 && num[horizontal-1][position] == 0 &&
                num[horizontal-2][position] == 0 && num[horizontal-2][position-1] == 0) {
                num[horizontal][position] = 0xFF;
                num[horizontal - 1][position] = 0xFF;
                num[horizontal - 2][position] = 0xFF;
                num[horizontal - 2][position-1] = 0xFF;
            }
            else {
                gameOver = true;
                startTime = get_absolute_time();
                restart();
            }
        }
    }
    else if (angle == 90 && strcmp(c, "l") == 0) {
        if (horizontal >= 2 && position >= 2 && position < 15) {
            if (num[horizontal-2][position-1] == 0 && num[horizontal-2][position-2] == 0 &&
                num[horizontal-2][position] == 0 && num[horizontal-1][position-2] == 0) {
                num[horizontal-2][position-1] = 0xFF;
                num[horizontal-2][position-2] = 0xFF;
                num[horizontal-2][position] = 0xFF;
                num[horizontal-1][position-2] = 0xFF;
            }
            else {
                gameOver = true;
                startTime = get_absolute_time();
                restart();
            }
        }
    }
    else if (angle == 180 && strcmp(c, "l") == 0) {
        if (horizontal >= 2 && position >= 1 && position < 16) {
            if (num[horizontal][position] == 0 && num[horizontal][position-1] == 0 &&
                num[horizontal-1][position-1] == 0 && num[horizontal-2][position-1] == 0) {
                num[horizontal][position] = 0xFF;
                num[horizontal][position-1] = 0xFF;
                num[horizontal - 1][position-1] = 0xFF;
                num[horizontal - 2][position-1] = 0xFF;
            }
            else {
                gameOver = true;
                startTime = get_absolute_time();
                restart();
            }
        }
    }
    else if (angle == 270 && strcmp(c, "l") == 0) {
        if (horizontal >= 2 && position >= 2 && position < 16) {
            if (num[horizontal-1][position] == 0 && num[horizontal-2][position] == 0 &&
                num[horizontal-1][position-1] == 0 && num[horizontal-1][position-2] == 0) {
                num[horizontal-1][position] = 0xFF;
                num[horizontal-2][position] = 0xFF;
                num[horizontal-1][position-1] = 0xFF;
                num[horizontal-1][position-2] = 0xFF;
            }
            else {
                gameOver = true;
                startTime = get_absolute_time();
                restart();
            }
        }
    }
    else if (angle == 0 && strcmp(c, "j") == 0) {
        if (horizontal <= 5 && position >= 1 && position < 16) {
            if (num[horizontal][position] == 0 && num[horizontal][position - 1] == 0 &&
                num[horizontal+1][position - 1] == 0 && num[horizontal + 2][position - 1] == 0) {
                num[horizontal][position] = 0xFF;
                num[horizontal][position-1] = 0xFF;
                num[horizontal+1][position-1] = 0xFF;
                num[horizontal+2][position-1] = 0xFF;
            }
            else {
                gameOver = true;
                startTime = get_absolute_time();
                restart();
            }
        }
    }
    else if (angle == 90 && strcmp(c, "j") == 0) {
        if (num[horizontal][position] == 0 && num[horizontal+1][position] == 0 && num[horizontal][position-1] == 0
            && num[horizontal][position-2] == 0) {
            num[horizontal][position] = 0xFF;
            num[horizontal+1][position] = 0xFF;
            num[horizontal][position-1] = 0xFF;
            num[horizontal][position-2] = 0xFF;
        }
        else {
            gameOver = true;
            startTime = get_absolute_time();
            restart();
        }
    }
    else if (angle == 180 && strcmp(c, "j") == 0) {
        if (num[horizontal][position] == 0 && num[horizontal][position-1] == 0 &&
            num[horizontal-1][position] == 0 && num[horizontal-2][position] == 0) {
            num[horizontal][position] = 0xFF;
            num[horizontal][position-1] = 0xFF;
            num[horizontal-1][position] = 0xFF;
            num[horizontal-2][position] = 0xFF;
        }
        else {
            gameOver = true;
            startTime = get_absolute_time();
            restart();
        }
    }
    else if (angle == 270 && strcmp(c, "j") == 0) {
        if (num[horizontal][position] == 0 && num[horizontal][position-1] == 0 &&
            num[horizontal][position-2] == 0 && num[horizontal-1][position-2] == 0) {
            num[horizontal][position] = 0xFF;
            num[horizontal][position-1] = 0xFF;
            num[horizontal][position-2] = 0xFF;
            num[horizontal-1][position-2] = 0xFF;
        }
        else {
            gameOver = true;
            startTime = get_absolute_time();
            restart();
        }
    }
    else if ((angle == 0 || angle == 180) && strcmp(c, "z") == 0) {
        if (num[horizontal][position] == 0 && num[horizontal+1][position] == 0 && num[horizontal+1][position-1] == 0 && num[horizontal+2][position-1] == 0) {
            num[horizontal][position] = 0xFF;
            num[horizontal+1][position] = 0xFF;
            num[horizontal+1][position-1] = 0xFF;
            num[horizontal+2][position-1] = 0xFF;
        }
        else {
            gameOver = true;
            startTime = get_absolute_time();
            restart();
        }
    }
    else if ((angle == 90 || angle == 270) && strcmp(c, "z") == 0) {
        if (num[horizontal][position] == 0 && num[horizontal][position-1] == 0 && num[horizontal-1][position-1] == 0 && num[horizontal-1][position-2] == 0) {
            num[horizontal][position] = 0xFF;
            num[horizontal][position - 1] = 0xFF;
            num[horizontal-1][position-1] = 0xFF;
            num[horizontal-1][position-2] = 0xFF;
        }
        else {
            gameOver = true;
            startTime = get_absolute_time();
            restart();
        }
    }
    else if ((angle == 0 || angle == 180) && strcmp(c, "s") == 0) {
        if (num[horizontal][position] == 0 && num[horizontal-1][position] == 0 && num[horizontal-1][position-1] == 0 && num[horizontal-2][position-1] == 0) {
            num[horizontal][position] = 0xFF;
            num[horizontal-1][position] = 0xFF;
            num[horizontal-1][position-1] = 0xFF;
            num[horizontal-2][position-1] = 0xFF;
        }
        else {
            gameOver = true;
            startTime = get_absolute_time();
            restart();
        }
    }
    else if ((angle == 90 || angle == 270) && strcmp(c, "s") == 0) {
        if (num[horizontal][position] == 0 && num[horizontal][position-1] == 0 && num[horizontal+1][position-1] == 0 && num[horizontal+1][position-2] == 0) {
            num[horizontal][position] = 0xFF;
            num[horizontal][position-1] = 0xFF;
            num[horizontal+1][position-1] = 0xFF;
            num[horizontal+1][position-2] = 0xFF;
        }
        else {
            gameOver = true;
            startTime = get_absolute_time();
            restart();
        }
    }
    else if (angle == 0 && strcmp(c, "t") == 0) {
        if (num[horizontal][position] == 0 && num[horizontal+1][position] == 0 && num[horizontal-1][position] == 0 && num[horizontal][position-1] == 0) {
            num[horizontal][position] = 0xFF;
            num[horizontal+1][position] = 0xFF;
            num[horizontal-1][position] = 0xFF;
            num[horizontal][position-1] = 0xFF;
        }
        else {
            gameOver = true;
            startTime = get_absolute_time();
            restart();
        }
    }
    else if (angle == 90 && strcmp(c, "t") == 0) {
        if (num[horizontal][position] == 0 && num[horizontal][position-1] == 0 && num[horizontal][position-2] == 0 &&
            num[horizontal-1][position-1] == 0) {
            num[horizontal][position] = 0xFF;
            num[horizontal][position-1] = 0xFF;
            num[horizontal][position-2] = 0xFF;
            num[horizontal-1][position-1] = 0xFF;
        }
        else {
            gameOver = true;
            startTime = get_absolute_time();
            restart();
        }
    }
    else if (angle == 180 && strcmp(c, "t") == 0) {
        if (num[horizontal][position] == 0 && num[horizontal][position-1] == 0 && num[horizontal+1][position-1] == 0 && num[horizontal-1][position-1] == 0) {
            num[horizontal][position] = 0xFF;
            num[horizontal][position-1] = 0xFF;
            num[horizontal+1][position-1] = 0xFF;
            num[horizontal-1][position-1] = 0xFF;
        }
        else {
            gameOver = true;
            startTime = get_absolute_time();
            restart();
        }
    }
    else if (angle == 270 && strcmp(c, "t") == 0) {
        if (num[horizontal][position] == 0 && num[horizontal][position-1] == 0 && num[horizontal][position-2] == 0 && num[horizontal+1][position-1] == 0) {
            num[horizontal][position] = 0xFF;
            num[horizontal][position-1] = 0xFF;
            num[horizontal][position-2] = 0xFF;
            num[horizontal+1][position-1] = 0xFF;
        }
        else {
            gameOver = true;
            startTime = get_absolute_time();
            restart();
        }
    }
    else if (strcmp(c, "o") == 0) {
        if (position >= 1 && position < 16 && horizontal >= 1 && horizontal < 8 && num[horizontal][position] == 0 && num[horizontal-1][position] == 0 && num[horizontal][position-1] == 0 && num[horizontal-1][position-1] == 0) {
            num[horizontal][position] = 0xFF;
            num[horizontal-1][position] = 0xFF;
            num[horizontal][position-1] = 0xFF;
            num[horizontal-1][position-1] = 0xFF;
        }
        else {
            gameOver = true;
            startTime = get_absolute_time();
            restart();
        }
    }
    else if ((angle == 0 || angle == 180) && strcmp(c, "i") == 0) {
        if (num[horizontal][position] == 0 && num[horizontal+1][position] == 0 && num[horizontal-1][position] == 0 && num[horizontal-2][position] == 0) {
            num[horizontal][position] = 0xFF;
            num[horizontal+1][position] = 0xFF;
            num[horizontal-1][position] = 0xFF;
            num[horizontal-2][position] = 0xFF;
        }
        else {
            gameOver = true;
            startTime = get_absolute_time();
            restart();
        }
    }
    else if ((angle == 90 || angle == 270) && strcmp(c, "i") == 0) {
        if (num[horizontal][position] == 0 && num[horizontal][position-1] == 0 && num[horizontal][position-2] == 0 && num[horizontal][position-3] == 0) {
            num[horizontal][position-1] = 0xFF;
            num[horizontal][position-2] = 0xFF;
            num[horizontal][position-3] = 0xFF;
            num[horizontal][position] = 0xFF;
        }
        else {
            gameOver = true;
            startTime = get_absolute_time();
            restart();
        }
    }
}

void updateNumArray(char c[]) {
    if (rotate) {
        angle += 90;
        angle = angle%360;
    }
    if (goLeft && horizontal > 2 && strcmp(c, "l") == 0) {
        horizontal -= 1;
        goLeft = false;
    }
    if (goRight && horizontal < 7 && strcmp(c, "l") == 0) {
        horizontal += 1;
        if (angle == 0 || angle == 180) {
            goRight = false;
        }
    }
    if (goRight && (angle == 90 || angle == 270) && strcmp(c, "l") == 0) {
        if (horizontal == 7) {
            horizontal += 1;
        }
        goRight = false;
    }

    if (goLeft && angle == 0 && horizontal > 0 && strcmp(c, "j") == 0) {
        horizontal -= 1;
    }

    if (goRight && angle == 0 && horizontal < 5 && strcmp(c, "j") == 0) {
        horizontal += 1;
    }

    if (goLeft && angle == 90 && horizontal > 0 && strcmp(c, "j") == 0) {
        horizontal -= 1;
    }

    if (goRight && angle == 90 && horizontal < 6 && strcmp(c, "j") == 0) {
        horizontal += 1;
    }

    if (goLeft && angle == 180 && horizontal > 2 && strcmp(c, "j") == 0) {
        horizontal -= 1;
    }

    if (goRight && angle == 180 && horizontal < 7 && strcmp(c, "j") == 0) {
        horizontal += 1;
    }

    if (goLeft && angle == 270 && horizontal > 1 && strcmp(c, "j") == 0) {
        horizontal -= 1;
    }

    if (goRight && angle == 270 && horizontal < 7 && strcmp(c, "j") == 0) {
        horizontal += 1;
    }

    if (goLeft && (angle == 0 || angle == 180) && horizontal > 0 && strcmp(c, "z") == 0) {
        horizontal -= 1;
    }

    if (goLeft && (angle == 90 || angle == 270) && horizontal > 1 && strcmp(c, "z") == 0) {
        horizontal -= 1;
    }

    if (goRight && (angle == 0 || angle == 180) && horizontal < 5 && strcmp(c, "z") == 0) {
        horizontal += 1;
    }

    if (goRight && (angle == 90 || angle == 270) && horizontal < 7 && strcmp(c, "z") == 0) {
        horizontal += 1;
    }

    if (goLeft && (angle == 0 || angle == 180) && horizontal > 2 && strcmp(c, "s") == 0) {
        horizontal -= 1;
    }

    if (goLeft && (angle == 90 || angle == 270) && horizontal > 0 && strcmp(c, "s") == 0) {
        horizontal -= 1;
    }

    if (goRight && (angle == 0 || angle == 180) && horizontal < 7 && strcmp(c, "s") == 0) {
        horizontal += 1;
    }

    if (goRight && (angle == 90 || angle == 270) && horizontal < 6 && strcmp(c, "s") == 0) {
        horizontal += 1;
    }

    if (goLeft && angle == 0 && horizontal > 1 && strcmp(c, "t") == 0) {
        horizontal -= 1;
    }

    if (goLeft && angle == 90 && horizontal > 1 && strcmp(c, "t") == 0) {
        horizontal -= 1;
    }

    if (goLeft && angle == 180 && horizontal > 1 && strcmp(c, "t") == 0) {
        horizontal -= 1;
    }

    if (goLeft && angle == 270 && horizontal > 0 && strcmp(c, "t") == 0) {
        horizontal -= 1;
    }

    if (goRight && angle == 0 && horizontal < 6 && strcmp(c, "t") == 0) {
        horizontal += 1;
    }

    if (goRight && angle == 90 && horizontal < 7 && strcmp(c, "t") == 0) {
        horizontal += 1;
    }

    if (goRight && angle == 180 && horizontal < 6 && strcmp(c, "t") == 0) {
        horizontal += 1;
    }

    if (goRight && angle == 270 && horizontal < 6 && strcmp(c, "t") == 0) {
        horizontal += 1;
    }

    if (goLeft && horizontal > 1 && strcmp(c, "o") == 0) {
        horizontal -= 1;
    }

    if (goRight && horizontal < 7 && strcmp(c, "o") == 0) {
        horizontal += 1;
    }

    if (goLeft && strcmp(c, "i") == 0 && (angle == 0 || angle == 180) && horizontal > 2) {
        horizontal -= 1;
    }

    if (goLeft && strcmp(c, "i") == 0 && (angle == 90 || angle == 270) && horizontal > 0) {
        horizontal -= 1;
    }

    if (goRight && strcmp(c, "i") == 0 && (angle == 0 || angle == 180) && horizontal < 6) {
        horizontal += 1;
    }

    if (goRight && strcmp(c, "i") == 0 && (angle == 90 || angle == 270) && horizontal < 7) {
        horizontal += 1;
    }


    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 16; j++) {
            if (!perm[i][j]) {
                num[i][j] = 0x00;
            }
        }
    }

    initializePiece(c);
    limit = getLimit(horizontal, position, c);

    if (position - limit > 1) {
        position--;
    }
    else {
        if (strcmp(c, "l") == 0) {
            if (angle == 0) {
                perm[horizontal][position] = 1;
                perm[horizontal - 1][position] = 1;
                perm[horizontal - 2][position] = 1;
                perm[horizontal - 2][position-1] = 1;
            }
            else if (angle == 90) {
                perm[horizontal-2][position-1] = 1;
                perm[horizontal-2][position-2] = 1;
                perm[horizontal-2][position] = 1;
                perm[horizontal-1][position-2] = 1;
            }
            else if (angle == 180) {
                perm[horizontal][position] = 1;
                perm[horizontal][position-1] = 1;
                perm[horizontal - 1][position-1] = 1;
                perm[horizontal - 2][position-1] = 1;
            }
            else if (angle == 270) {
                perm[horizontal-1][position] = 1;
                perm[horizontal-2][position] = 1;
                perm[horizontal-1][position-1] = 1;
                perm[horizontal-1][position-2] = 1;
            }
            currentPiece = pieces[rand()%7];
            clearRows();
            resetPosition(currentPiece);
        }
        else if (strcmp(c, "j") == 0) {
            if (angle == 0) {
                perm[horizontal][position] = 1;
                perm[horizontal][position-1] = 1;
                perm[horizontal+1][position-1] = 1;
                perm[horizontal+2][position-1] = 1;
            }
            else if (angle == 90) {
                perm[horizontal][position] = 1;
                perm[horizontal+1][position] = 1;
                perm[horizontal][position-1] = 1;
                perm[horizontal][position-2] = 1;
            }
            else if (angle == 180) {
                perm[horizontal][position] = 1;
                perm[horizontal][position-1] = 1;
                perm[horizontal - 1][position] = 1;
                perm[horizontal - 2][position] = 1;
            }
            else if (angle == 270) {
                perm[horizontal][position] = 1;
                perm[horizontal][position-1] = 1;
                perm[horizontal][position-2] = 1;
                perm[horizontal-1][position-2] = 1;
            }
            currentPiece = pieces[rand()%7];
            clearRows();
            resetPosition(currentPiece);
        }
        else if (strcmp(c, "z") == 0) {
            if (angle == 0 || angle == 180) {
                perm[horizontal][position] = 1;
                perm[horizontal+1][position] = 1;
                perm[horizontal+1][position-1] = 1;
                perm[horizontal+2][position-1] = 1;
            }
            else if (angle == 90 || angle == 270) {
                perm[horizontal][position] = 1;
                perm[horizontal][position-1] = 1;
                perm[horizontal-1][position-1] = 1;
                perm[horizontal-1][position-2] = 1;
            }
            currentPiece = pieces[rand()%7];
            clearRows();
            resetPosition(currentPiece);
        }
        else if (strcmp(c, "s") == 0) {
            if (angle == 0 || angle == 180) {
                perm[horizontal][position] = 1;
                perm[horizontal-1][position] = 1;
                perm[horizontal-1][position-1] = 1;
                perm[horizontal-2][position-1] = 1;
            }
            else if (angle == 90 || angle == 270) {
                perm[horizontal][position] = 1;
                perm[horizontal][position-1] = 1;
                perm[horizontal+1][position-1] = 1;
                perm[horizontal+1][position-2] = 1;
            }
            currentPiece = pieces[rand()%7];
            clearRows();
            resetPosition(currentPiece);
        }
        else if (strcmp(c, "t") == 0) {
            if (angle == 0) {
                perm[horizontal][position] = 1;
                perm[horizontal+1][position] = 1;
                perm[horizontal-1][position] = 1;
                perm[horizontal][position-1] = 1;
            }
            else if (angle == 90) {
                perm[horizontal-1][position-1] = 1;
                perm[horizontal][position-1] = 1;
                perm[horizontal][position-2] = 1;
                perm[horizontal][position] = 1;
            }
            else if (angle == 180) {
                perm[horizontal][position] = 1;
                perm[horizontal][position-1] = 1;
                perm[horizontal+1][position-1] = 1;
                perm[horizontal-1][position-1] = 1;
            }
            else if (angle == 270) {
                perm[horizontal][position] = 1;
                perm[horizontal][position-1] = 1;
                perm[horizontal][position-2] = 1;
                perm[horizontal+1][position-1] = 1;
            }
            currentPiece = pieces[rand()%7];
            clearRows();
            resetPosition(currentPiece);
        }
        else if (strcmp(c, "o") == 0) {
            perm[horizontal][position] = 1;
            perm[horizontal-1][position] = 1;
            perm[horizontal][position-1] = 1;
            perm[horizontal-1][position-1] = 1;

            currentPiece = pieces[rand()%7];
            clearRows();
            resetPosition(currentPiece);
        }
        else if (strcmp(c, "i") == 0) {
            if (angle == 0 || angle == 180) {
                perm[horizontal][position] = 1;
                perm[horizontal+1][position] = 1;
                perm[horizontal-1][position] = 1;
                perm[horizontal-2][position] = 1;
            }
            else if (angle == 90 || angle == 270) {
                perm[horizontal][position] = 1;
                perm[horizontal][position-1] = 1;
                perm[horizontal][position-2] = 1;
                perm[horizontal][position-3] = 1;
            }
            currentPiece = pieces[rand()%7];
            clearRows();
            resetPosition(currentPiece);
        }
    }
    rotate = false;
    goLeft = false;
    goRight = false;
}

void oled_write(char c[]) {
    ssd1306_clear();
    updateNumArray(c);
    for (uint8_t i = 0; i < 8; i++) {
        ssd1306_send_command(0xB0 + i);
        ssd1306_send_command(0x00);
        ssd1306_send_command(0x10);
        for (uint8_t col = 0; col < 128; col++) {
            ssd1306_send_data(num[i][col/8], 1);
        }
    }
}


void alarm0_isr() {
    hw_clear_bits(&timer_hw->intr, 1u << 0);
    currentTime = get_absolute_time();

    if (!gameOver) {
        oled_write(currentPiece);
    }
    else {
        ssd1306_clear();
        char line[15] = "Game Over!";
        ssd1306_draw_text(0, 0, line);
        sprintf(line, "Score: %d", score);
        ssd1306_draw_text(0, 1, line);
        if (currentTime - startTime < 5000000) {
            gameOver = true;
        }
        else {
            score = 0;
            gameOver = false;
        }
    }

    lastAngle = angle;
    uint32_t now = timer_hw->timerawl;  // Get the current timer value (in microseconds)
    timer_hw->alarm[0] = now + 200000;  // Set Alarm 0 to trigger 0.2 seconds later

}

void setup_timer_interrupt() {
    //clear pending interrupts
    hw_clear_bits(&timer_hw->intr, 1u << 0);

    //enable interrupt on timer ALARM0
    hw_set_bits(&timer_hw->inte, 1u << 0);

    //set ISR in vector table (look-up table used by microprocessor to execute interrupt)
    irq_set_exclusive_handler(TIMER_IRQ_0, alarm0_isr);
    irq_set_enabled(TIMER_IRQ_0, true);

    //first alarm
    uint32_t now = timer_hw->timerawl;
    timer_hw->alarm[0] = (uint32_t)(now + 1000000);
}


int main() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 16; j++) {
            perm[i][j] = false;
        }
    }

    stdio_init_all();
    ssd1306_init();
    ssd1306_clear();
    setup_timer_interrupt();
    restart();

    while (true) {
        if (gpio_get(LEFT) == 0) {
            goLeft = true;
            goRight = false;
        }
        if (gpio_get(RIGHT) == 0) {
            goRight = true;
            goLeft = false;
        }
        if (gpio_get(ROTATE) == 0) {
            if ((angle == 90 || angle == 270) && horizontal == 8) {
                rotate = false;
            }
            else {
                rotate = true;
            }
        }
        if (gpio_get(RESET) == 0) {
            gameOver = true;
            startTime = get_absolute_time();
            restart();
        }
        sleep_ms(50);
    }
    return 0;
}