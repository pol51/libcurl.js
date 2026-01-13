add_executable(test test.cpp)

target_include_directories(test PRIVATE
    "${PROJECT_SOURCE_DIR}/include/curl"
    "${PROJECT_SOURCE_DIR}/lib"
)

target_link_options(test PRIVATE
    -sALLOW_MEMORY_GROWTH
    -sEXIT_RUNTIME
    -sFETCH
    -sEXPORTED_RUNTIME_METHODS=['ccall']
)

target_link_libraries(test PRIVATE curl)
