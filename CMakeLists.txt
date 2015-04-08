project(TinyBatt C)
cmake_minimum_required(VERSION 2.6)

set(TinyBatt_MAJOR_VERSION 0)
set(TinyBatt_MINOR_VERSION 0)
set(TinyBatt_PATCH_VERSION 1)
set(TinyBatt_VERSION
  ${TinyBatt_MAJOR_VERSION}.${TinyBatt_MINOR_VERSION}.${TinyBatt_PATCH_VERSION})

add_subdirectory(src)