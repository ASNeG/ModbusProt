
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${ASIO_ROOT_DIR}/include
    ${CPUNIT_ROOT_DIR}/src
)

file(
    GLOB UNIT_TEST_SRC
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
)

add_executable(
    unit_test 
    ${UNIT_TEST_SRC}
)

target_link_libraries(
    unit_test 
    ${CPUNIT_ROOT_DIR}/lib/libCPUnit.a
    ${CMAKE_SOURCE_DIR}/build/libModbusProt_static.a
)

