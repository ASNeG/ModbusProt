
include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_SOURCE_DIR}
    ${ASIO_ROOT_DIR}/include 
)

file(
    GLOB ModbusProt_SRC
    ${CMAKE_CURRENT_LIST_DIR}/Base/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ModbusTCP/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ModbusRTU/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ModbusProt/*.cpp
)

add_library(
    ModbusProt
    OBJECT
    ${ModbusProt_SRC}
)

set_property(TARGET ModbusProt PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(
    ModbusProt_shared 
    SHARED 
    $<TARGET_OBJECTS:ModbusProt>
)
add_library(
    ModbusProt_static 
    STATIC 
    $<TARGET_OBJECTS:ModbusProt>
)

if (${PROJ_UNIT_TEST} STREQUAL "ON")
    add_executable(
        analyse 
        ${CMAKE_CURRENT_LIST_DIR}/Analyse/main.cpp
    )
    
    add_executable(
        channel 
        ${CMAKE_CURRENT_LIST_DIR}/Analyse/Channel.cpp
    )
endif()

# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------
#
# installation
#
# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------
install(
    TARGETS ModbusProt_static 
    DESTINATION lib
)
install(
    TARGETS ModbusProt_shared 
    DESTINATION lib
)
install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/src/
    DESTINATION include/ModbusProt
    FILES_MATCHING PATTERN "*.h*" 
                   PATTERN "Analyse" EXCLUDE
)

