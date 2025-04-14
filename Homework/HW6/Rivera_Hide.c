#include <fcntl.h>
#include <stddef.h>
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

void print_binary(char c) {
  for (int i = 7; i >= 0; i--) {
    printf("%d", (c << i) & 1);
  }
  printf("\n");
}

long get_file_size(const char *fn) {
  struct stat file_stats;
  if (stat(fn, &file_stats) == 0) {
    return file_stats.st_size;
  } else {
    fprintf(stderr, "get_file_size error!\n");
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  int in_fd;
  int rd_count;

  long size = get_file_size(INFILE_TXT);
  printf("File size of %s: %ld bytes\n", INFILE_TXT, size);

  size_t stega_stream_buffer_size = (size + 7) / 8;
  printf("stega_stream_buffer_size: %ld\n", stega_stream_buffer_size);
  char stega_stream[stega_stream_buffer_size];

  // buffer init 0
  for (size_t i = 0; i < stega_stream_buffer_size; i++) {
    stega_stream[i] = 0;
  }

  in_fd = open(INFILE_TXT, O_RDONLY);
  if (in_fd < 0) {
    fprintf(stderr, "Error opening txt infile.\n");
    exit(1);
  }

  char buffer[BUF_SIZE];
  /** read loop for breaking txt into a stream */
  while (1) {
    size_t buf_pointer = 0;
    rd_count = read(in_fd, buffer, 50);
    if (rd_count <= 0)
      break;

    for (size_t i = 0; i < rd_count; i++) {
      // isolates lsb
      char lsb = buffer[i] & 0x01;
      size_t byte_index = i / 8;
      size_t bit_index = i % 8;
      printf("lsb: %d\n", lsb);
      printf("modified lsb: ");
      print_binary(lsb << 7 - bit_index);
      printf("\n");
      stega_stream[byte_index + buf_pointer] |= (lsb << 7 - bit_index);
      print_binary(stega_stream[byte_index]);
    }
    exit(0);
    buf_pointer += rd_count;
  }

  // hopefully all of the bytes we care about are in stega_stream;
  close(in_fd);

  printf("Packed LSB buffer: (in bits): ");
  for (size_t i = 0; i < size; i++) {
    size_t byte_index = i / 8;
    size_t bit_index = i % 8;
    char bit = (stega_stream[byte_index] >> bit_index) & 0x01;
    printf("%d", bit);
  }
  printf("\n");
  exit(0);
  ///////
  in_fd = open(INFILE_BMP, O_RDONLY);
  if (in_fd < 0) {
    fprintf(stderr, "Error opening bmp infile.\n");
    exit(1);
  }

  int out_fd;
  int wt_count;

  out_fd = creat(OUTFILE, OUTPUT_MODE);
  if (out_fd < 0) {
    fprintf(stderr, "Error creating outfile.\n");
    exit(1);
  }

  /* exahusts 54 bytes */
  read(in_fd, buffer, HEADER_SKIP);
  write(out_fd, buffer, HEADER_SKIP);

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
