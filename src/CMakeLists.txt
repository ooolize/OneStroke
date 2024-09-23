file(GLOB_RECURSE SRC_FILES src/*.cpp)

set(SOURCES ${SRC_FILES})

add_executable(ZhouBoTong ${SOURCES})

target_include_directories(ZhouBoTong PUBLIC include)
target_link_libraries(ZhouBoTong PUBLIC pthread)

set_directory_properties(ZhouBoTong PROERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
