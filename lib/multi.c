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

/* Deliver the response headers to the callback set with
   CURLOPT_HEADERFUNCTION, mirroring curl: one invocation for the status line,
   one per response header line, and one for the trailing blank line. If
   HEADERFUNCTION is unset but HEADERDATA is set, header lines fall back to the
   CURLOPT_WRITEFUNCTION callback. Nothing is delivered when no response was
   received (e.g. a timeout, where status is 0). */
static void deliver_headers(Curl_easy *handle, emscripten_fetch_t *fetch) {
  if (fetch->status == 0) {
    return;
  }

  curl_write_callback cb = handle->set.fwrite_header;
  void *userdata = handle->set.writeheader;
  if (cb == NULL) {
    if (userdata == NULL) {
      return;
    }
    cb = handle->set.fwrite_func;
  }
  if (cb == NULL) {
    return;
  }

  /* The HTTP version is not exposed by the Fetch API, so it is hardcoded. */
  char status_line[128];
  int n = snprintf(status_line, sizeof(status_line), "HTTP/1.1 %d %s\r\n",
                   (int)fetch->status, fetch->statusText);
  if (n > 0) {
    cb(status_line, (size_t)n, 1, userdata);
  }

  size_t len = emscripten_fetch_get_response_headers_length(fetch);
  if (len > 0) {
    char *buf = malloc(len + 1);
    if (buf) {
      emscripten_fetch_get_response_headers(fetch, buf, len + 1);
      char *line = buf;
      while (*line) {
        char *eol = strchr(line, '\n');
        size_t linelen = eol ? (size_t)(eol - line + 1) : strlen(line);
        cb(line, linelen, 1, userdata);
        if (!eol) {
          break;
        }
        line = eol + 1;
      }
      free(buf);
    }
  }

  cb((char *)"\r\n", 2, 1, userdata);
}

void process_result(emscripten_fetch_t *fetch) {
  Curl_easy *handle = fetch->userData;
  struct Curl_multi *multi = handle->state.multi;

  handle->info.httpcode = fetch->status;

  deliver_headers(handle, fetch);

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
