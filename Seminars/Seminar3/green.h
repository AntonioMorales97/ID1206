#include <ucontext.h>

typedef struct green_t {
    struct ucontext_t *context;
    void *(*fun)(void*);
    void *arg;
    struct green_t *next;
    struct green_t *join;
    int zombie;
} green_t;

typedef struct green_cond_t {
    struct green_t *susp;
} green_cond_t;

typedef struct green_mutex_t {
    volatile int taken;
    struct green_t *susp;
} green_mutex_t;

/* Create, yield, and wait */
int green_create(green_t *thread, void *(*fun)(void*), void *arg);
int green_yield();
int green_join(green_t *thread);

/* Condition variables */
void green_cond_init(green_cond_t*);
void green_cond_wait(green_cond_t*, green_mutex_t*);
void green_cond_signal(green_cond_t*);
void green_cond_broadcast(green_cond_t*);

/* Mutex locks */
int green_mutex_init(green_mutex_t *mutex);
int green_mutex_lock(green_mutex_t *mutex);
int green_mutex_unlock(green_mutex_t *mutex);
