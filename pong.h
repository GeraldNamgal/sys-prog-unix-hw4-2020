// Gerald Arocena
// CSCI E-28, Spring 2020
// TODO: date goes here
// hw 4

/* *
 * 
 * note: TODO -- need to cite code referenced, e.g., bounce2d.c from lecture
 */

/* *
 * TODO
 * some parameters
 */
#define	BLANK		    ' '                     // ball constants
#define	DFL_SYMBOL	    'o'
#define BORDR_SIZE       3
#define OFF_SCREEN      -2
#define LOST            -1
#define NO_HIT           0
#define BOUNCE           1
#define BALL_BELOW       2
#define BALL_ABOVE       3
#define TOTAL_BALLS      3

#define	TOP_ROW		    BORDR_SIZE              // court constants
#define	BOT_ROW 	    LINES - BORDR_SIZE
#define	LEFT_EDGE	    BORDR_SIZE
#define	RIGHT_EDGE	    COLS - BORDR_SIZE - 1

#define	TICKS_PER_SEC	50		                // affects speed
#define X_MAX            6                      // max delay for x ball speed
#define Y_MIN            7 // TODO (change #?): 7   // min delay for y ball speed 
#define Y_MAX           13 // TODO (change #?): 20  // max delay for y ball speed
#define TIME_LIMIT      99                 // max num mins. of allowed game play
#define SECS_PER_MIN    60
#define INTERVAL_SECS    3

/* *
 * TODO
 * the only object in town
 */
struct ppball { int   x_pos, x_dir, x_delay, x_count,
			          y_pos, y_dir, y_delay, y_count ;	          
		        char  symbol ; } ;