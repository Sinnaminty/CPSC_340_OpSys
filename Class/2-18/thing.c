#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_RAND 100
#define MAX_CAP 350

int globalCounter = 0;
pthread_mutex_t mutex;

struct Args {
  char *m_name;
  int m_capacity;
};

// ptr = int
void *thread_func(void *pArgs) {
  struct Args args;
  args = *((struct Args *)pArgs);

  while (args.m_capacity > 0) {
    if (globalCounter < MAX_CAP) {
      pthread_mutex_lock(&mutex);
      globalCounter++;
      pthread_mutex_unlock(&mutex);
      // between 1 and 100
      int random = (1 + (rand() % MAX_RAND)) * 100000;

      // printf("%s is waiting for %d...\n", args.m_name, random);

      for (int i = 0; i < random; i++) {
        // simulating a wait.
        ;
      }

      args.m_capacity--;
      // printf("gateName: %s - gateCapacity: %d - globalCounter: %d\n",
      // args.m_name,
      //       args.m_capacity, globalCounter);
    } else {
      printf("MAX CAP REACHED! %s turned away %d people! Womp womp!\n",
             args.m_name, args.m_capacity);
      pthread_exit(0);
    }
  }

  printf("%s All done! globalCounter: %d\n", args.m_name, globalCounter);

  pthread_exit(0);
}

int main() {

  pthread_t g1;
  pthread_t g2;
  pthread_t g3;
  pthread_t g4;
  pthread_t pThreads[] = {g1, g2, g3, g4};

  struct Args g1Args = {"Gate 1", 100};
  struct Args g2Args = {"Gate 2", 100};
  struct Args g3Args = {"Gate 3", 100};
  struct Args g4Args = {"Gate 4", 100};
  struct Args pThreadArgs[] = {g1Args, g2Args, g3Args, g4Args};

  for (int i = 0; i < 4; i++) {
    pthread_create(&pThreads[i], NULL, &thread_func, (void *)(&pThreadArgs[i]));
  }

  for (int i = 0; i < 4; i++) {
    pthread_join(pThreads[i], NULL);
  }

  printf("Final Capacity: %d\n", globalCounter);
  pthread_mutex_destroy(&mutex);

  return 0;
}
