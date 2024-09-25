cmake_minimum_required(VERSION 3.28)

project(static-reflection-demo LANGUAGES CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

find_program(CLANG_EXEC clang++)
if (CLANG_EXEC)
  message(STATUS "Found clang++: ${CLANG_EXEC}")
  set(CMAKE_CXX_COMPILER ${CLANG_EXEC})
else ()
  message(STATUS "Cannot find clang++, use default: ${CMAKE_CXX_COMPILER}")
endif ()

add_subdirectory(src)
add_subdirectory(test)
