#include "curl.h"
#include "urldata.h"
#include <stdarg.h>
#include <stdlib.h>

CURLcode curl_global_init(long flags) { return CURLE_OK; }

CURL *curl_easy_init(void) { return calloc(1, sizeof(Curl_easy)); }

void curl_easy_cleanup(CURL *ptr) {
  free(((Curl_easy *)ptr)->state.url);
  free(((Curl_easy *)ptr)->set.range);
  free(ptr);
}

CURLcode curl_easy_getinfo(CURL *data, CURLINFO info, ...) {
  Curl_easy *handle = (Curl_easy *)data;
  va_list arg;
  void *paramp;

  va_start(arg, info);
  paramp = va_arg(arg, void *);

  switch (info) {
  case CURLINFO_PRIVATE:
    *(void **)paramp = handle->set.private_data;
    break;
  case CURLINFO_RESPONSE_CODE:
    *(long *)paramp = handle->info.httpcode;
  default:
    break;
  }

  va_end(arg);
  return CURLE_OK;
}
