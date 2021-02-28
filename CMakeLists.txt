cmake_minimum_required(VERSION 3.4)
project(main)

set(CMAKE_CXX_FLAGS "-g -std=c++2a -Wall")


set(cclox_lib_SRC
  ${PROJECT_SOURCE_DIR}/Lox.cpp
  ${PROJECT_SOURCE_DIR}/Token.cpp
  ${PROJECT_SOURCE_DIR}/Scanner.cpp
  ${PROJECT_SOURCE_DIR}/Parser.cpp
  ${PROJECT_SOURCE_DIR}/Interpreter.cpp
  ${PROJECT_SOURCE_DIR}/Environment.cpp
  ${PROJECT_SOURCE_DIR}/LoxFunction.cpp
  ${PROJECT_SOURCE_DIR}/Resolver.cpp
  ${PROJECT_SOURCE_DIR}/LoxClass.cpp
)

add_library(cclox_lib ${cclox_lib_SRC})

add_executable(cclox ${PROJECT_SOURCE_DIR}/main.cpp)
target_link_libraries(cclox cclox_lib)

# tests
option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(tests)
endif()
