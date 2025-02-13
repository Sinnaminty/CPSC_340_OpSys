#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void *print_message_function(void *ptr) {
  char *m;
  m = (char *)ptr;
  printf("%s", m);
  pthread_exit(0);
}

int main() {
  pthread_t t1, t2;

  char *m1 = "Hello\n";
  char *m2 = "World!\n";

  pthread_create(&t1, NULL, print_message_function, m1);

  pthread_create(&t2, NULL, print_message_function, m2);
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  return 0;
}
