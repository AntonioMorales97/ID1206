#include <stdio.h>
#include "green.h"

volatile int count = 0;
green_mutex_t mutex;

/**
* The fourth test: testing mutex.
*/
void *test_mutex(void *arg){
  int id = *(int*) arg;
  int loop = 1000000;
  while(loop > 0){
    green_mutex_lock(&mutex);
    printf("thread %d: %d\n", id, loop);
    loop--;
    count++;
    green_mutex_unlock(&mutex);
  }
}

int main(){
  green_t g0, g1, g2, g3;
  int a0 = 0;
  int a1 = 1;
  int a2 = 2;
  int a3 = 3;

  green_mutex_init(&mutex);

  green_create(&g0, test_mutex, &a0);
  green_create(&g1, test_mutex, &a1);
  green_create(&g2, test_mutex, &a2);
  green_create(&g3, test_mutex, &a3);

  green_join(&g0);
  green_join(&g1);
  green_join(&g2);
  green_join(&g3);
  printf("%d\n", count);
  printf("done\n");
  return 0;
}
