#include "curl.h"
#include "multihandle.h"
#include "urldata.h"
#include "util.h"
#include <emscripten/fetch.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

CURLM *curl_multi_init(void) {
  struct Curl_multi *multi = calloc(1, sizeof(struct Curl_multi));
  multi->handles = malloc_handles();
  return multi;
}

void process_result(emscripten_fetch_t *fetch) {
  Curl_easy *handle = fetch->userData;
  struct Curl_multi *multi = handle->state.multi;

  handle->info.httpcode = fetch->status;
  if (fetch->status == 200) {
    handle->set.fwrite_func((char *)fetch->data, fetch->numBytes, 1,
                            handle->set.out);
  }
  add_handle(multi->handles, handle);

  emscripten_fetch_close(fetch);
  multi->timer_cb(multi, 0, multi->timer_userp);
}

CURLMcode curl_multi_add_handle(CURLM *m, CURL *d) {
  struct Curl_multi *multi = m;
  Curl_easy *handle = d;
  handle->state.multi = multi;

  emscripten_fetch_attr_t attr;
  emscripten_fetch_attr_init(&attr);
  attr.timeoutMSecs = handle->set.timeout;
  attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
  attr.userData = handle;
  strcpy(attr.requestMethod, "GET");
  attr.onsuccess = process_result;
  attr.onerror = process_result;
  emscripten_fetch(&attr, handle->state.url);
  return CURLM_OK;
}

CURLMcode curl_multi_remove_handle(CURLM *m, CURL *d) { return CURLM_OK; }

CURLMcode curl_multi_cleanup(CURLM *m) {
  free_handles(((struct Curl_multi *)m)->handles);
  free(m);
  return CURLM_OK;
}

CURLMsg *curl_multi_info_read(CURLM *m, int *msgs_in_queue) {
  static CURLMsg msg;
  Curl_easy *handle = pop_handle(((struct Curl_multi *)m)->handles);
  if (handle == NULL) {
    return NULL;
  }
  msg.msg = CURLMSG_DONE;
  msg.easy_handle = handle;
  msg.data.result =
      handle->info.httpcode == 200 ? CURLE_OK : CURLE_HTTP_RETURNED_ERROR;
  return &msg;
}

CURLMcode curl_multi_setopt(CURLM *m, CURLMoption option, ...) {
  va_list param;
  struct Curl_multi *multi = m;
  va_start(param, option);
  switch (option) {
  case CURLMOPT_TIMERFUNCTION:
    multi->timer_cb = va_arg(param, curl_multi_timer_callback);
    break;
  case CURLMOPT_TIMERDATA:
    multi->timer_userp = va_arg(param, void *);
    break;
  default:
    break;
  }
  return CURLM_OK;
}

CURLMcode curl_multi_socket_action(CURLM *m, curl_socket_t s, int ev_bitmask,
                                   int *running_handles) {
  return CURLM_OK;
}
