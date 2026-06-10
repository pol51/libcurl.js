#pragma once

#include "curl.h"

struct PureInfo {
  int httpcode;
};

struct UrlState {
  char *url;
  void *multi; // libcurl.js
};

struct UserDefined {
  void *out;
  curl_write_callback fwrite_func;
  curl_write_callback fwrite_header;
  void *writeheader;
  void *private_data;
  unsigned int timeout;
  char *range;
};

typedef struct {
  struct UserDefined set;
  struct UrlState state;
  struct PureInfo info;
} Curl_easy;
