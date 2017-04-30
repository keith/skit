#include "library.h"
#include "filesystem.h"
#include "sourcekitd/sourcekitd.h"
#include <assert.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENV_VAR "SOURCEKIT_BINARY_PATH"

// Bind a given symbol from a dlopen'd binary to skit_$symbol
// and then check for errors
#define BIND(symbol)                                                           \
  *(void **)(&skit_##symbol) = dlsym(handle, #symbol);                         \
  check_dlerror();

char *copy_xcode_select_sourcekit_path(void);
char *sourcekit_binary_path(void);
void check_dlerror(void);
void load_sourcekit_symbols(void);

bool (*skit_sourcekitd_response_is_error)(sourcekitd_response_t obj);
char *(*skit_sourcekitd_variant_json_description_copy)(
    sourcekitd_variant_t obj);
sourcekitd_error_t (*skit_sourcekitd_response_error_get_kind)(
    sourcekitd_response_t err);
sourcekitd_object_t (*skit_sourcekitd_request_create_from_yaml)(
    const char *yaml, char **error);
sourcekitd_response_t (*skit_sourcekitd_send_request_sync)(
    sourcekitd_object_t req);
sourcekitd_variant_t (*skit_sourcekitd_response_get_value)(
    sourcekitd_response_t resp);
void (*skit_sourcekitd_initialize)(void);
void (*skit_sourcekitd_response_dispose)(sourcekitd_response_t obj);
void (*skit_sourcekitd_shutdown)(void);

void check_dlerror() {
  char *error = dlerror();
  if (error) {
    fprintf(stderr, "%s\n", error);
    exit(EXIT_FAILURE);
  }
}

char *sourcekit_binary_path() {
#ifdef __APPLE__
  char *options[3] = {
      getenv(ENV_VAR),
      "/Applications/Xcode.app/Contents/Developer/Toolchains/"
      "XcodeDefault.xctoolchain/usr/lib/sourcekitd.framework/sourcekitd",
      "/Applications/Xcode-Beta.app/Contents/Developer/Toolchains/"
      "XcodeDefault.xctoolchain/usr/lib/sourcekitd.framework/sourcekitd",
  };
#else
  char *options[2] = {
      getenv(ENV_VAR), "/usr/lib/libsourcekitdInProc.so",
  };
#endif

  int count = sizeof(options) / sizeof(char *);
  for (int i = 0; i < count; i++) {
    char *path = options[i];
    if (path && file_readable(path)) {
      return path;
    }
  }

  return NULL;
}

char *copy_xcode_select_sourcekit_path() {
  FILE *fp = popen("xcode-select -p", "r");
  if (!fp) {
    return NULL;
  }

  char *path = malloc(200);
  fgets(path, 200, fp);
  if (ferror(fp)) {
    return NULL;
  }

  strtok(path, "\n");
  strcat(path, "/Toolchains/XcodeDefault.xctoolchain/usr/lib/"
               "sourcekitd.framework/sourcekitd");

  return path;
}

void load_sourcekit_symbols() {
  void *handle;
  char *xcode_select_path = NULL;
#ifdef __APPLE__
  if (getenv(ENV_VAR) == NULL) {
    xcode_select_path = copy_xcode_select_sourcekit_path();
  }
#endif

  if (xcode_select_path) {
    handle = dlopen(xcode_select_path, RTLD_LAZY);
    free(xcode_select_path);
  } else {
    char *path = sourcekit_binary_path();
    if (!path) {
      fprintf(stderr,
              "Couldn't find readable sourcekitd binary path. Set the "
              "'%s' to override this.\n",
              ENV_VAR);
      exit(EXIT_FAILURE);
    }

    handle = dlopen(path, RTLD_LAZY);
  }

  if (!handle) {
    fprintf(stderr, "Failed to open sourcekit framework '%s'\n", dlerror());
    exit(EXIT_FAILURE);
  }

  dlerror();

  BIND(sourcekitd_initialize);
  BIND(sourcekitd_request_create_from_yaml);
  BIND(sourcekitd_response_dispose);
  BIND(sourcekitd_response_error_get_kind);
  BIND(sourcekitd_response_get_value);
  BIND(sourcekitd_response_is_error);
  BIND(sourcekitd_send_request_sync);
  BIND(sourcekitd_shutdown);
  BIND(sourcekitd_variant_json_description_copy);
}
