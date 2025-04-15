#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 4096
#define HEADER_SKIP 54
#define OUTPUT_MODE 0600
#define INFILE "Santorini-Stega.bmp"
#define OUTFILE "Stegano.txt"

int main(int argc, char *argv[]) {
  int in_fd;
  in_fd = open(INFILE, O_RDONLY);
  if (in_fd < 0) {
    fprintf(stderr, "Error opening bmp infile.\n");
    exit(1);
  }

  int out_fd;
  out_fd = creat(OUTFILE, OUTPUT_MODE);
  if (out_fd < 0) {
    fprintf(stderr, "Error creating outfile.\n");
    exit(1);
  }

  char buffer[BUF_SIZE];
  /* exahusts 54 bytes */
  read(in_fd, buffer, HEADER_SKIP);
  // write(out_fd, buffer, HEADER_SKIP);

  int rd_count;
  int wt_count;
  size_t stega_buf_index = 0;

  /* read loop */

  char *stor_buff[BUF_SIZE];

  while (1) {
    rd_count = read(in_fd, buffer, BUF_SIZE);
    if (rd_count <= 0)
      break;

    for (size_t i = 0; i < rd_count; i++) {
    }

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
