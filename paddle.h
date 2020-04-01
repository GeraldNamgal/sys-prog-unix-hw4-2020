// Gerald Arocena
// CSCI E-28, Spring 2020
// TODO: date goes here
// hw 4

#include <stdbool.h>

#define BLANK         ' '
#define	PAD_SYMBOL    '#'
#define PAD_HEIGHT     4

#define TOP_HIT        1
#define MIDDLE_HIT     2
#define BOTTOM_HIT     3

struct pppaddle { int   pad_top, pad_bot, pad_col;
                  char  pad_char;
                  int   pad_mintop, pad_maxbot; };

void paddle_init( int, int, int );
void paddle_up();
void paddle_down();
int paddle_contact ( int, int );

