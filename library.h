#include "sourcekitd/sourcekitd.h"

void load_sourcekit_symbols(void);

extern bool (*skit_sourcekitd_response_is_error)(sourcekitd_response_t obj);
extern char *(*skit_sourcekitd_variant_json_description_copy)(
    sourcekitd_variant_t obj);
extern sourcekitd_error_t (*skit_sourcekitd_response_error_get_kind)(
    sourcekitd_response_t err);
extern sourcekitd_object_t (*skit_sourcekitd_request_create_from_yaml)(
    const char *yaml, char **error);
extern sourcekitd_response_t (*skit_sourcekitd_send_request_sync)(
    sourcekitd_object_t req);
extern sourcekitd_variant_t (*skit_sourcekitd_response_get_value)(
    sourcekitd_response_t resp);
extern void (*skit_sourcekitd_initialize)(void);
extern void (*skit_sourcekitd_response_dispose)(sourcekitd_response_t obj);
extern void (*skit_sourcekitd_shutdown)(void);
