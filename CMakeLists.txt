cmake_minimum_required(VERSION 3.25)
project(Sqlite-cpp VERSION 1.0.1 DESCRIPTION "Envoltorio para manejar Sqlite desde C++")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Encuentra SQLite3
find_package(SQLite3 REQUIRED)

# Define la biblioteca compartida
add_library(Sqlite-cpp SHARED
    src/sqlite.cpp
)

# Asegúrate de incluir el directorio de headers
target_include_directories(Sqlite-cpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Enlaza SQLite3 a la biblioteca
target_link_libraries(Sqlite-cpp PUBLIC SQLite::SQLite3)
