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

static int  balls_left = 2;

static void	    set_up();
static void     putUpWalls();
static void     serve();
static void     ball_move();
static void     wrap_up();
static int      bounce_or_lose( struct ppball *, int, int );
static void     move_the_ball( int, int, int, int );
static int      corner_bounce( struct ppball * );
static int      edge_bounce( struct ppball * );
static void     padd_middle_hit( struct ppball * );
static void     padd_top_hit( struct ppball *, int, int );
static void     padd_bottom_hit( struct ppball *, int, int );
static void     up_paddle();
static void     down_paddle();
static void     reset();
static void     game_over();

/* *
 * main()
 * 
 */ 
int main()
{
	int	c;	

    srand( getpid() );                   // use pid as seed for random generator 

	set_up();                                            // initialize all stuff
    serve();                                                    // start up ball    

	while ( ( c = getch() ) != 'Q' )
    {
		if ( c == 'k' )            
            up_paddle() ;

		else if ( c == 'm' )             
            down_paddle() ;       
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
	initscr();		                                           // turn on curses	
	noecho();		                                            // turn off echo	
	cbreak();		                                       // turn off buffering	
    curs_set(0);                                        // make cursor invisible
    
    putUpWalls();                                                  // draw court     
    paddle_init( RIGHT_EDGE, TOP_ROW, BOT_ROW );                  // draw paddle
	
    signal( SIGINT, SIG_IGN );	                                // ignore SIGINT		
}

static void serve()
{
    // TODO: change the initial position to something calculated

    the_ball.y_pos = Y_INIT;                         // ball's initial positions
	the_ball.x_pos = X_INIT;
                                                                // ball y speed:
    if ( ( the_ball.y_count = the_ball.y_delay = ( rand() % Y_MAX ) ) < Y_MIN )
    {
        the_ball.y_count = the_ball.y_delay = Y_MIN ;      
    }
	the_ball.x_count = the_ball.x_delay = ( rand() % X_MAX ) ;   // ball x speed
	
    the_ball.y_dir = 1 ;                                    // ball's directions
	the_ball.x_dir = 1 ;
	
    the_ball.symbol = DFL_SYMBOL ;                         // ball's char symbol
    
    mvaddch(the_ball.y_pos, the_ball.x_pos, the_ball.symbol);       // draw ball
	refresh();
	
	signal( SIGALRM, ball_move );                          // set signal handler
	set_ticker( 1000 / TICKS_PER_SEC );	               // set millisecs per tick 
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
	endwin();	               	                           // put back to normal	
}

/* *
 *
 * SIGARLM handler: decr directional counters, move when they hit 0
 * note: may have too much going on in this handler
 */
static void ball_move()
{
	int	y_cur, x_cur, y_moved, x_moved;

	signal( SIGALRM , SIG_IGN );		                  // dont get caught now 	
	
    y_cur = the_ball.y_pos;                                          // old spot 
    x_cur = the_ball.x_pos;   
	y_moved = 0;                                                 // set up flags
    x_moved = 0 ;

	if ( --the_ball.y_count < 0 ) {
		the_ball.y_pos += the_ball.y_dir ;	                       // move y dir
		the_ball.y_count = the_ball.y_delay ;                     // reset count
		y_moved = 1;                                            // flag movement
	}

	if ( --the_ball.x_count < 0 ) { 
		the_ball.x_pos += the_ball.x_dir ;                         // move x dir	
		the_ball.x_count = the_ball.x_delay ;	                  // reset count
		x_moved = 1;                                            // flag movement
	}

	if ( y_moved || x_moved )                                     // ball moved?
        move_the_ball( y_cur, x_cur, y_moved, x_moved );      
        	
    signal(SIGALRM, ball_move);		                        // re-enable handler	
}

/* *
 * TODO
 */
void move_the_ball( int y_cur, int x_cur, int y_moved, int x_moved )
{
    int save_y, save_x, ret_value;    
    
    getyx( stdscr, save_y, save_x );                     // save cursor location    
    
    ret_value = bounce_or_lose( &the_ball, y_moved, x_moved );

    if ( ret_value == OFF_SCREEN )                                            // lost?
    {
        if ( balls_left > 0 )
            reset();
        else
            game_over();
        return;                                      // TODO: returning correct?
    }

    if ( ret_value == BOUNCE ) {
        the_ball.y_pos = y_cur;                              // move back to cur 
        the_ball.x_pos = x_cur;
        if ( y_moved )                            // "bounce" in opposite dir(s)
            the_ball.y_pos += the_ball.y_dir;
        if ( x_moved )	
            the_ball.x_pos += the_ball.x_dir;
                                                       // hit another boundary?:
        if ( bounce_or_lose( &the_ball, y_moved, x_moved ) != NO_HIT ) { 
            the_ball.y_pos = y_cur;                          // move back to cur 
            the_ball.x_pos = x_cur;       
        }                    
    }

    mvaddch( y_cur, x_cur, BLANK );
    mvaddch( the_ball.y_pos, the_ball.x_pos, the_ball.symbol );
    
    move( save_y, save_x );                            // return cursor location
    
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
    if ( bp->x_pos == COLS + 1)
        return OFF_SCREEN;

    else if ( bp->x_pos >= RIGHT_EDGE + 1 )
        return LOST;   
    
    else if ( y_moved == 0 && x_moved == 0 )     // paddle move (ball stays put)
    {
        if ( paddle_contact( bp->y_pos + 1, bp->x_pos ) != NO_CONTACT ) 
            return BALL_ABOVE;                           // paddle is below ball
        if ( paddle_contact( bp->y_pos - 1, bp->x_pos ) != NO_CONTACT ) 
            return BALL_BELOW;                           // paddle is above ball
    } 

    else if ( corner_bounce( bp ) )
        return BOUNCE;
    
    else if ( edge_bounce( bp ) )
        return BOUNCE;

    else if ( bp->x_pos == RIGHT_EDGE )
    {
        int ret_value = paddle_contact( bp->y_pos, bp->x_pos );       
        
        if ( ret_value == PADD_MIDDLE ) { 
            padd_middle_hit( &the_ball );
            return BOUNCE;
        }
        else if ( ret_value == PADD_TOP ) {
            padd_top_hit( &the_ball, y_moved, x_moved );   
            return BOUNCE;
        }
        else if ( ret_value == PADD_BOTTOM ) {
            padd_bottom_hit( &the_ball, y_moved, x_moved ); 
            return BOUNCE;
        }        
    }   

	return NO_HIT;
}

static int corner_bounce( struct ppball *bp ) {
    if ( bp->y_pos == TOP_ROW && bp->x_pos == LEFT_EDGE ) {          // ball loc
		bp->y_dir = 1, bp->x_dir = 1;
        return BOUNCE;        
    }
    else if ( bp->y_pos == BOT_ROW && bp->x_pos == LEFT_EDGE ) {     // ball loc
		bp->y_dir = -1, bp->x_dir = 1;
        return BOUNCE;
    }
    else if ( bp->y_pos == TOP_ROW && bp->x_pos == RIGHT_EDGE ) {    // ball loc
        if ( paddle_contact( bp->y_pos, bp->x_pos) == AT_MIN_TOP ) { // padd loc
            bp->y_dir = 1, bp->x_dir = -1;                      // corner bounce            
            bp->x_delay = ( rand() % X_MAX );                     // chg x speed
            if ( ( bp->y_delay = ( rand() % Y_MAX ) ) < Y_MIN )   // chg y speed
                bp->y_delay = Y_MIN;
            bp->x_count = 0, bp->y_count = 0;               // force  countdowns              
        }
        else 
            bp->y_dir = 1;               // else up-and-down bounce, no padd hit
        return BOUNCE;
    }        
    else if ( bp->y_pos == BOT_ROW && bp->x_pos == RIGHT_EDGE ) {    // ball loc
        if ( paddle_contact( bp->y_pos, bp->x_pos) == AT_MAX_BOT ) { // padd loc
            bp->y_dir = -1, bp->x_dir = -1;                     // corner bounce            
            bp->x_delay = ( rand() % X_MAX );                     // chg x speed
            if ( ( bp->y_delay = ( rand() % Y_MAX ) ) < Y_MIN )   // chg y speed
                bp->y_delay = Y_MIN;
            bp->x_count = 0, bp->y_count = 0;               // force  countdowns              
        }
        else 
            bp->y_dir = -1;              // else up-and-down bounce, no padd hit
        return BOUNCE;
    }
    return NO_HIT;   
}

static int edge_bounce( struct ppball * bp )
{
    if ( bp->x_pos == LEFT_EDGE )
    {
		bp->x_dir = 1;
        return BOUNCE;     
    }
    
    else if ( bp->y_pos == TOP_ROW )
    {
        bp->y_dir = 1;
        return BOUNCE;
    }
    
    else if ( bp->y_pos == BOT_ROW )
    {
        bp->y_dir = -1;
        return BOUNCE;
    }

    return NO_HIT;
}

static void padd_middle_hit( struct ppball * bp )
{
    bp->x_dir = -1;
    
    bp->x_delay = ( rand() % X_MAX );                          // change x speed
    if ( ( bp->y_delay = ( rand() % Y_MAX ) ) < Y_MIN )        // change y speed
        bp->y_delay = Y_MIN;
    
    bp->x_count = 0, bp->y_count = 0;                       // force  countdowns                             
}

static void padd_top_hit( struct ppball * bp, int y_moved, int x_moved )
{
    if ( y_moved && !x_moved )                     // hit top surface of paddle?
    {
        bp->y_dir = -1;
        return;                                                        // return
    }           

    if ( y_moved && x_moved && bp->y_dir == 1 )         // hit corner of paddle?
    {       
        bp->y_dir = -1;
        bp->x_dir = -1;
    }

    else                                         // else hit left face of paddle
        bp->x_dir = -1;

    bp->x_delay = ( rand() % X_MAX );                          // change x speed
    if ( ( bp->y_delay = ( rand() % Y_MAX ) ) < Y_MIN )        // change y speed
        bp->y_delay = Y_MIN;                     

    bp->x_count = 0, bp->y_count = 0;                       // force  countdowns
}

static void padd_bottom_hit( struct ppball * bp, int y_moved, int x_moved )
{
    if ( y_moved && !x_moved )                  // hit bottom surface of paddle? 
    {
        bp->y_dir = 1; 
        return;                                                        // return
    }              

    if ( y_moved && x_moved && bp->y_dir == -1 )        // hit corner of paddle?
    {   bp->y_dir = 1;                
        bp->x_dir = -1;                
    }

    else                                         // else hit left face of paddle
        bp->x_dir = -1;

    bp->x_delay = ( rand() % X_MAX );                          // change x speed
    if ( ( bp->y_delay = ( rand() % Y_MAX ) ) < Y_MIN )        // change y speed
        bp->y_delay = Y_MIN;                

    bp->x_count = 0, bp->y_count = 0;                       // force  countdowns                
}

static void up_paddle()
{
    if ( bounce_or_lose( &the_ball, 0, 0 ) == BALL_ABOVE )
    {
        the_ball.y_dir = -1;                         // bounce/change ball's dir
        
        if ( the_ball.y_pos != TOP_ROW + 1 )      // if ball is not at y min top
        {
            mvaddch(the_ball.y_pos, the_ball.x_pos, BLANK);  // move/redraw ball
            the_ball.y_pos += the_ball.y_dir ; 
            mvaddch(the_ball.y_pos, the_ball.x_pos, the_ball.symbol);
            
            paddle_up();                                      // move the paddle
        }     
    }

    else
        paddle_up();
}

static void down_paddle()
{
    if ( bounce_or_lose( &the_ball, 0, 0 ) == BALL_BELOW )
    {
        the_ball.y_dir = 1;                          // bounce/change ball's dir
        
        if ( the_ball.y_pos != BOT_ROW - 1 )   // if ball is not at y max bottom
        {
            mvaddch(the_ball.y_pos, the_ball.x_pos, BLANK);  // move/redraw ball
            the_ball.y_pos += the_ball.y_dir ;
            mvaddch(the_ball.y_pos, the_ball.x_pos, the_ball.symbol);   
            
            paddle_down();                                    // move the paddle        
        } 
    }

    else
        paddle_down();
}

static void reset()
{
    clear();                                                     // clear screen
    set_up();                                          // reinitialize all stuff
    serve();                                              // start up ball again 
    balls_left--;
}

static void game_over()
{

}