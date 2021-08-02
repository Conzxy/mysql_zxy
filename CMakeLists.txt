cmake_minimum_required(VERSION 3.12)
project(zxymysql)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-g -lmysqlclient")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/util)
include_directories(${CMAKE_SOURCE_DIR}/third-party/TinySTL/include)

aux_source_directory(${CMAKE_SOURCE_DIR}/lib LIB_LIST)
link_libraries(/usr/lib/x86_64-linux-gnu/libmysqlclient.so)

add_subdirectory(test)
