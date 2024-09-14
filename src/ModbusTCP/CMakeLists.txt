include_directories(
    ${CMAKE_SOURCE_DIR}
)


file(
    GLOB ModbusTCP_SRC
    ./ModbusTCP/*.cpp
)

add_library(
    ModbusTCP
    SHARED
    ${ModbusTCP_SRC}
)
