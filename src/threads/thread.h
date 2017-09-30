#ifndef THREADS_THREAD_H
#define THREADS_THREAD_H

#include <debug.h>
#include <list.h>
#include <stdint.h>
#include <threads/fixed-point.h>

/* States in a thread's life cycle. */
enum thread_status
  {
    THREAD_RUNNING,     /* Running thread. */
    THREAD_READY,       /* Not running but ready to run. */
    THREAD_BLOCKED,     /* Waiting for an event to trigger. */
    THREAD_DYING        /* About to be destroyed. */
  };

/* Thread identifier type.
   You can redefine this to whatever type you like. */
typedef int tid_t;
#define TID_ERROR ((tid_t) -1)          /* Error value for tid_t. */
typedef int a, b;
#define BIGGER(a, b)  ((a) > (b) ? (a): (b))
/* Thread priorities. */
#define PRI_MIN 0                       /* Lowest priority. */
#define PRI_DEFAULT 31                  /* Default priority. */
#define PRI_MAX 63                      /* Highest priority. */
#define NICE_DEFAULT 0
#define RECENT_CPU_BEGIN 0
#define PRIORITY_FAKE -1
#define LOCK_LEVEL 8

#define NICE_MIN -20

#define NICE_MAX 20

struct child_status {
  tid_t child_id;
  bool is_exit_called;
  bool has_been_waited;
  int child_exit_status;
  struct list_elem elem_child_status;  
};

/* A kernel thread or user process.

   Each thread structure is stored in its own 4 kB page.  The
   thread structure itself sits at the very bottom of the page
   (at offset 0).  The rest of the page is reserved for the
   thread's kernel stack, which grows downward from the top of
   the page (at offset 4 kB).  Here's an illustration:

        4 kB +---------------------------------+
             |          kernel stack           |
             |                |                |
             |                |                |
             |                V                |
             |         grows downward          |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             +---------------------------------+
             |              magic              |
             |                :                |
             |                :                |
             |               name              |
             |              status             |
        0 kB +---------------------------------+

   The upshot of this is twofold:

      1. First, `struct thread' must not be allowed to grow too
         big.  If it does, then there will not be enough room for
         the kernel stack.  Our base `struct thread' is only a
         few bytes in size.  It probably should stay well under 1
         kB.

      2. Second, kernel stacks must not be allowed to grow too
         large.  If a stack overflows, it will corrupt the thread
         state.  Thus, kernel functions should not allocate large
         structures or arrays as non-static local variables.  Use
         dynamic allocation with malloc() or palloc_get_page()
         instead.

   The first symptom of either of these problems will probably be
   an assertion failure in thread_current(), which checks that
   the `magic' member of the running thread's `struct thread' is
   set to THREAD_MAGIC.  Stack overflow will normally change this
   value, triggering the assertion. */
/* The `elem' member has a dual purpose.  It can be an element in
   the run queue (thread.c), or it can be an element in a
   semaphore wait list (synch.c).  It can be used these two ways
   only because they are mutually exclusive: only a thread in the
   ready state is on the run queue, whereas only a thread in the
   blocked state is on a semaphore wait list. */
struct thread
  {
    /* Owned by thread.c. */
    tid_t tid;                          /* Thread identifier. */
    enum thread_status status;          /* Thread state. */
    char name[16];                      /* Name (for debugging purposes). */
    uint8_t *stack;                     /* Saved stack pointer. */
    int priority;                       /* Priority. */
    //change
    int stored_priority;
    
    
    //end change
    struct list_elem allelem;           /* List element for all threads list. */

    /* Shared between thread.c and synch.c. */
    struct list_elem elem;              /* List element. */
    tid_t parent_id; 
#ifdef USERPROG
    /* Owned by userprog/process.c. */
    uint32_t *pagedir;                  /* Page directory. */
    tid_t parent_id;
    int child_load_status;
    struct lock lock_child;
    struct condition cond_child;
    struct list_children;
    struct file *exec_file;
    struct hash suppl_page_table;
    mapid_t mapid_allocator;
    struct hash mmfiles;
    

#endif

    /* Owned by thread.c. */
    unsigned magic;
                         /* Detects stack overflow. */
    int64_t sleep_ticks;
    int priority_original;
    bool is_donated;
    struct list locks;
    struct lock *lock_blocked_by;
    int nice;
    int recent_cpu;

  };

/* If false (default), use round-robin scheduler.
   If true, use multi-level feedback queue scheduler.
   Controlled by kernel command-line option "-o mlfqs". */
extern bool thread_mlfqs;

void thread_init (void);
void thread_start (void);

void thread_tick (void);
void thread_print_stats (void);

typedef void thread_func (void *aux);
tid_t thread_create (const char *name, int priority, thread_func *, void *);

void thread_block (void);
void thread_unblock (struct thread *);


struct thread *thread_current (void);
tid_t thread_tid (void);
const char *thread_name (void);

void thread_exit (void) NO_RETURN;
void thread_yield (void);

/* Performs some operation on thread t, given auxiliary data AUX. */
typedef void thread_action_func (struct thread *t, void *aux);
void thread_foreach (thread_action_func *, void *);

int thread_get_priority (void);
void thread_set_priority (int);

void thread_given_set_priority (struct thread *, int, bool);
void thread_donate_priority (struct thread *, int);
void thread_calculate_advanced_priority (void);
void calculate_advanced_priority_for_all (void);
void calculate_advanced_priority (struct thread *, void *aux);
void thread_calculate_recent_cpu (void);
void calculate_recent_cpu_for_all (void);
void calculate_recent_cpu (struct thread *, void *aux);
void calculate_load_avg (void);

struct thread * thread_get_by_id (tid_t);


int thread_get_nice (void);
void thread_set_nice (int);
int thread_get_recent_cpu (void);
int thread_get_load_avg (void);

//change
void thread_priority_temporarily_up(void);
void thread_priority_restore(void);
bool compare(struct list_elem*,struct list_elem*,void*);
//task 2
void thread_block_till(int64_t);
void thread_set_next_wakeup(void);
bool compare_ticks(struct list_elem*,struct list_elem*,void*);
//end task 2
//t02
void thread_yield_current(struct thread *);
bool invcompare(struct list_elem*,struct list_elem*,void*);
//end change
struct thread *thread_get_by_id (tid_t );


void thread_sleep (int64_t );
void thread_wakeup (void);
#endif /* threads/thread.h */