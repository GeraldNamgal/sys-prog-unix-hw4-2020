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
#define	BLANK		    ' '
#define	DFL_SYMBOL	    'o'
#define BORDR_SIZE       3
#define	TOP_ROW		    BORDR_SIZE + 1
#define	BOT_ROW 	    LINES - BORDR_SIZE - 1
#define	LEFT_EDGE	    BORDR_SIZE + 1
#define	RIGHT_EDGE	    COLS - BORDR_SIZE - 1
#define	X_INIT		    10		                /* starting col		*/
#define	Y_INIT		    10		                /* starting row		*/
#define	TICKS_PER_SEC	50		                /* affects speed	*/
#define X_MAX            6                      /* max delay for x ball speed */
#define Y_MIN            7                      /* min delay for y ball speed */
#define Y_MAX           20                      /* max delay for y ball speed */

/* *
 * TODO
 * the only object in town
 */
struct ppball { int   x_pos, x_dir, x_delay, x_count,
			          y_pos, y_dir, y_delay, y_count ;	          
		        char  symbol ; } ;