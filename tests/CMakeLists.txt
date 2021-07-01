cmake_minimum_required(VERSION 3.8)

project(tests CXX)
enable_language(C ASM)

set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_CURRENT_LIST_DIR}/../CMakeLists.txt)

add_subdirectory(log)
add_subdirectory(bindings)