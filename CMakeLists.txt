cmake_minimum_required(VERSION 3.5)
project (ModbusProt)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)


option(PROJ_UNIT_TEST "Enable/Disable unittest (Default: OFF)" ON)


set(CPUNIT_ROOT_DIR "${CMAKE_SOURCE_DIR}/../cpunit" CACHE STRING "cpunit root directory")
set(ASIO_ROOT_DIR "${CMAKE_SOURCE_DIR}/../asio-1.30.2" CACHE STRING "asio root directory")

add_compile_options(-DASIO_ENABLE_HANDLER_TRACKING)

include(src/CMakeLists.txt)

if (${PROJ_UNIT_TEST} STREQUAL "ON")
    include(tst/CMakeLists.txt)
endif ()


