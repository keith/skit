#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char *copy_file_contents(char *path);
int file_readable(char *path);

int file_readable(char *path) {
  return access(path, R_OK) == 0;
}

char *copy_file_contents(char *path) {
  FILE *fp = fopen(path, "r");
  if (!fp) {
    fclose(fp);
    fprintf(stderr, "Failed to open file at '%s'\n", path);
    exit(1);
  }

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  rewind(fp);
  char *buffer = malloc((unsigned long)size + 1);
  if (!buffer) {
    fclose(fp);

    fprintf(stderr, "Failed to allocate memory for reading '%s'\n", path);
    exit(1);
  }

  if (!fread(buffer, (size_t)size, 1, fp)) {
    fclose(fp);
    free(buffer);

    fprintf(stderr, "Failed to read file '%s'\n", path);
    exit(1);
  }

  fclose(fp);
  buffer[size] = '\0';
  return buffer;
}
