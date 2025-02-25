#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int value = 5;
int main() {
  pid_t pid;
  pid = fork();

  if (pid == 0) {
    value += 15;
    return 0;
  } else if (pid > 0) {
    wait(NULL);
    printf("PARENT: value = %d", value);
    return 0;
  }
}
