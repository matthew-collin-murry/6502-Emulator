
cmake_minimum_required(VERSION 3.28)

set(CMAKE_BUILD_TYPE Debug)

project(emulator-6502)

set(CMAKE_CXX_STANDARD 20)

include_directories(./src/)

# Retrieve the googletest test framework
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_compile_options(-pedantic -Wall -Wextra)

# TODO add back emulator executable when needed

# add_executable(
#   emulator
#   ./src/main.cpp
#   ./src/m6502.cpp
#   ./src/m6502.h
# )

add_executable(
  tests
  ./src/tests/load_register_tests__extra.cpp
  ./src/tests/store_register_tests.cpp
  ./src/tests/jump_return_tests.cpp
  ./src/tests/stack_operations_tests.cpp
  ./src/tests/logical_tests.cpp
  ./src/tests/logical_tests.h
  ./src/m6502.cpp
  ./src/m6502.h
)

# target_compile_options(tests PUBLIC -Og)

# target_precompile_headers(
#   emulator
#   PUBLIC
#     src/project_header.h
# )

target_precompile_headers(
  tests
  PUBLIC
    src/project_header.h
)

target_link_libraries(
  tests GTest::gtest_main
)