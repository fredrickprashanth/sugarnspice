#include <stdio.h>
#include <time.h>
#include <signal.h>

#ifndef __UTILS_H__

#define __UTILS_H__

#define delta_timespec(t1, t2) (((double)((t1.tv_sec - t2.tv_sec)*1000000000.0 + (t1.tv_nsec - t2.tv_nsec) + 100.0))/1000000.0)

void _sig_term_handler(int);





#ifdef NDEBUG

#define debug_printf(format, ...)
#define debug_print_error(...)
static void time_it() { }

#else /* !NDEBUG */

#define debug_printf(...) printf("debug:"__VA_ARGS__)
#define time_it do_time_it
				
#endif /* NDEBUG */


static void inline
register_sigterm()
{
	signal(SIGTERM, _sig_term_handler);
}
#endif /* __UTILS_H__ */
