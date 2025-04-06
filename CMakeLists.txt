project(TinyBatt C)
cmake_minimum_required(VERSION 2.6)

set(TinyBatt_MAJOR_VERSION 0)
set(TinyBatt_MINOR_VERSION 0)
set(TinyBatt_PATCH_VERSION 4)
set(TinyBatt_VERSION
  ${TinyBatt_MAJOR_VERSION}.${TinyBatt_MINOR_VERSION}.${TinyBatt_PATCH_VERSION})

add_subdirectory(src)


# Package settings
set(CPACK_GENERATOR DEB;RPM;TGZ)  # default package generators
set(CPACK_INSTALL_CMAKE_PROJECTS "${PROJECT_BINARY_DIR};TinyBatt;ALL;/")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 1)
set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/LICENSE)
set(CPACK_PACKAGE_FILE_NAME tinybatt-${TinyBatt_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "TinyBatt - brief one-line battery information")
set(CPACK_PACKAGE_DESCRIPTION_FILE ${PROJECT_SOURCE_DIR}/DESCRIPTION)
set(CPACK_PACKAGE_CONTACT "Serhii Frolov <dunkan.aidaho@gmail.com>")
# DEB settings
set(CPACK_DEBIAN_PACKAGE_SECTION Utilities)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.13)")
# RPM settings
set(CPACK_RPM_PACKAGE_LICENSE GPL)
set(CPACK_RPM_PACKAGE_REQUIRES "libc6 >= 2.13")
include(CPack)
