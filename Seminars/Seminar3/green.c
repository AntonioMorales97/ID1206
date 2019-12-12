#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include <assert.h>
#include <signal.h>
#include <sys/time.h>
#include "green.h"

#define FALSE 0
#define TRUE 1

#define PERIOD 100
#define STACK_SIZE 4096

/* To avoid having some functions above others... */
void green_thread(void);
void timer_handler(int);
green_t *dequeue(green_t**);
void enqueue(green_t**, green_t*);

static sigset_t block;

static ucontext_t main_cntx = {0};
static green_t main_green = {&main_cntx, NULL, NULL, NULL, NULL, FALSE};

static green_t *running = &main_green;
static green_t *ready_queue = NULL;

/* This will run when library has loaded */
static void init() __attribute__((constructor));

/* Initializes everything */
void init() {
    getcontext(&main_cntx);

    /* For timer interrupts */
    sigemptyset(&block);
    sigaddset(&block, SIGVTALRM);
    struct sigaction act = {0};
    struct timeval interval;
    struct itimerval period;
    act.sa_handler = timer_handler;
    assert(sigaction(SIGVTALRM, &act, NULL) == 0);
    interval.tv_sec = 0;
    interval.tv_usec = PERIOD;
    period.it_interval = interval;
    period.it_value = interval;
    setitimer(ITIMER_VIRTUAL, &period, NULL);
}

/**
* This function will handle the running thread when an interrupt
* occurs. Since this means that interrupts are enabled and we will do
* queue operations, we can't risk having an interrupt here. We therefore
* block interrupts while doing theese operations and unblock them before
* swaping since they should be enabled.
*/
void timer_handler(int sig) {
    sigprocmask(SIG_BLOCK, &block, NULL); //testing
    green_t *susp = running;

    /* Add the running thread to the ready queue */
    enqueue(&ready_queue, susp);

    /* Execute the next thread */
    green_t *next = dequeue(&ready_queue);
    running = next;
    sigprocmask(SIG_UNBLOCK, &block, NULL); //testing
    swapcontext(susp->context, next->context);
}

/**
* Dequeues the head of the given queue.
*/
green_t *dequeue(green_t **queue) {
    green_t *head = *queue;
    *queue = (*queue)->next;
    head->next = NULL;
    return head;
}

/**
* Enqueues the given thread in the last position of the given queue.
*/
void enqueue(green_t **queue, green_t *thread) {
  if (*queue == NULL) {
    /* Empty, insert first */
    *queue = thread;
  } else {
    /* Insert last */
    green_t *current = *queue;
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = thread;
  }
}

/**
* This function will create a thread and enqueue it to the ready queue.
*/
int green_create(green_t *new, void *(*fun)(void*), void *arg) {
    ucontext_t *cntx = (ucontext_t *) malloc(sizeof(ucontext_t));
    getcontext(cntx);

    void *stack = malloc(STACK_SIZE);
    cntx->uc_stack.ss_sp = stack;
    cntx->uc_stack.ss_size = STACK_SIZE;

    makecontext(cntx, green_thread, 0);
    new->context = cntx;
    new->fun = fun;
    new->arg = arg;
    new->next = NULL;
    new->join = NULL;
    new->zombie = FALSE;

    /* Add new to the ready queue, block interrupts for safety */
    sigprocmask(SIG_BLOCK, &block, NULL);
    enqueue(&ready_queue, new);
    sigprocmask(SIG_UNBLOCK, &block, NULL);

    return 0;
}

/**
* Starts the execution of the real function and, after returning
* from the call, terminate the thread.
*/
void green_thread() {
    //sigprocmask(SIG_BLOCK, &block, NULL); // Not here because fun is going to run
    green_t *this = running;
    (*this->fun)(this->arg);

    /* Place waiting (joining) thread in ready queue */
    if (this->join != NULL) {
        sigprocmask(SIG_BLOCK, &block, NULL);
        enqueue(&ready_queue, this->join);
        sigprocmask(SIG_UNBLOCK, &block, NULL);
    }

    /* Free allocated memory structures. See malloc() in green_create */
    /* How does this work? Since we use the stack AND deallocate it... */
    /* LUCKY, should be in green_join */
    //free(this->context->uc_stack.ss_sp);
    //free(this->context);

    /* We are done, i.e a zombie */
    this->zombie = TRUE;

    /* Find the next thread to run. This is where the thread will end. */
    sigprocmask(SIG_BLOCK, &block, NULL);
    green_t *next = dequeue(&ready_queue);
    running = next;
    sigprocmask(SIG_UNBLOCK, &block, NULL);
    setcontext(next->context);
}

/**
* Puts the running thread last in the ready queue and runs the next thread
* in line.
*/
int green_yield() {
    /* Suspend the running thread */
    sigprocmask(SIG_BLOCK, &block, NULL);
    green_t *susp = running;
    enqueue(&ready_queue, susp);

    /* Run next thread */
    green_t *next = dequeue(&ready_queue);
    running = next;
    swapcontext(susp->context, next->context);
    sigprocmask(SIG_UNBLOCK, &block, NULL);

    /* OBS: When the suspendend thread is scheduled, it will continue
      its execution from exactly this point! */
    return 0;
}

