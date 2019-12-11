#include <stdio.h>
#include "green.h"

int flag = 0;
green_cond_t cond;

/**
* The second test. Test for testing condition variables.
* OBS: Might fail without a mutex AND if we have timer
* interrupts enabled!
*/
void *test_condition_variables(void *arg){
  int id = *(int*) arg;
  int loop = 100000;
  while(loop > 0){
    if(flag == id){
      printf("thread %d: %d\n", id, loop);
      loop--;
      flag = (id + 1) % 4;
      green_cond_broadcast(&cond);//green_cond_signal(&cond);
    } else {
      green_cond_wait(&cond, NULL); //NULL because mutex are not used here.
    }
  }
}

int main(){
  green_t g0, g1, g2, g3;
  int a0 = 0;
  int a1 = 1;
  int a2 = 2;
  int a3 = 3;

  green_cond_init(&cond);

  green_create(&g0, test_condition_variables, &a0);
  green_create(&g1, test_condition_variables, &a1);
  green_create(&g2, test_condition_variables, &a2);
  green_create(&g3, test_condition_variables, &a3);

  green_join(&g0);
  green_join(&g1);
  green_join(&g2);
  green_join(&g3);
  printf("done\n");
  return 0;
}
