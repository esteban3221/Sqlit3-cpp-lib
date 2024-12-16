cmake_minimum_required(VERSION 3.25)
project(Sqlite-cpp VERSION 1.0.1 DESCRIPTION "Envoltorio para manejar Sqlite desde c++")

set(CMAKE_CXX_STANDARD 20)

find_package(SQLite3 REQUIRED)
find_library(SQLITE3_LIBRARY sqlite3 REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(Sqlite-cpp SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sqlite.cpp
)


