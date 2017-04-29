#include <sourcekitd/sourcekitd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *copy_file_contents(char *path);
char *error_from_response(sourcekitd_response_t resp);
int file_readable(char *path);
int perform_sourcekit_request_from_yamlfile(char *filepath);

char *error_from_response(sourcekitd_response_t resp) {
  switch (sourcekitd_response_error_get_kind(resp)) {
    case SOURCEKITD_ERROR_CONNECTION_INTERRUPTED:
      return "connection interrupted";
    case SOURCEKITD_ERROR_REQUEST_INVALID:
      return "request invalid";
    case SOURCEKITD_ERROR_REQUEST_FAILED:
      return "request failed";
    case SOURCEKITD_ERROR_REQUEST_CANCELLED:
      return "request cancelled";
  }
}

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

int perform_sourcekit_request_from_yamlfile(char *filepath) {
  char *yaml = copy_file_contents(filepath);
  char *error = NULL;
  sourcekitd_object_t request =
      sourcekitd_request_create_from_yaml(yaml, &error);
  free(yaml);

  if (!request) {
    if (error) {
      fprintf(stderr,
              "Got an error from sourcekitd when creating yaml request: %s\n",
              error);
      free(error);
    } else {
      fprintf(stderr, "sourcekitd failed to create request silently\n");
    }

    return 1;
  }

  sourcekitd_response_t response = sourcekitd_send_request_sync(request);
  if (sourcekitd_response_is_error(response)) {
    fprintf(stderr, "Got an error from sourcekitd: %s\n",
            error_from_response(response));
    sourcekitd_response_dispose(response);

    return 1;
  }

  sourcekitd_variant_t value = sourcekitd_response_get_value(response);
  char *json = sourcekitd_variant_json_description_copy(value);
  printf("%s\n", json);

  sourcekitd_response_dispose(response);
  free(json);

  return 0;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: skit [YAMLFILE|-v|--version]\n");
    return 1;
  }

  if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
    printf("0.1.2\n");
    return 0;
  }

  char *filepath = argv[1];
  if (!file_readable(filepath)) {
    fprintf(stderr, "No readable file exists at '%s'\n", filepath);
    return 1;
  }

  sourcekitd_initialize();
  int result = perform_sourcekit_request_from_yamlfile(filepath);
  sourcekitd_shutdown();

  return result;
}
