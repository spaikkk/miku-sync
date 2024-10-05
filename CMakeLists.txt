set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
configure_file(${CMAKE_CURRENT_LIST_DIR}/.clangd.in
                   ${CMAKE_CURRENT_LIST_DIR}/.clangd @ONLY)
cmake_minimum_required(VERSION 3.25)

project(palle LANGUAGES CXX)


add_subdirectory(include/fmt-master)
add_subdirectory(src)



