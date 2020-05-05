/*
 *  $Id$
 */

#include <assert.h>
#include <time.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>

#include <rtems/posix/seterr.h>
#include <rtems/posix/time.h>

/*PAGE
 *
 *  _POSIX_Timespec_subtract
 */

void _POSIX_Timespec_subtract(
  const struct timespec *the_start,
  const struct timespec *end,
  struct timespec *result
)
{
  struct timespec  start_struct = *the_start;
  struct timespec *start = &start_struct;
  unsigned int nsecs_per_sec = TOD_NANOSECONDS_PER_SECOND;
 
  if (end->tv_nsec < start->tv_nsec) {
    int seconds = (start->tv_nsec - end->tv_nsec) / nsecs_per_sec + 1;
    start->tv_nsec -= nsecs_per_sec * seconds;
    start->tv_sec += seconds;
  }
 
  if (end->tv_nsec - start->tv_nsec > nsecs_per_sec) {
    int seconds = (start->tv_nsec - end->tv_nsec) / nsecs_per_sec;
    start->tv_nsec += nsecs_per_sec * seconds;
    start->tv_sec -= seconds;
  }
 
  result->tv_sec  = end->tv_sec - start->tv_sec;
  result->tv_nsec = end->tv_nsec - start->tv_nsec;
}

/*PAGE
 *
 *  _POSIX_Timespec_to_interval
 */

Watchdog_Interval _POSIX_Timespec_to_interval(
  const struct timespec *time
)
{
  Watchdog_Interval  ticks;

  ticks  = (time->tv_sec * TOD_MICROSECONDS_PER_SECOND) / 
             _TOD_Microseconds_per_tick;

  ticks += (time->tv_nsec / TOD_NANOSECONDS_PER_MICROSECOND) / 
             _TOD_Microseconds_per_tick;

  return ticks;
}

/*PAGE
 *
 *  _POSIX_Interval_to_timespec
 */
 
void _POSIX_Interval_to_timespec(
  Watchdog_Interval  ticks,
  struct timespec   *time
)
{
  unsigned32  usecs;

  usecs = ticks * _TOD_Microseconds_per_tick;

  time->tv_sec  = usecs / TOD_MICROSECONDS_PER_SECOND;
  time->tv_nsec = (usecs % TOD_MICROSECONDS_PER_SECOND) * 
                    TOD_NANOSECONDS_PER_MICROSECOND;
}

/*PAGE
 *
 *  4.5.1 Get System Time, P1003.1b-1993, p. 91
 */

time_t time(
  time_t   *tloc
)
{
  time_t  seconds_since_epoch;

  /*
   *  No error is the time of day is not set.   For RTEMS the system time
   *  starts out at the rtems epoch.
   */

  /*
   *  Internally the RTEMS epoch is 1988.  This must be taken into account.
   */

  seconds_since_epoch = _TOD_Seconds_since_epoch;
     
  seconds_since_epoch += POSIX_TIME_SECONDS_1970_THROUGH_1988;

  if ( tloc )
    *tloc = seconds_since_epoch;

  return seconds_since_epoch;
}

/*PAGE
 *
 *  14.2.1 Clocks, P1003.1b-1993, p. 263
 */

int clock_settime(
  clockid_t              clock_id,
  const struct timespec *tp
)
{
  struct tm         split_time;
  TOD_Control       tod;
  Watchdog_Interval seconds;

  assert( tp );

  switch ( clock_id ) {
 
    case CLOCK_REALTIME:
      (void) gmtime_r( &tp->tv_sec, &split_time );
 
      /*
       *  Convert the tm structure format to that used by the TOD Handler
       *
       *  NOTE: TOD Handler does not honor leap seconds.
       */

      tod.year   = split_time.tm_year + 1900;  /* RHS is years since 1900 */
      tod.month  = split_time.tm_mon + 1;      /* RHS uses 0-11 */
      tod.day    = split_time.tm_mday;
      tod.hour   = split_time.tm_hour;
      tod.minute = split_time.tm_min;
      tod.second = split_time.tm_sec;  /* RHS allows 0-61 for leap seconds */

      tod.ticks  = (tp->tv_nsec / TOD_NANOSECONDS_PER_MICROSECOND) /
                      _TOD_Microseconds_per_tick;

      if ( !_TOD_Validate( &tod ) )
        set_errno_and_return_minus_one( EINVAL );
 
      /*
       *  We can't use the tp->tv_sec field because it is based on 
       *  a different EPOCH.
       */

      seconds = _TOD_To_seconds( &tod );
      _Thread_Disable_dispatch();
        _TOD_Set( &tod, seconds );
      _Thread_Enable_dispatch();
      break;
 
#ifdef _POSIX_CPUTIME
    case CLOCK_PROCESS_CPUTIME:
      return POSIX_NOT_IMPLEMENTED();
      break;
#endif
 
#ifdef _POSIX_THREAD_CPUTIME
    case CLOCK_THREAD_CPUTIME:
      return POSIX_NOT_IMPLEMENTED();
      break;
#endif
    default:
      set_errno_and_return_minus_one( EINVAL );
 
  }
  return 0;
}

/*PAGE
 *
 *  14.2.1 Clocks, P1003.1b-1993, p. 263
 */

