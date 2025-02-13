#include <pthread.h>
#include <stdio.h>
#define NUM_THREADS 4

void *threadFunc(void *pArg) {
  int myNum = *((int *)pArg);
  printf("Thread Number: %d\n", myNum);
  pthread_exit(0);
}

int main() {
  pthread_t tid[NUM_THREADS];
  int tNum[NUM_THREADS];
  for (int i = 0; i < NUM_THREADS; i++) {
    tNum[i] = i;
    pthread_create(&tid[i], NULL, threadFunc, &tNum[i]);
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(tid[i], NULL);
  }

  return 0;
}
