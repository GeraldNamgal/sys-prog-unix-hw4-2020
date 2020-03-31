// Gerald Arocena
// CSCI E-28, Spring 2020
// TODO: date goes here
// hw 4

/* *
 * 
 * note: TODO -- need to cite code referenced, e.g., bounce2d.c from lecture
 *
 *	alarmlib.c	
 *			timer functions for higher resolution clock
 *
 *			set_ticker( number_of_milliseconds )
 *				arranges for the interval timer to issue
 *				SIGALRM at regular intervals
 *			millisleep( number_of_milliseconds )
 *
 * 	version 12.04.01 changed interface to set_ticker so it rets -1 on error
 *	version 98.03.16
 */

#include	<sys/time.h>
#include	<signal.h>
#include	<errno.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>

/* *
 * 
 * arg in milliseconds, converted into micro seoncds
 * Returns -1 on error, 0 if no error
 */
int set_ticker( int n_msecs )
{
	struct itimerval new_timeset, old_timeset;
	long	n_sec, n_usecs;

	n_sec = n_msecs / 1000 ;
	n_usecs = ( n_msecs % 1000 ) * 1000L ;

	new_timeset.it_interval.tv_sec  = n_sec;	/* set reload  */
	new_timeset.it_interval.tv_usec = n_usecs;	/* new ticker value */
	new_timeset.it_value.tv_sec     = n_sec  ;	/* store this	*/
	new_timeset.it_value.tv_usec    = n_usecs ;	/* and this 	*/

	if ( setitimer( ITIMER_REAL, &new_timeset, &old_timeset ) != 0 ){
		printf("Error with timer..errno=%d\n", errno );
		return -1;
	}
	return 0;
}

// TODO: need these or delete?
#if 0
static void my_handler();

void millisleep( int n )
{
	signal( SIGALRM , my_handler);	/* set handler		*/
	set_ticker( n );			/* set alarm timer	*/
	pause();				/* wait for sigalrm	*/
}

static void my_handler()
{
	set_ticker( 0 );			/* turns off ticker */
}
#endif