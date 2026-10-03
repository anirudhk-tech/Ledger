cmake_minimum_required(VERSION 3.20)
project(ledger CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(ledger_core src/pager.cpp)
target_include_directories(ledger_core PUBLIC include)

add_executable(ledger apps/main.cpp)
target_link_libraries(ledger PRIVATE ledger_core)
