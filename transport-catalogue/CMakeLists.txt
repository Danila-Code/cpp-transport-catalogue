cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(transport-catalogue)

set(CMAKE_CXX_STANDARD 20)
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(
        CMAKE_CXX_FLAGS_DEBUG
        "${CMAKE_CXX_FLAGS_DEBUG} /JMC"
    )
else()
    set(
        CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-unused-parameter -Wno-implicit-fallthrough"
    )
endif()

file(GLOB sources
    *.cpp
    *.h
)

add_executable(
    transport-catalogue
    ${sources}
)