/**
* Waits for a thread to terminate. The running thread is therefore added
* to the join field and another thread is selected for execution.
*/
int green_join(green_t *thread) {
    sigprocmask(SIG_BLOCK, &block, NULL);
    if (thread->zombie){
      /* Collect the result */
      return 0;
    }

    green_t *susp = running;
    /* Add as joining thread */
    thread->join = susp;
    /* Select the next thread for execution */
    green_t *next = dequeue(&ready_queue);
    running = next;
    sigprocmask(SIG_UNBLOCK, &block, NULL);
    swapcontext(susp->context, next->context);

    free(thread->context->uc_stack.ss_sp);
    free(thread->context);
    thread->context = NULL;
    return 0;
}

/*******************CONDITION VARIABLES**************************************/

/**
* Initializes condition variable.
*/
void green_cond_init(green_cond_t *cond) {
    cond->susp = NULL;
}

/**
* This will suspend the currently running thread on the condition.
*/
void green_cond_wait(green_cond_t *cond, green_mutex_t *mutex) {
    /* Block timer interrupts */
    sigprocmask(SIG_BLOCK, &block, NULL);

    /* Suspend the currently running thread on the condition*/
    green_t *susp = running;
    enqueue(&cond->susp, susp);

    /* Mutex implementation */
    if (mutex != NULL) {
        /* Release the lock since we have it! */
        mutex->taken = FALSE;

        /* Move the suspended threads to the ready queue */
        enqueue(&ready_queue, mutex->susp);
        mutex->susp = NULL;
    }

    /* Run the next thread in the ready queue */
    green_t *next = dequeue(&ready_queue);
    running = next;
    swapcontext(susp->context, next->context);
    //sigprocmask(SIG_BLOCK, &block, NULL); // Not necessary

    /* Mutex implementation */
    if (mutex != NULL) {
        /* Try to take the lock */
        while (mutex->taken) {
            /* Bad luck, suspend on mutex */
            sigprocmask(SIG_BLOCK, &block, NULL);
            green_t *susp = running;
            enqueue(&mutex->susp, susp);
            green_t *next = dequeue(&ready_queue);
            running = next;
            swapcontext(susp->context, next->context);
            sigprocmask(SIG_UNBLOCK, &block, NULL);
        }
        /* Take the lock! */
        mutex->taken = TRUE;
    }

    /* Unblock timer interrupts */
    sigprocmask(SIG_UNBLOCK, &block, NULL);
}

/**
* Move the first suspended thread to the ready queue
*/
void green_cond_signal(green_cond_t *cond) {
    sigprocmask(SIG_BLOCK, &block, NULL);
    if (cond->susp == NULL) {
        /* No suspended threads on condition */
        return;
    }

    green_t *first = dequeue(&cond->susp);
    enqueue(&ready_queue, first);
    sigprocmask(SIG_UNBLOCK, &block, NULL);
}

/**
* Move ALL the suspended threads to the ready queue.
*/
void green_cond_broadcast(green_cond_t *cond){
  sigprocmask(SIG_BLOCK, &block, NULL);
  if (cond->susp == NULL) {
      /* No suspended threads on condition */
      return;
  }
  /* Move ALL suspended threads to the  ready queue */
  green_t *first = cond->susp;
  cond->susp = NULL;
  enqueue(&ready_queue, first);

  sigprocmask(SIG_UNBLOCK, &block, NULL);
}

/**********************MUTEX***********************************/

/* Initializes the mutex */
int green_mutex_init(green_mutex_t *mutex) {
    mutex->taken = FALSE;
    mutex->susp = NULL;
    return 0;
}

/**
* This function will try to grab the lock and if it is taken it will yield.
* When a thread is woken up after being suspended on the mutex it will try
* again to grab the lock, and most likely succeed.
*/
int green_mutex_lock(green_mutex_t *mutex) {
    /* Block timer interrupts */
    sigprocmask(SIG_BLOCK, &block, NULL);

    green_t *susp = running;
    while(mutex->taken){
        /* Suspend the running thread on the mutex */
        enqueue(&mutex->susp, susp);

        /* Execute the next thread */
        green_t *next = dequeue(&ready_queue);
        running = next;
        swapcontext(susp->context, next->context);
    }
    /* Mutex lock is free, take it! */
    mutex->taken = TRUE;

    /* Unblock timer interrupts */
    sigprocmask(SIG_UNBLOCK, &block, NULL);
    return 0;
}

/**
* Releases a mutex lock and moves the next suspended thread on the mutex to the
* ready queue.
*/
int green_mutex_unlock(green_mutex_t *mutex) {
    /* Block timer interrupts */
    sigprocmask(SIG_BLOCK, &block, NULL);

    /* If there are suspended threads, move the first to the ready queue */
    if (mutex->susp != NULL) {
        green_t *next = dequeue(&mutex->susp);
        enqueue(&ready_queue, next);
    }

    /* Release lock */
    mutex->taken = FALSE;

    /* Unblock timer interrupts */
    sigprocmask(SIG_UNBLOCK, &block, NULL);
    return 0;
}
