cmake_minimum_required(VERSION 2.6)
project(96boardsctl)

set (96boardsctl_VERSION_MAJOR 1)
set (96boardsctl_VERSION_MINOR 0)

configure_file ("${PROJECT_SOURCE_DIR}/config.h.in" "${PROJECT_BINARY_DIR}/config.h")
include_directories ("${PROJECT_BINARY_DIR}")

find_package(PkgConfig)
pkg_check_modules(PC_LIBS REQUIRED libusb libftdi)

add_executable(96boardsctl 96boardsctl.c)
target_link_libraries(96boardsctl usb ftdi)

install (TARGETS 96boardsctl DESTINATION bin)
install (FILES udev-rules/71-ftdi_sio.rules DESTINATION etc/udev/rules.d)
