#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 4096
#define HEADER_SKIP 54
#define OUTPUT_MODE 0600
#define INFILE_BMP "Santorini.bmp"
#define INFILE_TXT "nVidia-openAcc.txt"
#define OUTFILE "Santorini-Stega.bmp"

int main(int argc, char *argv[]) {
  int in_bmp_fd, in_txt_fd, out_fd, rd_count, wt_count;
  char bmp_buffer[BUF_SIZE], txt_buffer[BUF_SIZE];

  in_bmp_fd = open(INFILE_BMP, O_RDONLY);
  if (in_bmp_fd < 0) {
    fprintf(stderr, "Error opening infile.\n");
    exit(1);
  }

  out_fd = creat(OUTFILE, OUTPUT_MODE);
  if (out_fd < 0) {
    fprintf(stderr, "Error creating outfile.\n");
    exit(1);
  }

  /* exahusts 54 bytes */
  read(in_bmp_fd, bmp_buffer, HEADER_SKIP);
  write(out_fd, bmp_buffer, HEADER_SKIP);

  /* read loop */
  while (1) {
    rd_count = read(in_bmp_fd, bmp_buffer, BUF_SIZE);
    if (rd_count <= 0)
      break;

    for (int i = 0; i < rd_count; i++) {
      bmp_buffer[i] = bmp_buffer[i] & 0xFE;
    }

    wt_count = write(out_fd, bmp_buffer, rd_count);
    if (wt_count <= 0) {
      fprintf(stderr, "Write Error.\n");
      exit(1);
    }
  }

  close(in_bmp_fd);
  close(out_fd);

  if (rd_count == 0) {
    printf("Copy Successful\n");
    exit(0);
  } else {
    fprintf(stderr, "Last Read Error.\n");
    exit(1);
  }
}
