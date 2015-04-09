# Configure a header file to pass some of the CMake settings
# to the source code
configure_file(
  "${PROJECT_SOURCE_DIR}/src/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )
# Add the binary tree to the search path for include files
# so that we will find config.h
include_directories("${PROJECT_BINARY_DIR}")

add_executable(tinybatt tinybatt.c)
target_link_libraries(tinybatt m)

install(
  TARGETS tinybatt
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )

# If you have something else than GCC, consult compiler documentation on
# flags required to enable c99 standard.
set(CMAKE_C_FLAGS "-std=c99")
# The following line should in the future replace explicit flag declaration:
#set(CMAKE_C_STANDARD_REQUIRED c99)  # this will work only with very recent cmake
