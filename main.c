#include "filesystem.h"
#include "library.h"
#include "sourcekitd/sourcekitd.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *error_from_response(sourcekitd_response_t resp);
bool perform_sourcekit_request_from_yamlfile(char *filepath);

char *error_from_response(sourcekitd_response_t resp) {
  switch (skit_sourcekitd_response_error_get_kind(resp)) {
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

bool perform_sourcekit_request_from_yamlfile(char *filepath) {
  char *yaml = copy_file_contents(filepath);
  char *error = NULL;
  sourcekitd_object_t request =
      skit_sourcekitd_request_create_from_yaml(yaml, &error);
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

    return false;
  }

  sourcekitd_response_t response = skit_sourcekitd_send_request_sync(request);
  if (skit_sourcekitd_response_is_error(response)) {
    fprintf(stderr, "Got an error from sourcekitd: %s\n",
            error_from_response(response));
    skit_sourcekitd_response_dispose(response);

    return false;
  }

  sourcekitd_variant_t value = skit_sourcekitd_response_get_value(response);
  char *json = skit_sourcekitd_variant_json_description_copy(value);
  printf("%s\n", json);

  skit_sourcekitd_response_dispose(response);
  free(json);

  return true;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: skit [YAMLFILE...|-v|--version]\n");
    return EXIT_FAILURE;
  }

  if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
    printf("0.3.0\n");
    return EXIT_SUCCESS;
  }

  for (int i = 1; i < argc; i++) {
    char *filepath = argv[i];
    if (!file_readable(filepath)) {
      fprintf(stderr, "No readable file exists at '%s'\n", filepath);
      return EXIT_FAILURE;
    }
  }

  bool success = true;

  load_sourcekit_symbols();
  skit_sourcekitd_initialize();

  for (int i = 1; i < argc; i++) {
    char *filepath = argv[i];
    success = success && perform_sourcekit_request_from_yamlfile(filepath);
  }

  skit_sourcekitd_shutdown();

  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
