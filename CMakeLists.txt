project(pbs)

cmake_minimum_required (VERSION 3.7.1)

set(CMAKE_CXX_STANDARD 17)  # only useful when CMake version >= 3.1
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_LINKER  "/usr/bin/ld")
set(CMAKE_VERBOSE_MAKEFILE OFF)  # Show all commands and parameters

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "RELEASE")
ENDIF()

include_directories("inc")
file(GLOB SOURCES "src/*.cpp")
add_executable(pbs ${SOURCES})

# Find Boost
find_package(Boost REQUIRED COMPONENTS program_options system filesystem)
include_directories( ${Boost_INCLUDE_DIRS} )
target_link_libraries(pbs ${Boost_LIBRARIES})
