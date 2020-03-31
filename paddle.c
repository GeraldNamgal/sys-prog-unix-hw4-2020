// Gerald Arocena
// CSCI E-28, Spring 2020
// TODO: date goes here
// hw 4

#include    "paddle.h"
#include	<curses.h>

struct pppaddle paddle;

void paddle_init( int x_pos, int y_max, int y_min )
{
    // TODO (not sure if relevant): can restrict paddle length to something like
    //       < LINES - 2 * BORDR_SIZE - 2

    paddle.pad_top = ( y_max + y_min ) / 2;            // to center of y range    
    paddle.pad_top = paddle.pad_top - PAD_HEIGHT / 2;  // acct for paddle height
    paddle.pad_bot = paddle.pad_top + PAD_HEIGHT - 1;
    paddle.pad_col = x_pos;
    paddle.pad_char = PAD_SYMBOL;
    paddle.pad_mintop = y_max;
    paddle.pad_maxbot = y_min;

    // draw paddle
    for (int i = 0; i < PAD_HEIGHT; i++)
    {
        move( paddle.pad_top + i, paddle.pad_col );
        addch(paddle.pad_char);
    }
}

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
        move( LINES-1, COLS-1 );		             /* park cursor	*/
        refresh();
    }

    #if 0
    if (paddle->pad_top > paddle->pad_mintop)
        move( paddle );
    #endif
}

void paddle_down()
{
    int cur_top = paddle.pad_top;
    bool moved = false;

    if ( paddle.pad_bot < paddle.pad_maxbot )
    {
        paddle.pad_bot++;
        paddle.pad_top++;
        moved = true;
    }

    if (moved)
    {

        move( LINES-1, COLS-1 );		             /* park cursor	*/
    }
}
