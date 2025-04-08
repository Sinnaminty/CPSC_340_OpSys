#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 4096
#define OUTPUT_MODE 0600

int main(int argc, char *argv[]) {
  int in_fd, out_fd, rd_count, wt_count;
  char buffer[BUF_SIZE];

  if (argc != 3) {
    fprintf(stderr, "Must Contain three args.\n");
    exit(1);
  }

  in_fd = open(argv[1], O_RDONLY);
  if (in_fd < 0) {
    fprintf(stderr, "Error opening infile.\n");
    exit(1);
  }
  out_fd = creat(argv[2], OUTPUT_MODE);
  if (out_fd < 0) {
    fprintf(stderr, "Error creating outfile.\n");
    exit(1);
  }

  /* read loop */
  while (1) {
    rd_count = read(in_fd, buffer, BUF_SIZE);
    if (rd_count <= 0)
      break;

    wt_count = write(out_fd, buffer, rd_count);
    if (wt_count <= 0) {
      fprintf(stderr, "Write Error.\n");
      exit(1);
    }
  }

  close(in_fd);
  close(out_fd);

  if (rd_count == 0) {
    printf("Copy Successful\n");
    exit(0);
  } else {
    fprintf(stderr, "Last Read Error.\n");
    exit(1);
  }
}
