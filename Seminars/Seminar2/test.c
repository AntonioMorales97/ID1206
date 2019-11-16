#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "ptmall.h"
#include "rand.h"

//#define ROUNDS 1000
//#define LOOP 1000
//#define BUFFER 100

int ROUNDS; //
int LOOP; // number of blocks requested
int BUFFER; //how many blocks we can hold at a given time
int MAX_REQUEST_SIZE; // max block size request

void test(int bufferSize){
  void *buffer[bufferSize];
  for(int i = 0; i < bufferSize; i++){
    buffer[i] = NULL;
  }

  //for(int j = 0; j < ROUNDS; j++){
    for(int i = 0; i < LOOP; i++){

      int index = rand() % bufferSize;
      if(buffer[index] != NULL){
        pree(buffer[index]);
      }

      size_t size = (size_t)request(MAX_REQUEST_SIZE);
      int *memory;
      memory = palloc(size);

      if(memory == NULL){
        fprintf(stderr, "palloc failed\n");
        return;
        //exit(1);
      }
      buffer[index] = memory;
      /* writing to the memory so we know it exists*/
      *memory = 123;
    }
  //}
}

void evalFlistLength(){
  printf("# Checking length of flist\n# BufferSize\tflistLength\n");
  for(int i = 10; i < BUFFER; i+= 10){
    init();
    test(i);
    bench1(i);
    terminate();
  }
  init();
  test(BUFFER);
  bench1(BUFFER);
  terminate();
}

void evalFlistDistr(){
  printf("# Checking distribution of the block sizes in flist\n");
  init();
  test(BUFFER);
  bench11(BUFFER);
  terminate();
}

void testSanity(){
  init();
  test(BUFFER);
  sanity();
  terminate();
}

int main(int argc, char const *argv[]){
  //printf("%d\n", argc);
  if(argc < 4){
    printf("Please enter:\tBuffer size\tLoops\tMax size of request\n");
    exit(1);
  }
  BUFFER = atoi(argv[1]);
  //ROUNDS = atoi(argv[2]);
  LOOP = atoi(argv[2]);
  MAX_REQUEST_SIZE = atoi(argv[3]);
  srand(time(0));
  //evalFlistLength();
  evalFlistDistr();
  //testSanity();
  /*
  init();
  test();
  sanity();
  bench1();
  terminate();
  */
  /*
  void *test1 = palloc(100);
  printf("Palloced\n");
  sanity();
  printf("Preeing\n");
  pree(test);
  pree(test1);
  printf("Preed\n");
  sanity();
  */

  /*
  init();
  test();
  sanity();
  bench1();
  terminate();
  */
  return 0;
}
