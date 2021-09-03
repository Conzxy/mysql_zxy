cmake_minimum_required(VERSION 3.12)
project(zxymysql)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-g -pthread")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/util)
include_directories(${CMAKE_SOURCE_DIR}/include/third-party/TinySTL/include)

aux_source_directory(${CMAKE_SOURCE_DIR}/lib LIB_LIST)

link_libraries(
	#"/usr/lib/x86_64-linux-gnu/libssl.a"
	#"/usr/lib/x86_64-linux-gnu/libmysqlclient.a"
	"/usr/lib/x86_64-linux-gnu/libmysqlclient.so"
	#"${CMAKE_SOURCE_DIR}/lib/libmysqlzxy.a"
	)

#message(${CMAKE_CXX_FLAGS})
#add_library(mysqlzxy STATIC ${LIB_LIST})
add_library(mysqlzxy SHARED ${LIB_LIST})
#add_subdirectory(test)

#add_executable(trunc_test ${CMAKE_SOURCE_DIR}/test/trunc-test.cc)
#target_link_libraries(trunc_test ${CMAKE_SOURCE_DIR}/lib/libmysqlzxy.so /usr/lib/x86_64-linux-gnu/libmysqlclient.so)

add_executable(null_insert ${CMAKE_SOURCE_DIR}/test/null_insert.cc ${LIB_LIST})
#target_link_libraries(null_insert ${CMAKE_SOURCE_DIR}/lib/libmysqlzxy.so /usr/lib/x86_64-linux-gnu/libmysqlclient.so)
