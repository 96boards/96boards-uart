cmake_minimum_required(VERSION 2.6)
project(96boardsctl)

set (96boardsctl_VERSION_MAJOR 1)
set (96boardsctl_VERSION_MINOR 0)

configure_file ("${PROJECT_SOURCE_DIR}/config.h.in" "${PROJECT_BINARY_DIR}/config.h")
include_directories ("${PROJECT_BINARY_DIR}")

find_package(PkgConfig)

pkg_check_modules(LIBFTDI libftdi1)
if (NOT LIBFTDI_FOUND)
    pkg_check_modules(LIBFTDI REQUIRED libftdi)
endif()

include_directories(${LIBFTDI_INCLUDE_DIRS})
link_directories(${LIBFTDI_LIBRARY_DIRS})
add_definitions(${LIBFTDI_CFLAGS_OTHER})

add_executable(96boardsctl 96boardsctl.c)
target_link_libraries(96boardsctl ${LIBFTDI_LIBRARIES})

install (TARGETS 96boardsctl DESTINATION bin)
install (FILES udev-rules/71-ftdi_sio.rules DESTINATION etc/udev/rules.d)
