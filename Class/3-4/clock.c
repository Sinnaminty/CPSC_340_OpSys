#include <omp.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[]) {

  clock_t start;
  double time_used;

#pragma omp parallel
  {
    printf("I am a parallel region.\n");
  }

  return 0;
}
