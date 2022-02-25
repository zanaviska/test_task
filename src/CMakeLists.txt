cmake_minimum_required(VERSION 3.19)
 
set(files lock_guard.cpp sm_sender.cpp sm_receiver.cpp)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

add_executable(bussines_logic ${files} bussines_logic.cpp)
add_executable(tcp_client client.cpp)
add_executable(server ${files} server.cpp) 
