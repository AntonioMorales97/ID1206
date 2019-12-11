#include <stdio.h>
#include "green.h"

/**
* The first test: testing threads with yield().
* OBS: With interrupts (and blocking in yield) this will still work because
* there is no shared data structure.
*/
void *test_threads(void *arg){
  int i = *(int*) arg;
  int loop = 50000;
  while(loop > 0){
    printf("thread %d: %d\n", i, loop);
    loop--;
    green_yield();
  }
}

int main(){
  green_t g0, g1;
  int a0 = 0;
  int a1 = 1;

  green_create(&g0, test_threads, &a0);
  green_create(&g1, test_threads, &a1);

  green_join(&g0);
  green_join(&g1);
  printf("done\n");
  return 0;
}
