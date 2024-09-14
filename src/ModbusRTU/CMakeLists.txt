include_directories(
    ${CMAKE_SOURCE_DIR}
)


file(
    GLOB ModbusRTU_SRC
    ./ModbusRTU/*.cpp
)

add_library(
    ModbusRTU
    SHARED
    ${ModbusRTU_SRC}
)