int clock_gettime(
  clockid_t        clock_id,
  struct timespec *tp
)
{
  ISR_Level      level;
  time_t         seconds;
  long           ticks;

  if ( !tp )
    set_errno_and_return_minus_one( EINVAL );

  switch ( clock_id ) {

    case CLOCK_REALTIME:
 
      _ISR_Disable( level );
        seconds = _TOD_Seconds_since_epoch;
        ticks   = _TOD_Current.ticks;
      _ISR_Enable( level );
 
      tp->tv_sec  = seconds + POSIX_TIME_SECONDS_1970_THROUGH_1988;
      tp->tv_nsec = ticks * _TOD_Microseconds_per_tick * 
                      TOD_NANOSECONDS_PER_MICROSECOND; 
      break;

#ifdef _POSIX_CPUTIME
    case CLOCK_PROCESS_CPUTIME:
      /* don't base this on _Watchdog_Ticks_since_boot--duration is too short*/
      return POSIX_NOT_IMPLEMENTED();
      break;
#endif

#ifdef _POSIX_THREAD_CPUTIME
    case CLOCK_THREAD_CPUTIME:
      return POSIX_NOT_IMPLEMENTED();
      break;
#endif
    default:
      set_errno_and_return_minus_one( EINVAL );

  }
  return 0;
}

/*PAGE
 *
 *  14.2.1 Clocks, P1003.1b-1993, p. 263
 */

int clock_getres(
  clockid_t        clock_id,
  struct timespec *res
)
{
  if ( !res )
    set_errno_and_return_minus_one( EINVAL );
 
  switch ( clock_id ) {
 
    /*
     *  All time in rtems is based on the same clock tick.
     */

    case CLOCK_REALTIME:
    case CLOCK_PROCESS_CPUTIME:
    case CLOCK_THREAD_CPUTIME:
      if ( res )
        _POSIX_Interval_to_timespec( _TOD_Microseconds_per_tick, res ); 
      break;
 
    default:
      set_errno_and_return_minus_one( EINVAL );
 
  }
  return 0;
}

/*PAGE
 *
 *  14.2.5 High Resolution Sleep, P1003.1b-1993, p. 269
 */

int nanosleep(
  const struct timespec  *rqtp,
  struct timespec        *rmtp
)
{
  Watchdog_Interval  ticks;
  struct timespec   *the_rqtp;

  if ( !rqtp )
    set_errno_and_return_minus_one( EINVAL );

  the_rqtp = (struct timespec *)rqtp;

  /*
   *  Return EAGAIN if the delay interval is negative.  
   *
   *  NOTE:  This behavior is beyond the POSIX specification.  
   *         FSU pthreads shares this behavior.
   */

  if ( the_rqtp->tv_sec < 0 )
    the_rqtp->tv_sec = 0;

  if ( /* the_rqtp->tv_sec < 0 || */ the_rqtp->tv_nsec < 0 )
    set_errno_and_return_minus_one( EAGAIN );

  if ( the_rqtp->tv_nsec >= TOD_NANOSECONDS_PER_SECOND )
    set_errno_and_return_minus_one( EINVAL );
 
  ticks = _POSIX_Timespec_to_interval( the_rqtp );

  /*
   *  This behavior is also beyond the POSIX specification but is
   *  consistent with the RTEMS api and yields desirable behavior.
   */

  if ( !ticks ) {
    _Thread_Yield_processor();
    _Thread_Dispatch();
    if ( rmtp ) {
       rmtp->tv_sec = 0; 
       rmtp->tv_nsec = 0; 
    }
    return 0;
  }
  
  _Thread_Disable_dispatch();
    _Thread_Set_state(
      _Thread_Executing,
      STATES_DELAYING | STATES_INTERRUPTIBLE_BY_SIGNAL
    );
    _Watchdog_Initialize(
      &_Thread_Executing->Timer,
      _Thread_Delay_ended,
      _Thread_Executing->Object.id,
      NULL
    );
    _Watchdog_Insert_ticks( &_Thread_Executing->Timer, ticks );
  _Thread_Enable_dispatch();

  /* calculate time remaining */

  if ( rmtp ) {
    ticks -= 
      _Thread_Executing->Timer.stop_time - _Thread_Executing->Timer.start_time;

    _POSIX_Interval_to_timespec( ticks, rmtp );

    /*
     *  If there is time remaining, then we were interrupted by a signal.
     */

    if ( ticks )
      set_errno_and_return_minus_one( EINTR );
  }

  return 0;
}

/*PAGE
 *
 *  20.1.3 Accessing a Process CPU-time CLock, P1003.4b/D8, p. 55
 */

int clock_getcpuclockid(
  pid_t      pid,
  clockid_t *clock_id
)
{
  return POSIX_NOT_IMPLEMENTED();
}

/*PAGE
 *
 *  20.1.5 CPU-time Clock Attribute Access, P1003.4b/D8, p. 58
 */

int clock_setenable_attr(
  clockid_t    clock_id,
  int          attr
)
{
  return POSIX_NOT_IMPLEMENTED();
}

/*PAGE
 *
 *  20.1.5 CPU-time Clock Attribute Access, P1003.4b/D8, p. 58
 */

int clock_getenable_attr(
  clockid_t    clock_id,
  int         *attr
)
{
  return POSIX_NOT_IMPLEMENTED();
}
