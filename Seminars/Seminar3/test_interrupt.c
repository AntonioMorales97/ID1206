#include <stdio.h>
#include "green.h"

int flag = 0;
green_cond_t cond;

/**
* The third test: testing threads without yield, instead with
* a timer interrupt to see that it works.
*/
void *test_threads(void *arg){
  int i = *(int*) arg;
  int loop = 500000;
  while(loop > 0){
    printf("thread %d: %d\n", i, loop);
    loop--;
  }
}

int main(){
  green_t g0, g1;
  int a0 = 0;
  int a1 = 1;

  green_cond_init(&cond);

  green_create(&g0, test_threads, &a0);
  green_create(&g1, test_threads, &a1);

  green_join(&g0);
  green_join(&g1);
  printf("done\n");
  return 0;
}
