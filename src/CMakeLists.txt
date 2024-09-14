cmake_minimum_required(VERSION 3.5)
project	(ModbusProt)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)

option(MODBUS_PROT_OS "Select operation system (Default: LINUX)" LINUX)

include(ModbusProt/CMakeLists.txt)
include(ModbusTCP/CMakeLists.txt)
include(ModbusRTU/CMakeLists.txt)

#if (GDS_DB STREQUAL "LINUX")
#    include(Linux/CMakeLists.txt)
#else ()
#    include(Windows/CMakeLists.txt)
#endif()


include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/../../../devel/asio-1.30.2/include
    
)
add_executable (proxy1 proxy1.cpp)
add_executable (proxy2 proxy2.cpp)