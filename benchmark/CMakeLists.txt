file(GLBO_RECURSE BENCHMARK_FILES benchmark/*.cpp)

set(BENCHMARK_SOURCES ${BENCHMARK_FILES})

find_package(benchmark REQUIRED)

add_executable(ZhouBoTongBenchmark ${BENCHMARK_SOURCES})

target_link_libraries(ZhouBoTongBenchmark PUBLIC benchmark::benchmark)

set_directory_properties(ZhouBoTongBenchmark PROERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)