#include "multihandle.h"
#include <curl.h>
#include <emscripten.h>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

static int count = 0;

class MultiContainer {
public:
  MultiContainer() : multi_(curl_multi_init()) {
    curl_multi_setopt(multi_, CURLMOPT_TIMERFUNCTION, staticTimer);
    curl_multi_setopt(multi_, CURLMOPT_TIMERDATA, this);
  }

  ~MultiContainer() {
    auto handles =
        (std::unordered_set<void *> *)((struct Curl_multi *)multi_)->handles;
    std::cout << "Remaining handles: " << handles->size() << std::endl;
    curl_multi_cleanup(multi_);
    std::cout << "Completed requests: " << count << std::endl;
  }

  CURLM *multi_;

private:
  static int staticTimer(CURLM *multi, long timeout_ms, void *userp) {
    MultiContainer *self = static_cast<MultiContainer *>(userp);
    return self->timer(multi, timeout_ms);
  }

  int timer(CURLM *multi, long timeout_ms) {
    CURLMcode mcode;
    int still_running;
    do {
      mcode = curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0,
                                       &still_running);
    } while (mcode == CURLM_CALL_MULTI_PERFORM);

    int num_messages;
    CURLMsg *curl_message;
    while ((curl_message = curl_multi_info_read(multi_, &num_messages)) !=
           nullptr) {
      if (curl_message->msg == CURLMSG_DONE) {
        curl_multi_remove_handle(multi_, curl_message->easy_handle);
        int httpCode = 0;
        curl_easy_getinfo(curl_message->easy_handle, CURLINFO_RESPONSE_CODE,
                          &httpCode);
        std::cout << "HTTP code: " << httpCode << std::endl;
        ++count;
      }
    }
    return 0;
  }
};

static MultiContainer container;

class CurlTest {
public:
  CurlTest() : curl_(curl_easy_init()) {
    curl_easy_setopt(curl_, CURLOPT_PRIVATE, this);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, staticWrite);
    curl_easy_setopt(curl_, CURLOPT_TIMEOUT, 1L);
  }

  void request(const std::string &url) {
    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
    curl_multi_add_handle(container.multi_, curl_);
  }

  ~CurlTest() { curl_easy_cleanup(curl_); }

private:
  static size_t staticWrite(char *ptr, size_t size, size_t nmemb,
                            void *userdata) {
    CurlTest *self = static_cast<CurlTest *>(userdata);
    return self->write(ptr, size, nmemb);
  }

  size_t write(char *ptr, size_t size, size_t nmemb) {
    auto total = size * nmemb;
    data_.insert(data_.end(), ptr, ptr + total);
    std::cout << data_;
    return total;
  }

  CURL *curl_;
  std::string data_;
};

static std::vector<CurlTest> tests(3);

extern "C" {
EMSCRIPTEN_KEEPALIVE void prepare() {
  std::string escape_test = "https://example.com/?a=1&b=2";
  auto escaped = curl_escape(escape_test.c_str(), escape_test.length());
  std::cout << escaped << std::endl;
  free(escaped);

  curl_global_init(CURL_GLOBAL_ALL);
}

EMSCRIPTEN_KEEPALIVE void request(int index, const char *url) {
  tests[index].request(url);
}
}
