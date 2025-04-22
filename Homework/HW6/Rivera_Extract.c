#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

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

  /* exahusts 54 bytes */
  lseek(in_fd, HEADER_SKIP, SEEK_SET);

  unsigned char bmp_byte;
  unsigned char ch = 0;
  int bit_count = 0;

  while (read(in_fd, &bmp_byte, 1) == 1) {
    /* extract LSB and shift it into correct position */
    ch = (ch << 1) | (bmp_byte & 0x01);
    bit_count++;

    if (bit_count == 8) {
      if (ch == '\0') {
        break; /* we are donneeee*/
      }
      write(out_fd, &ch, 1);
      bit_count = 0;
      ch = 0;
    }
  }
  close(in_fd);
  close(out_fd);
  printf("Extraction Successful.\n");

  exit(0);
}
