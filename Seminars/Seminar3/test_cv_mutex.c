#include <stdio.h>
#include "green.h"

int flag = 0;
green_cond_t cond;
green_mutex_t mutex;

/**
* The fifth test: testing condition variables with mutex using the atomical
* call to green_cond_wait.
* OBS: green_cond_broadcast
*/
void *test_condition_variables_with_mutex(void *arg){
  int id = *(int*) arg;
  int loop = 1000000;
  while(loop > 0){
    green_mutex_lock(&mutex);
    while(flag != id){
      green_cond_wait(&cond, &mutex);
    }
    flag = (id + 1) % 4;
    printf("thread %d: %d\n", id, loop);
    loop--;
    green_cond_broadcast(&cond);
    green_mutex_unlock(&mutex);
  }
}

int main(){
  green_t g0, g1, g2, g3;
  int a0 = 0;
  int a1 = 1;
  int a2 = 2;
  int a3 = 3;

  green_cond_init(&cond);
  green_mutex_init(&mutex);

  green_create(&g0, test_condition_variables_with_mutex, &a0);
  green_create(&g1, test_condition_variables_with_mutex, &a1);
  green_create(&g2, test_condition_variables_with_mutex, &a2);
  green_create(&g3, test_condition_variables_with_mutex, &a3);

  green_join(&g0);
  green_join(&g1);
  green_join(&g2);
  green_join(&g3);
  printf("done\n");
  return 0;
}
