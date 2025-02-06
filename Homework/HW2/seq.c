#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * Aiden Rivera
 * seq.c
 * 2/6/25
 */

int main() {
  pid_t pid = getpid();
  int i;
  printf("Sequential Process Creation: Original PID: %d\n", pid);

  for (i = 0; i < 3; i++) {
    pid = fork();
    if (pid < 0) {
      fprintf(stderr, "Fork Failed!\n");
      exit(1);
    }
    if (pid == 0) {
      // We're the child.
      printf("Child: %d created by Parent: %d\n", getpid(), getppid());
      sleep(1);
    } else {
      // We're the parent.
      wait(NULL);
      break;
    }
  }

  return 0;
}
