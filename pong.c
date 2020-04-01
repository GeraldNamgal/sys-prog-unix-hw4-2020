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
static int      bounce_or_lose( struct ppball * );

/* *
 * main()
 * 
 */ 
int main()
{
	int	c;	

    srand( getpid() );  // use pid as seed for random generator 

	set_up();           // init all stuff
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
    for (int i = BORDR_SIZE; i <= COLS - BORDR_SIZE; i++)
        addch('-');

    // print left border
    for ( int i = BORDR_SIZE + 1; i < LINES - BORDR_SIZE; i++ )
    {        
        move( i, BORDR_SIZE );
        addch('|');
    }

    // print bottom border
    move( LINES - BORDR_SIZE, BORDR_SIZE );
    for (int i = BORDR_SIZE; i <= COLS - BORDR_SIZE; i++)
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
	int	y_cur, x_cur, moved, save_y, save_x;

	signal( SIGALRM , SIG_IGN );		         /* dont get caught now 	*/
	y_cur = the_ball.y_pos ;		             /* old spot		*/
	x_cur = the_ball.x_pos ;
	moved = 0 ;

	if ( --the_ball.y_count < 0 ) {
		the_ball.y_pos += the_ball.y_dir ;	     /* move	*/
		the_ball.y_count = the_ball.y_delay ;    /* reset*/
		moved = 1;
	}

	if ( --the_ball.x_count < 0 ) { 
		the_ball.x_pos += the_ball.x_dir ;       /* move	*/
		the_ball.x_count = the_ball.x_delay ;	 /* reset*/
		moved = 1;
	}

	if ( moved ) {
        if ( moving_paddle == true )
            getyx( stdscr, save_y, save_x );     /* save cursor location */
		mvaddch( y_cur, x_cur, BLANK );
		mvaddch( the_ball.y_pos, the_ball.x_pos, the_ball.symbol );
		bounce_or_lose( &the_ball );
        if ( moving_paddle == true )
            move( save_y, save_x );              /* return to saved location */
        else
            move( LINES-1, COLS-1 );	         /* park cursor	*/		
		refresh();
	}
	
    signal(SIGALRM, ball_move);		             /* re-enable handler	*/
}

/* *
 *
 * bounce_or_lose: if ball hits walls, change its direction
 * args: address to ppball
 * rets: 1 if a bounce happened, 0 if not
 */
static int bounce_or_lose(struct ppball *bp)
{
	int	return_val = 0 ;

	if ( bp->y_pos == TOP_ROW )
		bp->y_dir = 1 , return_val = 1;        
	
    else if ( bp->y_pos == BOT_ROW )
		bp->y_dir = -1 , return_val = 1;
	
    if ( bp->x_pos == LEFT_EDGE )
		bp->x_dir = 1 , return_val = 1;     
	
    else if ( bp->x_pos == RIGHT_EDGE ) {
        if ( paddle_contact( the_ball.y_pos, the_ball.x_pos ) == 1 ) {
            bp->x_dir = -1 , return_val = 1 ,
            the_ball.x_count = the_ball.x_delay = ( rand() % X_MAX );
            if ( ( the_ball.y_count = the_ball.y_delay = ( rand() % Y_MAX ) )
                < Y_MIN )
                    the_ball.y_count = the_ball.y_delay = Y_MIN;
        }
    }

	return return_val;
}
