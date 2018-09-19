cmake_minimum_required (VERSION 2.6)
project (Bits)


OPTION(USE_CXX_11 "Build with C++11 or newer on compilers that require command-line flags to enable support for it." Off)

if(USE_CXX_11)
    if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
	    set(CMAKE_CXX_FLAGS -std=c++11)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang)
	    set(CMAKE_CXX_FLAGS -std=c++11)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
	    set(CMAKE_CXX_FLAGS /std:c++14)
    endif()
endif()

add_subdirectory (tests)