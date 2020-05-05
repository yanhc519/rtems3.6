/*  thread.h
 *
 *  This include file contains all constants and structures associated
 *  with the thread control block.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __THREAD_h
#define __THREAD_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/context.h>
#include <rtems/score/cpu.h>
#include <rtems/score/mppkt.h>
#include <rtems/score/object.h>
#include <rtems/score/priority.h>
#include <rtems/score/stack.h>
#include <rtems/score/states.h>
#include <rtems/score/tod.h>
#include <rtems/score/tqdata.h>
#include <rtems/score/watchdog.h>

/*
 *  The following defines the "return type" of a thread.
 */

typedef void Thread;

/*
 *  The following defines the ways in which the entry point for a
 *  thread can be invoked.  Basically, it can be passed any
 *  combination/permutation of a pointer and an unsigned32 value.
 *
 *  NOTE: For now, we are ignoring the return type.
 */

typedef enum {
  THREAD_START_NUMERIC,
  THREAD_START_POINTER,
  THREAD_START_BOTH_POINTER_FIRST,
  THREAD_START_BOTH_NUMERIC_FIRST
} Thread_Start_types;

typedef Thread ( *Thread_Entry )( );

/*
 *  The following lists the algorithms used to manage the thread cpu budget.
 *
 *  Reset Timeslice:   At each context switch, reset the time quantum.
 *  Exhaust Timeslice: Only reset the quantum once it is consumed.
 *  Callout:           Execute routine when budget is consumed.
 */

typedef enum {
  THREAD_CPU_BUDGET_ALGORITHM_NONE,
  THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE,
  THREAD_CPU_BUDGET_ALGORITHM_EXHAUST_TIMESLICE,
  THREAD_CPU_BUDGET_ALGORITHM_CALLOUT
}  Thread_CPU_budget_algorithms;

typedef struct Thread_Control_struct Thread_Control;

typedef void (*Thread_CPU_budget_algorithm_callout )( Thread_Control * );

/*
 *  The following structure contains the information which defines
 *  the starting state of a thread.
 */

typedef struct {
  Thread_Entry         entry_point;      /* starting thread address         */
  Thread_Start_types   prototype;        /* how task is invoked             */
  void                *pointer_argument; /* pointer argument                */
  unsigned32           numeric_argument; /* numeric argument                */
                                         /* initial execution modes         */
  boolean              is_preemptible;
  Thread_CPU_budget_algorithms          budget_algorithm;
  Thread_CPU_budget_algorithm_callout   budget_callout;
  unsigned32           isr_level;
  Priority_Control     initial_priority; /* initial priority                */
  boolean              core_allocated_stack;
  Stack_Control        Initial_stack;    /* stack information               */
  void                *fp_context;       /* initial FP context area address */
  void                *stack;            /* initial FP context area address */
}   Thread_Start_information;

/*
 *  The following structure contains the information necessary to manage
 *  a thread which it is  waiting for a resource.
 */

#define THREAD_STATUS_PROXY_BLOCKING 0x1111111

typedef struct {
  Objects_Id            id;              /* waiting on this object       */
  unsigned32            count;           /* "generic" fields to be used */
  void                 *return_argument; /*   when blocking */
  void                 *return_argument_1;
  unsigned32            option;

  /*
   *  NOTE: The following assumes that all API return codes can be
   *        treated as an unsigned32.  
   */
  unsigned32            return_code;     /* status for thread awakened   */

  Chain_Control         Block2n;         /* 2 - n priority blocked chain */
  Thread_queue_Control *queue;           /* pointer to thread queue      */
}   Thread_Wait_information;

/*
 *  The following defines the control block used to manage
 *  each thread proxy.
 *
 *  NOTE: It is critical that proxies and threads have identical
 *        memory images for the shared part.
 */

typedef struct {
  Objects_Control          Object;
  States_Control           current_state;
  Priority_Control         current_priority;
  Priority_Control         real_priority;
  unsigned32               resource_count;
  Thread_Wait_information  Wait;
  Watchdog_Control         Timer;
  MP_packet_Prefix        *receive_packet;
     /****************** end of common block ********************/
  Chain_Node               Active;
}   Thread_Proxy_control;


/*
 *  The following record defines the control block used
 *  to manage each thread.
 *
 *  NOTE: It is critical that proxies and threads have identical
 *        memory images for the shared part.
 */

typedef enum {
  THREAD_API_RTEMS,
  THREAD_API_POSIX
}  Thread_APIs;

#define THREAD_API_FIRST THREAD_API_RTEMS
#define THREAD_API_LAST  THREAD_API_POSIX

struct Thread_Control_struct {
  Objects_Control                       Object;
  States_Control                        current_state;
  Priority_Control                      current_priority;
  Priority_Control                      real_priority;
  unsigned32                            resource_count;
  Thread_Wait_information               Wait;
  Watchdog_Control                      Timer;
  MP_packet_Prefix                     *receive_packet;
     /****************** end of common block ********************/
  boolean                               is_global;
  boolean                               do_post_task_switch_extension;

