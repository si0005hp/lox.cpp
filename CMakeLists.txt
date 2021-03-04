cmake_minimum_required(VERSION 3.4)
project(main)

set(CMAKE_CXX_FLAGS "-g -std=c++2a -Wall")

set(LOX_SRX_DIR "${PROJECT_SOURCE_DIR}/src")

set(lox_lib_SRC
  ${LOX_SRX_DIR}/Lox.cpp
  ${LOX_SRX_DIR}/Token.cpp
  ${LOX_SRX_DIR}/Scanner.cpp
  ${LOX_SRX_DIR}/Parser.cpp
  ${LOX_SRX_DIR}/Interpreter.cpp
  ${LOX_SRX_DIR}/Environment.cpp
  ${LOX_SRX_DIR}/LoxFunction.cpp
  ${LOX_SRX_DIR}/Resolver.cpp
  ${LOX_SRX_DIR}/LoxClass.cpp
)

add_library(lox_lib ${lox_lib_SRC})

add_executable(lox ${LOX_SRX_DIR}/main.cpp)
target_link_libraries(lox lox_lib)

# tests
option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(test)
endif()

