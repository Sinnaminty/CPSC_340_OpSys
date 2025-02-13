#include <pthread.h>
#include <stdio.h>
#define NUM_THREADS 10

void *hello(void *arg) {
  printf("I'm a thread!\n");
  pthread_exit(0);
}

int main() {
  pthread_t tid[NUM_THREADS];
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_create(&tid[i], NULL, hello, NULL);
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(tid[i], NULL);
  }

  return 0;
}
