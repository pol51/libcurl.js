#pragma once

#include "urldata.h"
#ifdef __cplusplus
extern "C" {
#endif

void *malloc_handles();
void free_handles(void *handles);
void add_handle(void *handles, Curl_easy *handle);
Curl_easy *pop_handle(void *handles);
void foreach_handle(void *handles, void (*func)(Curl_easy *handle));

#ifdef __cplusplus
} /* end of extern "C" */
#endif
