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
#define INFILE_BMP "Santorini.bmp"
#define INFILE_TXT "nVidia-openACC.txt"
#define OUTFILE "Santorini-Stega.bmp"

int main(int argc, char *argv[]) {
  FILE *txt_file = fopen(INFILE_TXT, "rb");
  if (!txt_file) {
    fprintf(stderr, "Error Opening Text File.\n");
    exit(1);
  }

  fseek(txt_file, 0, SEEK_END);
  long text_len = ftell(txt_file);
  fseek(txt_file, 0, SEEK_SET);

  /* include space for null terminator */
  char *text_buf = (char *)malloc(text_len + 1);
  if (!text_buf) {
    fprintf(stderr, "Malloc Error!!\n");
    fclose(txt_file);
    exit(1);
  }

  fread(text_buf, 1, text_len, txt_file);
  fclose(txt_file);

  text_buf[text_len] = '\0';

  int in_fd = open(INFILE_BMP, O_RDONLY);
  if (in_fd < 0) {
    fprintf(stderr, "Error opening bmp file.\n");
    exit(1);
  }

  int out_fd = creat(OUTFILE, OUTPUT_MODE);
  if (out_fd < 0) {
    fprintf(stderr, "Error creating output bmp file.\n");
    close(in_fd);
    exit(1);
  }

  /* copy header */
  char header[HEADER_SKIP];
  if (read(in_fd, header, HEADER_SKIP) != HEADER_SKIP) {
    fprintf(stderr, "Error reading bmp header.\n");
    exit(1);
  }
  write(out_fd, header, HEADER_SKIP);

  unsigned char bmp_byte;
  size_t text_index = 0;
  int bit_index = 7;

  while (read(in_fd, &bmp_byte, 1) == 1) {
    if (text_index <= text_len) {
      /* clear LSB */
      bmp_byte &= 0xFE;
      /* then set it */
      bmp_byte |= (text_buf[text_index] >> bit_index) & 0x01;

      bit_index--;
      if (bit_index < 0) {
        bit_index = 7;
        text_index++;
      }
    }

    if (write(out_fd, &bmp_byte, 1) != 1) {
      fprintf(stderr, "Write Error.\n");
      exit(1);
    }
  }

  close(in_fd);
  close(out_fd);
  free(text_buf);

  printf("Embedding Successful.\n");
  exit(0);
}
