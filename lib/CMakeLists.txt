add_library(curl
    easy.c
    escape.c
    multi.c
    setopt.c
    slist.c
    util.cpp
)

target_include_directories(curl PRIVATE "${PROJECT_SOURCE_DIR}/include/curl")

install(TARGETS curl
    DESTINATION ${CMAKE_INSTALL_LIBDIR})
