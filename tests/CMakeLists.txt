
include_directories("${PROJECT_SOURCE_DIR}/src")
source_group(Headers FILES ${PROJECT_SOURCE_DIR}/src/bits.hpp )
add_executable (run_tests main.cpp ${PROJECT_SOURCE_DIR}/src/bits.hpp)
