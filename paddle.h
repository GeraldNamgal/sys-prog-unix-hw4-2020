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
#define	PAD_SYMBOL    '#'                                 // used to draw paddle

#define NO_CONTACT     0            // symbolic constants for padd-ball relation
#define PADD_TOP       1
#define PADD_MIDDLE    2
#define PADD_BOTTOM    3
#define AT_MIN_TOP     4
#define AT_MAX_BOT     5

			              // the ping pong paddle user moves around:
struct pppaddle { int   pad_top, pad_bot, pad_col;  // top, bottom, x-coordinate
                  char  pad_char;		     // char used to draw paddle
                  int   pad_mintop, pad_maxbot; };           // paddle's y range

void paddle_init( int, int, int );
void paddle_up();
void paddle_down();
int paddle_contact ( int, int );

