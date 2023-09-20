cmake_minimum_required(VERSION 3.13)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PUZZLE_BUILD_SUPPORT_DIR "${CMAKE_SOURCE_DIR}/build_support")
set(PUZZLE_THIRD_PARTY_DIR   "${CMAKE_SOURCE_DIR}/third_party")

project(PUZZLE
        VERSION 2023.9
        DESCRIPTION ""
        LANGUAGES C CXX
        )

# #####################################################################################################################
# COMPILER SETUP
# #####################################################################################################################

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Set build type: Debug.")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Compiler flags.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wl,-export-dynamic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-attributes -Wno-unused-command-line-argument")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -rdynamic")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL Debug))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -ggdb -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls")
elseif (CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL Release))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g")
endif()

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")

# Output directory.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# #####################################################################################################################
# MACROS
# #####################################################################################################################

# use to change the __FILE__ output
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_FILE_PREFIX_SIZE)
add_definitions("-DSOURCE_FILE_PREFIX_SIZE=${SOURCE_FILE_PREFIX_SIZE}")

# #####################################################################################################################
# DEPENDENCIES
# #####################################################################################################################

# Files.
set(PUZZLE_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(PUZZLE_TEST_DIR ${PROJECT_SOURCE_DIR}/test)

# Includes.
set(PUZZLE_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/include)
set(PUZZLE_TEST_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/test/include)
set(PUZZLE_THIRD_PARTY_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}/third_party/googletest/googletest/include
        )

include_directories(${PUZZLE_SRC_INCLUDE_DIR} ${PUZZLE_TEST_INCLUDE_DIR} ${PUZZLE_THIRD_PARTY_INCLUDE_DIR})
include_directories(BEFORE src) # This is needed for gtest.

function(disable_target_warnings NAME)
    target_compile_options(${NAME} PRIVATE "-w")
endfunction()

# CTest
enable_testing()

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(third_party)