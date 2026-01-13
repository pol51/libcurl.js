#pragma once

#include "multi.h"

struct Curl_multi {
  curl_multi_timer_callback timer_cb;
  void *timer_userp;
  void *handles; // libcurl.js
};
