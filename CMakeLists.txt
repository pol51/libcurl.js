cmake_minimum_required(VERSION 3.16)

project(libcurl.js VERSION 8.14.1)

include(GNUInstallDirs)

option(BUILD_TESTING "Build tests with ASAN" OFF)

if (BUILD_TESTING)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address -gsource-map)
endif()

add_subdirectory(lib)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()

install(DIRECTORY include/curl
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

configure_file(libcurl.pc.in ${PROJECT_BINARY_DIR}/libcurl.pc @ONLY)

install(FILES ${PROJECT_BINARY_DIR}/libcurl.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
