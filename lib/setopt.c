#include "curl.h"
#include "urldata.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

CURLcode curl_easy_setopt(CURL *d, CURLoption tag, ...) {
  va_list arg;
  CURLcode result;
  Curl_easy *data = d;

  va_start(arg, tag);
  switch (tag) {
  case CURLOPT_TIMEOUT:
    data->set.timeout = va_arg(arg, unsigned int) * 1000;
    break;
  case CURLOPT_WRITEDATA:
    data->set.out = va_arg(arg, void *);
    break;
  case CURLOPT_URL:
    free(data->state.url);
    data->state.url = strdup(va_arg(arg, char *));
    break;
  case CURLOPT_PRIVATE:
    data->set.private_data = va_arg(arg, void *);
    break;
  case CURLOPT_WRITEFUNCTION:
    data->set.fwrite_func = va_arg(arg, curl_write_callback);
    break;
  default:
    break;
  }
  va_end(arg);
  return CURLE_OK;
}
