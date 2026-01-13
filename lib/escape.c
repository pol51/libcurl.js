#include "curl.h"
#include <emscripten.h>
#include <stdlib.h>
#include <string.h>

char *curl_escape(const char *str, int length) {
  if (!str) {
    return NULL;
  }

  if (length < 0) {
    length = (int)strlen(str);
  }

  return (char *)EM_ASM_PTR(
      // clang-format off
    {
        const input = UTF8ToString($0, $1);
        const escaped = encodeURIComponent(input);
        const size = lengthBytesUTF8(escaped) + 1;
        const ptr = _malloc(size);
        if (!ptr) {
            return 0;
        }
        stringToUTF8(escaped, ptr, size);
        return ptr;
    },
      // clang-format on
      str, length);
}

void curl_free(void *p) { free(p); }
