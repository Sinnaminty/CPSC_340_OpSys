#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * Aiden Rivera
 * fan.c
 * 2/6/25
 */

int main() {
  pid_t pid = getpid();
  int i;
  printf("Fan Process Creation: Original PID: %d\n", pid);

  pid = fork();
  if (pid < 0) {
    fprintf(stderr, "Fork Failed!\n");
    exit(1);
  }
  if (pid == 0) {
    printf("Child: %d created by Parent: %d\n", getpid(), getppid());
    sleep(1);

    for (i = 0; i < 3; i++) {
      pid = fork();
      if (pid < 0) {
        fprintf(stderr, "Fork Failed!\n");
        exit(1);

      } else if (pid == 0) {
        // We're the child.
        printf("Child: %d created by Parent: %d\n", getpid(), getppid());
        exit(0);

      } else {
        // to keep the parent from exploding
        wait(NULL);
      }
    }
  } else {
    wait(NULL);
    exit(0);
  }

  return 0;
}
