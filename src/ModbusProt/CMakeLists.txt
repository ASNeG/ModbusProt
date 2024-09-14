include_directories(
    ${CMAKE_SOURCE_DIR}
)


file(
    GLOB ModbusProt_SRC
    ./ModbusProt/*.cpp
)

add_library(
    ModbusProt
    SHARED
    ${ModbusProt_SRC}
)