  boolean                               is_preemptible;
  unsigned32                            cpu_time_budget;
  Thread_CPU_budget_algorithms          budget_algorithm;
  Thread_CPU_budget_algorithm_callout   budget_callout;

  Chain_Control                        *ready;
  Priority_Information                  Priority_map;
  Thread_Start_information              Start;
  Context_Control                       Registers;
  void                                 *fp_context;
  void                                 *API_Extensions[ THREAD_API_LAST + 1 ];
  void                                **extensions;
};

/*
 *  The following constants define the stack size requirements for
 *  the idle thread.
 */
 
 
#define THREAD_IDLE_STACK_SIZE  STACK_MINIMUM_SIZE
 
/*
 *  The following defines the information control block used to
 *  manage this class of objects.
 */
 
SCORE_EXTERN Objects_Information _Thread_Internal_information;
 
/*
 *  The following define the thread control pointers used to access
 *  and manipulate the idle thread.
 */
 
SCORE_EXTERN Thread_Control *_Thread_Idle;

/*
 *  The following context area contains the context of the "thread"
 *  which invoked the start multitasking routine.  This context is 
 *  restored as the last action of the stop multitasking routine.  Thus
 *  control of the processor can be returned to the environment
 *  which initiated the system.
 */
 
SCORE_EXTERN Context_Control _Thread_BSP_context;
 
/*
 *  The following declares the dispatch critical section nesting
 *  counter which is used to prevent context switches at inopportune
 *  moments.
 */

SCORE_EXTERN unsigned32 _Thread_Dispatch_disable_level;

/*
 *  If this is non-zero, then the post-task switch extension
 *  is run regardless of the state of the per thread flag.
 */

SCORE_EXTERN unsigned32 _Thread_Do_post_task_switch_extension;

/*
 *  The following holds how many user extensions are in the system.  This
 *  is used to determine how many user extension data areas to allocate
 *  per thread.
 */

SCORE_EXTERN unsigned32 _Thread_Maximum_extensions;

/*
 *  The following is used to manage the length of a timeslice quantum.
 */

SCORE_EXTERN unsigned32 _Thread_Ticks_per_timeslice;

/*
 *  The following points to the array of FIFOs used to manage the
 *  set of ready threads.
 */

SCORE_EXTERN Chain_Control *_Thread_Ready_chain;

/*
 *  The following points to the thread which is currently executing.
 *  This thread is implicitly manipulated by numerous directives.
 */

SCORE_EXTERN Thread_Control *_Thread_Executing;

/*
 *  The following points to the highest priority ready thread
 *  in the system.  Unless the current thread is not preemptibl,
 *  then this thread will be context switched to when the next
 *  dispatch occurs.
 */

SCORE_EXTERN Thread_Control *_Thread_Heir;

/*
 *  The following points to the thread whose floating point
 *  context is currently loaded.
 */

SCORE_EXTERN Thread_Control *_Thread_Allocated_fp;

/*
 *  _Thread_Handler_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this handler.
 */

void _Thread_Handler_initialization (
  unsigned32   ticks_per_timeslice,
  unsigned32   maximum_extensions,
  unsigned32   maximum_proxies
);

/*
 *  _Thread_Create_idle
 *
 *  DESCRIPTION:
 *
 *  This routine creates the idle thread.
 *
 *  WARNING!! No thread should be created before this one.
 */
 
void _Thread_Create_idle( void );

/*
 *  _Thread_Start_multitasking
 *
 *  DESCRIPTION:
 *
 *  This routine initiates multitasking.  It is invoked only as
 *  part of initialization and its invocation is the last act of
 *  the non-multitasking part of the system initialization.
 */

void _Thread_Start_multitasking( void );

/*
 *  _Thread_Dispatch
 *
 *  DESCRIPTION:
 *
 *  This routine is responsible for transferring control of the
 *  processor from the executing thread to the heir thread.  As part
 *  of this process, it is responsible for the following actions:
 *
 *     + saving the context of the executing thread
 *     + restoring the context of the heir thread
 *     + dispatching any signals for the resulting executing thread
 */

void _Thread_Dispatch( void );

/*
 *  _Thread_Initialize
 *
 *  DESCRIPTION:
 *
 *  XXX
 *
 *  NOTES:
 *
 *  If stack_area is NULL, it is allocated from the workspace.
 *
 *  If the stack is allocated from the workspace, then it is guaranteed to be
 *  of at least minimum size.
 */

boolean _Thread_Initialize(
  Objects_Information                  *information,
  Thread_Control                       *the_thread,
  void                                 *stack_area,
  unsigned32                            stack_size,
  boolean                               is_fp,
  Priority_Control                      priority,
  boolean                               is_preemptible,
  Thread_CPU_budget_algorithms          budget_algorithm,
  Thread_CPU_budget_algorithm_callout   budget_callout,
  unsigned32                            isr_level,
  Objects_Name                          name
);

/*
 *  _Thread_Start
 *
 *  DESCRIPTION:
 *
 *  XXX
 */
 
