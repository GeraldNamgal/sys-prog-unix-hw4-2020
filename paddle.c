// Gerald Arocena
// CSCI E-28, Spring 2020
// 4-5-2020
// hw 4

/* *
 * paddle.c
 * 
 */

#include    "paddle.h"
#include	<curses.h>

static struct pppaddle paddle;

/* *
 * paddle_init( int x_pos, int y_min, int y_max )
 * purpose:
 * args:
 * rets:
 */
void paddle_init( int x_pos, int y_min, int y_max )
{
    int pad_height = ( y_max - y_min - 1 ) / 3;                 // paddle height
    paddle.pad_top = ( y_min + y_max ) / 2;             // pos at middle y range    
    paddle.pad_top = paddle.pad_top - pad_height / 2;  // acct for paddle height
    paddle.pad_bot = paddle.pad_top + pad_height - 1;
    paddle.pad_col = x_pos;
    paddle.pad_char = PAD_SYMBOL;
    paddle.pad_mintop = y_min + 1;
    paddle.pad_maxbot = y_max - 1;
    
    for (int i = 0; i < pad_height; i++)                          // draw paddle
    {
        move( paddle.pad_top + i, paddle.pad_col );
        addch( paddle.pad_char );
    }

    refresh();
}

/* *
 * paddle_up()
 * purpose: move the paddle up
 * args: none
 * rets: none
 */
void paddle_up()
{
    bool moved = false;

    if ( paddle.pad_top > paddle.pad_mintop )
    {
        paddle.pad_bot--;
        paddle.pad_top--;
        moved = true;
    }

    if (moved)
    {
        mvaddch( paddle.pad_bot + 1, paddle.pad_col, BLANK );
        mvaddch( paddle.pad_top, paddle.pad_col, PAD_SYMBOL );
        refresh();
    }
}

/* *
 * paddle_down()
 * purpose: move the paddle down
 * args: none
 * rets: none
 */
void paddle_down()
{
    bool moved = false;

    if ( paddle.pad_bot < paddle.pad_maxbot )
    {
        paddle.pad_bot++;
        paddle.pad_top++;
        moved = true;
    }

    if (moved)
    {
        mvaddch( paddle.pad_top - 1, paddle.pad_col, BLANK );
        mvaddch( paddle.pad_bot, paddle.pad_col, PAD_SYMBOL );
        refresh();
    }
}

/* *
 * paddle_contact( int ball_y, int ball_x )
 * purpose: determine ball position in relation to paddle
 * args: the ball's xy-coordinates
 * rets: symbolic constant depending on contact activity, e.g., NO_CONTACT
 */
int paddle_contact( int ball_y, int ball_x )
{    
    if ( ball_y > paddle.pad_top && ball_y < paddle.pad_bot
        && ball_x == paddle.pad_col )
        return PADD_MIDDLE;                // ball hit middle block(s) of paddle

    if ( ball_y == paddle.pad_top && ball_x == paddle.pad_col )
        return PADD_TOP;                   // ball hit top block of paddle

    if ( ball_y == paddle.pad_bot && ball_x == paddle.pad_col )
        return PADD_BOTTOM;                // ball hit bottom block of paddle

    if ( paddle.pad_top == paddle.pad_mintop && ball_y == paddle.pad_mintop - 1
        && ball_x == paddle.pad_col )      // ball is in upper right corner
        return AT_MIN_TOP;                 // & paddle is flush with top row

    if ( paddle.pad_bot == paddle.pad_maxbot && ball_y == paddle.pad_maxbot + 1
        && ball_x == paddle.pad_col )      // ball is in bottom right corner
        return AT_MAX_BOT;                 // & paddle is flush with bottom row
    
    return NO_CONTACT;           
}