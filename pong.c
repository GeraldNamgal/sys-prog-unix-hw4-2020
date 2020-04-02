// Gerald Arocena
// CSCI E-28, Spring 2020
// TODO: date goes here
// hw 4

/* *
 * 
 * note: TODO -- need to cite code referenced, e.g., bounce2d.c from lecture
 */

#include	<stdio.h>
#include	<curses.h>
#include	<signal.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	"pong.h"
#include	"alarmlib.h"
#include    <stdlib.h>
#include    "paddle.h"

struct ppball the_ball;

static bool moving_paddle = false;

static void	    set_up();
static void     putUpWalls();
static void     serve();
static void     ball_move();
static void     wrap_up();
static int      bounce_or_lose( struct ppball *, int, int );
static void     move_the_ball( int, int, int, int );

/* *
 * main()
 * 
 */ 
int main()
{
	int	c;	

    srand( getpid() );  // use pid as seed for random generator 

	set_up();           // initialize all stuff
    serve();            // start up ball    

	while ( ( c = getch() ) != 'Q' )
    {
		if ( c == 'k' ) {
            moving_paddle = true;
            paddle_up() ;
            moving_paddle = false;
        }	 

		else if ( c == 'm' ) {
            moving_paddle = true;
            paddle_down() ;
            moving_paddle = false;
        }
	}

	wrap_up();
	
    return 0;
}

/* *
 *
 * init ppball struct, signal handler, curses
 */

static void set_up()
{
	initscr();		            /* turn on curses	*/
	noecho();		            /* turn off echo	*/
	cbreak();		            /* turn off buffering	*/
    putUpWalls();               /* create court */
    paddle_init( RIGHT_EDGE, TOP_ROW, BOT_ROW );    
	signal( SIGINT, SIG_IGN );	/* ignore SIGINT	*/	
}

static void serve()
{
    // TODO: change the initial position to something calculated

    the_ball.y_pos = Y_INIT;
	the_ball.x_pos = X_INIT;
    if ( ( the_ball.y_count = the_ball.y_delay = ( rand() % Y_MAX ) ) < Y_MIN )
    {
        the_ball.y_count = the_ball.y_delay = Y_MIN ;
    }
	the_ball.x_count = the_ball.x_delay = ( rand() % X_MAX ) ;
	the_ball.y_dir = 1 ;
	the_ball.x_dir = 1 ;
	the_ball.symbol = DFL_SYMBOL ;
    
    mvaddch(the_ball.y_pos, the_ball.x_pos, the_ball.symbol);
	refresh();
	
	signal( SIGALRM, ball_move );
	set_ticker( 1000 / TICKS_PER_SEC );	/* send millisecs per tick */
}

/* *
 * putUpWalls()
 * purpose: 
 * args: 
 * rets: 
 */
static void putUpWalls()
{    
    // TODO: check that BORDR_SIZE doesn't exceed screen dimensions
    // e.g., if (3 rows + 2 * BORDR_SIZE ) > LINES
    //          || ( 3 cols + 2 * BORDR_SIZE ) > COLS ... some error ... ?
    // (you want the border, height of the paddle and space for the ball to go
    // through so can lose the game; doesn't need to handle screen sizes)
    
    // print top border
    move( BORDR_SIZE, BORDR_SIZE );
    for (int i = BORDR_SIZE; i < COLS - BORDR_SIZE; i++)
        addch('-');

    // print left border
    for ( int i = BORDR_SIZE + 1; i < LINES - BORDR_SIZE; i++ )
    {        
        move( i, BORDR_SIZE );
        addch('|');
    }

    // print bottom border
    move( LINES - BORDR_SIZE, BORDR_SIZE );
    for (int i = BORDR_SIZE; i < COLS - BORDR_SIZE; i++)
        addch('-');

    refresh();
}

/* *
 *
 * stop ticker and curses
 */
static void wrap_up()
{
	set_ticker( 0 );
	endwin();		/* put back to normal	*/
}

/* *
 *
 * SIGARLM handler: decr directional counters, move when they hit 0
 * note: may have too much going on in this handler
 */
static void ball_move()
{
	int	y_cur, x_cur, y_moved, x_moved;

	signal( SIGALRM , SIG_IGN );		         /* dont get caught now 	*/
	y_cur = the_ball.y_pos;                      /* old spot */
    x_cur = the_ball.x_pos;   
	y_moved = 0;
    x_moved = 0 ;

	if ( --the_ball.y_count < 0 ) {
		the_ball.y_pos += the_ball.y_dir ;	     /* move	*/
		the_ball.y_count = the_ball.y_delay ;    /* reset*/
		y_moved = 1;
	}

	if ( --the_ball.x_count < 0 ) { 
		the_ball.x_pos += the_ball.x_dir ;       /* move	*/
		the_ball.x_count = the_ball.x_delay ;	 /* reset*/
		x_moved = 1;
	}

	if ( y_moved || x_moved )
        move_the_ball( y_cur, x_cur, y_moved, x_moved );
        	
    signal(SIGALRM, ball_move);		             /* re-enable handler	*/
}

/* *
 * TODO
 */
