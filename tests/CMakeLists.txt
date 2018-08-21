
include_directories("${PROJECT_SOURCE_DIR}/src")
source_group(Headers FILES ${PROJECT_SOURCE_DIR}/src/bits.hpp )
add_executable (run_tests main.cpp ${PROJECT_SOURCE_DIR}/src/bits.hpp )

if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(CMAKE_CXX_FLAGS -std=c++11)
elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang)
    set(CMAKE_CXX_FLAGS -std=c++11)
elseif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    set(CMAKE_CXX_FLAGS /std:c++14)
endif()
