// Gerald Arocena
// CSCI E-28, Spring 2020
// TODO: date goes here
// hw 4

#include    "paddle.h"
#include	<curses.h>

static struct pppaddle paddle;

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

    refresh();
}

void paddle_up()
{
    bool moved = false;

    if ( paddle.pad_top > paddle.pad_mintop + 1 )
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
}

void paddle_down()
{
    bool moved = false;

    if ( paddle.pad_bot < paddle.pad_maxbot - 1 )
    {
        paddle.pad_bot++;
        paddle.pad_top++;
        moved = true;
    }

    if (moved)
    {
        mvaddch( paddle.pad_top - 1, paddle.pad_col, BLANK );
        mvaddch( paddle.pad_bot, paddle.pad_col, PAD_SYMBOL );
        move( LINES-1, COLS-1 );		             /* park cursor	*/
        refresh();
    }
}

// TODO
int paddle_contact( int y, int x )
{
    int     middle = 2 ,       // middle of paddle
            top    = 4 ,       // top of paddle
            bottom = 5 ;       // bottom of paddle           
    
    if ( y > paddle.pad_top && y < paddle.pad_bot && x == paddle.pad_col )
        return middle;
    
    return 0;                  // 0 for no contact
}