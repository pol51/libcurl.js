#pragma once

#include "curl.h"

#ifdef  __cplusplus
extern "C" {
#endif

CURL *curl_easy_init(void);
CURLcode curl_easy_setopt(CURL *curl, CURLoption option, ...);
void curl_easy_cleanup(CURL *curl);

CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, ...);

#ifdef  __cplusplus
} /* end of extern "C" */
#endif
