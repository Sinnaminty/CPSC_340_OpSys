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
#define INFILE_BMP "Santorini.bmp"
#define INFILE_TXT "nVidia-openACC.txt"
#define OUTFILE "Santorini-Stega.bmp"

int main(int argc, char *argv[]) {
  char *stega_buf = 0;
  long stega_buf_len;
  FILE *f = fopen(INFILE_TXT, "rb");
  if (f) {
    fseek(f, 0, SEEK_END);
    stega_buf_len = ftell(f);
    fseek(f, 0, SEEK_SET);
    stega_buf = (char *)malloc((stega_buf_len + 1) * sizeof(char));
    if (stega_buf) {
      fread(stega_buf, sizeof(char), stega_buf_len, f);
    }
    fclose(f);
  }

  // null terminated :D
  stega_buf[stega_buf_len] = '\0';

  int in_fd;
  in_fd = open(INFILE_BMP, O_RDONLY);
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
  write(out_fd, buffer, HEADER_SKIP);

  int rd_count;
  int wt_count;
  size_t stega_buf_index = 0;

  /* read loop */

  while (1) {
    rd_count = read(in_fd, buffer, BUF_SIZE);
    if (rd_count <= 0)
      break;

    // inject bits into our buffer.
    if (stega_buf_index < stega_buf_len) {
      for (size_t i = 0; i < rd_count; i++) {
        size_t byte_index = i / 8;
        size_t bit_index = i % 8;
        if (byte_index != 0 && byte_index % 8 == 0) {
          stega_buf_index++;
        }

        buffer[i] |= (stega_buf[stega_buf_index] >> (7 - bit_index)) & 0x01;
      }
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
