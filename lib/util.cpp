#include "util.h"
#include "urldata.h"
#include <unordered_set>

extern "C" {

void *malloc_handles() {
  void *handles = malloc(sizeof(std::unordered_set<Curl_easy *>));
  new (handles) std::unordered_set<Curl_easy *>();
  return handles;
}

void free_handles(void *handles) {
  if (!handles) {
    return;
  }
  auto *set = (std::unordered_set<Curl_easy *> *)handles;
  set->~unordered_set();
  free(handles);
}

void add_handle(void *handles, Curl_easy *handle) {
  auto *set = (std::unordered_set<Curl_easy *> *)handles;
  set->insert(handle);
}

Curl_easy *pop_handle(void *handles) {
  auto *set = (std::unordered_set<Curl_easy *> *)handles;
  if (set->empty()) {
    return nullptr;
  }
  auto it = set->begin();
  Curl_easy *handle = *it;
  set->erase(it);
  return handle;
}

void foreach_handle(void *handles, void (*func)(Curl_easy *handle)) {
  auto *set = (std::unordered_set<Curl_easy *> *)handles;
  for (auto handle : *set) {
    func(handle);
  }
}
}
