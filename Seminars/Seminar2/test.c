#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "ptmall.h"
#include "rand.h"

int allocs; // number of blocks requested
int buffer; //how many blocks we can hold at a given time
int maxReqSize; // max block size request

void test(int bufferSize){
  void *buffer[bufferSize];
  for(int i = 0; i < bufferSize; i++){
    buffer[i] = NULL;
  }

  for(int i = 0; i < allocs; i++){
    int index = rand() % bufferSize;
    if(buffer[index] != NULL){
      pree(buffer[index]);
    }

    size_t size = (size_t)request(maxReqSize);
    int *memory;
    memory = palloc(size);

    if(memory == NULL){
      fprintf(stderr, "palloc failed\n");
      return;
    }
    buffer[index] = memory;
    /* writing to the memory so we know it exists */
    *memory = 123;
  }
}

void evalFlistLength(){
  printf("# Checking length of flist\n# BufferSize\tflistLength\n");
  for(int i = 10; i < buffer; i+= 10){
    init();
    test(i);
    printCountLengthOfFlist(i);
    terminate();
  }
  init();
  test(buffer);
  printCountLengthOfFlist(buffer);
  terminate();
}

void evalFlistDistr(){
  printf("# Checking distribution of the block sizes in flist\n");
  init();
  test(buffer);
  printSizeDistributionOfFlist(buffer);
  terminate();
}

void testSanity(){
  init();
  test(buffer);
  sanity();
  terminate();
}

int main(int argc, char const *argv[]){
  if(argc < 4){
    printf("Please enter:\tBuffer size\tLoops\tMax size of request\n");
    exit(1);
  }
  buffer = atoi(argv[1]);
  allocs = atoi(argv[2]);
  maxReqSize = atoi(argv[3]);
  srand(time(0));
  //evalFlistLength();
  //evalFlistDistr();
  testSanity();
  return 0;
}