boolean _Thread_Start(
  Thread_Control           *the_thread,
  Thread_Start_types        the_prototype,
  void                     *entry_point,
  void                     *pointer_argument,
  unsigned32                numeric_argument
);

/*
 *  _Thread_Restart
 *
 *  DESCRIPTION:
 *
 *  XXX
 */
 
/* XXX multiple task arg profiles */
 
boolean _Thread_Restart(
  Thread_Control           *the_thread,
  void                     *pointer_argument,
  unsigned32                numeric_argument
);

/*
 *  _Thread_Close
 *
 *  DESCRIPTION:
 *
 *  XXX
 */
 
void _Thread_Close(
  Objects_Information  *information,
  Thread_Control       *the_thread
);

/*
 *  _Thread_Ready
 *
 *  DESCRIPTION:
 *
 *  This routine removes any set states for the_thread.  It performs
 *  any necessary scheduling operations including the selection of
 *  a new heir thread.
 */

void _Thread_Ready(
  Thread_Control *the_thread
);

/*
 *  _Thread_Clear_state
 *
 *  DESCRIPTION:
 *
 *  This routine clears the indicated STATES for the_thread.  It performs
 *  any necessary scheduling operations including the selection of
 *  a new heir thread.
 */

void _Thread_Clear_state(
  Thread_Control *the_thread,
  States_Control  state
);

/*
 *  _Thread_Set_state
 *
 *  DESCRIPTION:
 *
 *  This routine sets the indicated states for the_thread.  It performs
 *  any necessary scheduling operations including the selection of
 *  a new heir thread.
 *
 */

void _Thread_Set_state(
  Thread_Control *the_thread,
  States_Control  state
);

/*
 *  _Thread_Set_transient
 *
 *  DESCRIPTION:
 *
 *  This routine sets the TRANSIENT state for the_thread.  It performs
 *  any necessary scheduling operations including the selection of
 *  a new heir thread.
 */

void _Thread_Set_transient(
  Thread_Control *the_thread
);

/*
 *  _Thread_Reset_timeslice
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked upon expiration of the currently
 *  executing thread's timeslice.  If no other thread's are ready
 *  at the priority of the currently executing thread, then the
 *  executing thread's timeslice is reset.  Otherwise, the
 *  currently executing thread is placed at the rear of the
 *  FIFO for this priority and a new heir is selected.
 */

void _Thread_Reset_timeslice( void );

/*
 *  _Thread_Tickle_timeslice
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked as part of processing each clock tick.
 *  It is responsible for determining if the current thread allows
 *  timeslicing and, if so, when its timeslice expires.
 */

void _Thread_Tickle_timeslice( void );

/*
 *  _Thread_Yield_processor
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked when a thread wishes to voluntarily
 *  transfer control of the processor to another thread of equal
 *  or greater priority.
 */

void _Thread_Yield_processor( void );

/*
 *  _Thread_Load_environment
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the context of the_thread to its
 *  appropriate starting state.
 */

void _Thread_Load_environment(
  Thread_Control *the_thread
);

/*
 *  _Thread_Handler
 *
 *  DESCRIPTION:
 *
 *  This routine is the wrapper function for all threads.  It is
 *  the starting point for all threads.  The user provided thread
 *  entry point is invoked by this routine.  Operations
 *  which must be performed immediately before and after the user's
 *  thread executes are found here.
 */

void _Thread_Handler( void );

/*
 *  _Thread_Delay_ended
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked when a thread must be unblocked at the
 *  end of a time based delay (i.e. wake after or wake when).
 */

void _Thread_Delay_ended(
  Objects_Id  id,
  void       *ignored
);

/*
 *  _Thread_Change_priority
 *
 *  DESCRIPTION:
 *
 *  This routine changes the current priority of the_thread to
 *  new_priority.  It performs any necessary scheduling operations
 *  including the selection of a new heir thread.
 */

void _Thread_Change_priority (
  Thread_Control   *the_thread,
  Priority_Control  new_priority
);

/*
 *  _Thread_Set_priority
 *
 *  DESCRIPTION:
 *
 *  This routine updates the priority related fields in the_thread
 *  control block to indicate the current priority is now new_priority.
 */

void _Thread_Set_priority(
  Thread_Control   *the_thread,
  Priority_Control  new_priority
);

/*
 *  _Thread_Evaluate_mode
 *
 *  DESCRIPTION:
 *
 *  This routine XXX
 */

boolean _Thread_Evaluate_mode( void );

/*
 *  _Thread_Get
 *
 *  NOTE:  If we are not using static inlines, this must be a real
 *         subroutine call.
 */
 
#ifndef USE_INLINES
Thread_Control *_Thread_Get (
  Objects_Id           id,
  Objects_Locations   *location
);
#endif

/*
 *  _Thread_Idle_body
 *
 *  DESCRIPTION:
 *
 *  This routine is the body of the system idle thread.
 */
 
#if (CPU_PROVIDES_IDLE_THREAD_BODY == FALSE)
Thread _Thread_Idle_body(
  unsigned32 ignored
);
#endif

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/thread.inl>
#endif
#include <rtems/score/threadmp.h>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