void move_the_ball( int y_cur, int x_cur, int y_moved, int x_moved )
{
    int save_y, save_x, ret_value;
    
    if ( moving_paddle == true )
        getyx( stdscr, save_y, save_x );    // save cursor location

    ret_value = bounce_or_lose( &the_ball, y_moved, x_moved );     

    if ( ret_value == UP_DOWN_HIT || ret_value == LEFT_RIGHT_HIT
        || ret_value == CORNER_HIT ) {
            if ( y_moved )                  // "bounce" in opposite dir
                the_ball.y_pos += the_ball.y_dir * 2;
            if ( x_moved )	
                the_ball.x_pos += the_ball.x_dir * 2;
            if ( bounce_or_lose( &the_ball, y_moved, x_moved ) != 0 ) { 
                the_ball.y_pos = y_cur;     // hit another boundary, back to cur 
                the_ball.x_pos = x_cur;       
            }                    
    }

    mvaddch( y_cur, x_cur, BLANK );
    mvaddch( the_ball.y_pos, the_ball.x_pos, the_ball.symbol );
    
    if ( moving_paddle == true )
        move( save_y, save_x );              /* return cursor */
    else
        move( LINES-1, COLS-1 );	         /* or re-park cursor	*/		
    
    refresh();
}

/* *
 *
 * bounce_or_lose: if ball hits walls, change its direction
 * args: address to ppball
 * rets: 1 if a bounce happened, 0 if not
 */
static int bounce_or_lose( struct ppball *bp, int y_moved, int x_moved )
{
	if ( bp->y_pos == TOP_ROW && bp->x_pos == LEFT_EDGE ) {
		bp->y_dir = 1;
        bp->x_dir = 1;
        return CORNER_HIT;        
    }
    else if ( bp->y_pos == BOT_ROW && bp->x_pos == LEFT_EDGE ) {
		bp->y_dir = -1;
        bp->x_dir = 1;
        return CORNER_HIT;
    }
    else if ( bp->y_pos == TOP_ROW && bp->x_pos == RIGHT_EDGE )    
        if ( paddle_contact( bp->y_pos, bp->x_pos ) == AT_MIN_TOP ) {
            bp->y_dir = 1;
            bp->x_dir = -1;
            return CORNER_HIT;
        }
        else {
            bp->y_dir = 1;
            return UP_DOWN_HIT;
        }            
    else if ( bp->y_pos == BOT_ROW && bp->x_pos == RIGHT_EDGE )    
        if ( paddle_contact( bp->y_pos, bp->x_pos ) == AT_MAX_BOT ) {
            bp->y_dir = -1;
            bp->x_dir = -1;
            return CORNER_HIT;
        }
        else {
            bp->y_dir = -1;
            return UP_DOWN_HIT;
        }      
    else if ( bp->x_pos == LEFT_EDGE ) {
		bp->x_dir = 1;
        return LEFT_RIGHT_HIT;     
    }
    
    // TODO: need to make sure you change direction on bounces and for paddle
    // hits that you change the speed to a rand() one (double check the paddle
    // ones especially)

    // TODO: hit the paddle then went below bottom row with breaking the barrier

    // TODO: hitting bottom of paddle works and so does hitting the top (just fyi)

    // TODO: bouncing from lower left corner worked (just fyi)

    else if ( bp->x_pos == RIGHT_EDGE ) {
        if ( paddle_contact( bp->y_pos, bp->x_pos ) == PADD_MIDDLE ) {
            bp->x_dir = -1;
            bp->x_delay = ( rand() % X_MAX );         // change speed
            if ( ( bp->y_delay = ( rand() % Y_MAX ) ) < Y_MIN )
                bp->y_delay = Y_MIN;
            return LEFT_RIGHT_HIT;
        }

        else if ( paddle_contact( bp->y_pos, bp->x_pos ) == PADD_TOP ) {
            if ( y_moved && !x_moved ) {
                bp->y_dir = -1;
                return UP_DOWN_HIT;
            }

            bp->x_delay = ( rand() % X_MAX );   // change speed
            if ( ( bp->y_delay = ( rand() % Y_MAX ) ) < Y_MIN )
                bp->y_delay = Y_MIN;

            if ( y_moved && x_moved && bp->y_dir == 1 ) {
                bp->y_dir = -1;
                bp->x_dir = -1;
                return CORNER_HIT;
            }

            else {
                bp->x_dir = -1;
                return LEFT_RIGHT_HIT;
            }
        }

        else if ( paddle_contact( bp->y_pos, bp->x_pos ) == PADD_BOTTOM ) {
            if ( y_moved && !x_moved ) {
                bp->y_dir = 1;
                return UP_DOWN_HIT;
            }

            bp->x_delay = ( rand() % X_MAX );     // change speed
            if ( ( bp->y_delay = ( rand() % Y_MAX ) ) < Y_MIN )
                bp->y_delay = Y_MIN;

            if ( y_moved && x_moved && bp->y_dir == -1 ) {
                bp->y_dir = 1;
                bp->x_dir = -1;
                return CORNER_HIT;
            }

            else {
                bp->x_dir = -1;
                return LEFT_RIGHT_HIT;
            }
        }

        // TODO: corner hits on borders occur with right edge only when paddle
        // is flush with either top or bottom border
        
        // TODO: to know if corner hit on paddle, look at the y direction of the
        // ball. For instance, if it hit the bottom of paddle and y dir was +1
        // and it wasn't in the same column as the paddle, then it was a corner
        // hit on the paddle

        // TODO: when you change the ball's direction, need to consider if moving
        // it backward won't make it hit another boundary, e.g, from border wall
        // to paddle and vice-versa (could "stuck" the ball as a legal move)

        // TODO: can change the orientation at the beginning of ball_move instead
        // of at the end?
    }
    else if ( bp->y_pos == TOP_ROW ) {
        bp->y_dir = 1;
        return UP_DOWN_HIT;
    }
    else if ( bp->y_pos == BOT_ROW ) {
        bp->y_dir = -1;
        return UP_DOWN_HIT;
    }

	return NO_HIT;
}
