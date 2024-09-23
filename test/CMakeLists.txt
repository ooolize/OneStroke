file(GLOB_RECURSE TEST_FILES test/*.cpp)

set(TEST_SOURCES ${TEST_FILES})

find_package(fmt GTest REQUIRED)

add_executable(ZhouBoTongTest ${TEST_SOURCES})

target_link_libraries(ZhouBoTongTest PUBLIC GTest::GTest fmt::fmt)
set_directory_properties(ZhouBoTongTest PROERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)