#include <stdio.h>
#include <pthread.h>
#include <assert.h>

volatile int flag = 0;
pthread_cond_t cond;
pthread_mutex_t mutex;

/**
* The fifth test: testing condition variables with mutex
* using pthread.
*
* OBS: since we are using several threads on the condition
* variable we must use pthread_cond_broadcast to make all
* the suspended threads contend again!
*/
void *test_condition_variables_with_mutex(void *arg){
  int id = *(int*) arg;
  //printf("thread %d\n", id);
  int loop = 10000;
  while(loop > 0){
    pthread_mutex_lock(&mutex);
    while(flag != id){
      //printf("flag %d thread %d\n", flag, id);
      pthread_cond_wait(&cond, &mutex);
    }
    flag = (id + 1) % 4;
    printf("thread %d: %d\n", id, loop);
    loop--;
    pthread_cond_broadcast(&cond); //OBS: see above
    pthread_mutex_unlock(&mutex);
  }
}

int main(){
  pthread_t g0, g1, g2, g3;
  int a0 = 0;
  int a1 = 1;
  int a2 = 2;
  int a3 = 3;

  pthread_cond_init(&cond, NULL);
  pthread_mutex_init(&mutex, NULL);

  pthread_create(&g0, NULL, test_condition_variables_with_mutex, &a0);
  pthread_create(&g1, NULL, test_condition_variables_with_mutex, &a1);
  pthread_create(&g2, NULL, test_condition_variables_with_mutex, &a2);
  pthread_create(&g3, NULL, test_condition_variables_with_mutex, &a3);

  pthread_join(g0, NULL);
  pthread_join(g1, NULL);
  pthread_join(g2, NULL);
  pthread_join(g3, NULL);
  printf("done\n");
  return 0;
}
