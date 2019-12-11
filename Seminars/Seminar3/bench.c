#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
//#include "green.h"

#define MAX 5000000

volatile int LOOP = 0;

volatile int pthread_count = 0;
pthread_mutex_t pthread_mutex;

//volatile int green_count = 0;
//green_mutex_t green_mutex;

void *pthread_bench(void *arg) {
    int id = *(int*) arg;
    printf("# Pthread id#%d looping %d\n", id, LOOP);
    for (int i = 0; i < LOOP; i++) {
        pthread_mutex_lock(&pthread_mutex);
        pthread_count++;
        pthread_mutex_unlock(&pthread_mutex);
    }
}
/*
void *green_bench(void *arg) {
    int id = *(int*) arg;
    printf("# Green thread id#%d looping %d\n", id, LOOP);
    for (int i = 0; i < LOOP; i++) {
        green_mutex_lock(&green_mutex);
        green_count++;
        green_mutex_unlock(&green_mutex);
    }
}
*/

int main(int argc, char const *argv[]) {
    /* Init mutex */
    pthread_mutex_init(&pthread_mutex, NULL);
    //green_mutex_init(&green_mutex);

    pthread_t t1, t2, t3;
    int p0 = 0;
    int p1 = 1;
    int p2 = 2;

    /* Run bench for pthreads */
    for(int i = 1000000; i <= MAX; i += 1000000){
      LOOP = i;
      clock_t pthread_start_time, pthread_stop_time;
      pthread_start_time = clock();
      pthread_create(&t1, NULL, pthread_bench, &p0);
      pthread_create(&t2, NULL, pthread_bench, &p1);
      pthread_create(&t3, NULL, pthread_bench, &p2);
      pthread_join(t1, NULL);
      pthread_join(t2, NULL);
      pthread_join(t3, NULL);

      pthread_stop_time = clock();
      double pthread_ms = ((double) pthread_stop_time - pthread_start_time) / ((double)CLOCKS_PER_SEC/1000);

      printf("%.2f\t%d\n", pthread_ms, pthread_count);
      printf("# Pthreads finish! Counter: %d\n", pthread_count);
      pthread_count = 0;
    }

    /* Running same test but for green threads */
    /*
    green_t g1, g2, g3;
    int gid_0 = 0;
    int gid_1 = 1;
    int gid_2 = 2;
    */

    /* Run bench for green threads */
    /*
    for(int i = 1000000; i <= MAX; i += 1000000){
      LOOP = i;
      clock_t green_start_time, green_stop_time;
      green_start_time = clock();
      green_create(&g1, green_bench, &gid_0);
      green_create(&g2, green_bench, &gid_1);
      green_create(&g3, green_bench, &gid_2);
      green_join(&g1);
      green_join(&g2);
      green_join(&g3);

      green_stop_time = clock();
      double green_ms = ((double) green_stop_time - green_start_time) / ((double)CLOCKS_PER_SEC/1000);

      printf("%.2f\t%d\n", green_ms, green_count);
      printf("# Green threads finish! Counter: %d\n", green_count);
      green_count = 0;
    }
    */

    return 0;
}
