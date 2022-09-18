cmake_minimum_required(VERSION 3.22)
project(LRU C)

set(CMAKE_C_STANDARD 99)

set(MODULE_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake")

list(APPEND CMAKE_MODULE_PATH "${MODULE_DIR}")

add_executable(LRU src/main.c include/main.h src/obj_RAM.c include/obj_RAM.h src/sys_Util.c include/sys_Util.h)

find_package(UNISTRING REQUIRED)

target_include_directories(LRU PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(LRU PUBLIC ${UNISTRING_INCLUDE_DIR})

target_link_libraries(LRU PUBLIC ${UNISTRING_LIBRARY})
