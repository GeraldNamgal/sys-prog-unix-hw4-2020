// Gerald Arocena
// CSCI E-28, Spring 2020
// 4-5-2020
// hw 4

/* *
 * paddle.h
 * Header file for paddle.c
 */

#include <stdbool.h>

#define BLANK         ' '
#define	PAD_SYMBOL    '#'

#define NO_CONTACT     0
#define PADD_TOP       1
#define PADD_MIDDLE    2
#define PADD_BOTTOM    3
#define AT_MIN_TOP     4
#define AT_MAX_BOT     5

struct pppaddle { int   pad_top, pad_bot, pad_col;
                  char  pad_char;
                  int   pad_mintop, pad_maxbot; };

void paddle_init( int, int, int );
void paddle_up();
void paddle_down();
int paddle_contact ( int, int );

