# libcurl.js

Minimal curl wasm implementation to work with [fcitx5-chinese-addons](https://github.com/fcitx/fcitx5-chinese-addons)' [cloudpinyin](https://github.com/fcitx/fcitx5-chinese-addons/tree/master/modules/cloudpinyin).

Network ability is implemented with `emscripten_fetch`.

All socket-related codes are stub, and there are semantic changes in `CURLMOPT_TIMERFUNCTION`, `curl_multi_add_handle` and more.
