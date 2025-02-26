#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_CAP 500

int currentOccupancy = 0;
pthread_mutex_t mutex;

struct Args {
  char *m_name;
  int m_capacity;
};

void *thread_func(void *pArgs) {
  struct Args args;
  args = *((struct Args *)pArgs);

  while (args.m_capacity > 0) {
    if (currentOccupancy < MAX_CAP) {
      pthread_mutex_lock(&mutex);
      currentOccupancy++;
      pthread_mutex_unlock(&mutex);
      args.m_capacity--;

      if (args.m_capacity % 10 == 0) {
        printf("Gate %s capacity: %d | Current Occupancy: %d\n", args.m_name,
               args.m_capacity, currentOccupancy);
      }
      usleep(10000);

    } else {
      printf("MAX CAP REACHED! %s turned away %d people! Womp womp!\n",
             args.m_name, args.m_capacity);
      pthread_exit(0);
    }
  }

  printf("Gate %s is now closed!\n", args.m_name);

  pthread_exit(0);
}

int main() {
  pthread_t pThreads[8];

  struct Args eastGateArgs = {"E", 60};
  struct Args northEastGateArgs = {"NE", 100};
  struct Args northGateArgs = {"N", 40};
  struct Args northWestGateArgs = {"NW", 100};
  struct Args westGateArgs = {"W", 50};
  struct Args southWestGateArgs = {"SW", 50};
  struct Args southGateArgs = {"S", 50};
  struct Args southEastGateArgs = {"SE", 50};

  struct Args pThreadArgs[] = {
      eastGateArgs, northEastGateArgs, northGateArgs, northWestGateArgs,
      westGateArgs, southWestGateArgs, southGateArgs, southEastGateArgs};

  for (int i = 0; i < 8; i++) {
    pthread_create(&pThreads[i], NULL, &thread_func, (void *)(&pThreadArgs[i]));
  }

  for (int i = 0; i < 8; i++) {
    pthread_join(pThreads[i], NULL);
  }

  printf("Final Occupancy: %d\n", currentOccupancy);

  pthread_mutex_destroy(&mutex);

  return 0;
}
