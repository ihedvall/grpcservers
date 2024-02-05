# Copyright 2024 Ingemar Hedvall
# SPDX-License-Identifier: MIT

project(TestServers
        VERSION 1.0
        DESCRIPTION "Google unit tests for the google RPC servers"
        LANGUAGES CXX)

add_executable(test_serv
 		testsyslogrunner.cpp testsyslogrunner.h)

target_include_directories(test_serv PRIVATE ../include ../src)
target_include_directories(test_serv PRIVATE ${utillib_SOURCE_DIR}/include)
target_include_directories(test_serv PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(test_serv PRIVATE ${SQLite3_INCLUDE_DIRS})
target_include_directories(test_serv PRIVATE ${PostgreSQL_INCLUDE_DIRS})
target_include_directories(test_serv PRIVATE ${workflowlib_SOURCE_DIR}/include)
target_include_directories(test_serv PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/..)
target_include_directories(test_serv PRIVATE ${odslib_SOURCE_DIR}/src)
target_compile_definitions(test_serv PRIVATE XML_STATIC)

if (MINGW)
    target_link_options(test_serv PRIVATE -static -fstack-protector)
elseif (MSVC)
    target_compile_definitions(test_serv PRIVATE _WIN32_WINNT=0x0A00)
endif()

target_link_libraries(test_serv PRIVATE serv)
target_link_libraries(test_serv PRIVATE ods)
target_link_libraries(test_serv PRIVATE workflow)
target_link_libraries(test_serv PRIVATE mdf)
target_link_libraries(test_serv PRIVATE util)
target_link_libraries(test_serv PRIVATE ${SQLite3_LIBRARIES})
target_link_libraries(test_serv PRIVATE ${Boost_LIBRARIES})
target_link_libraries(test_serv PRIVATE ${EXPAT_LIBRARIES})
target_link_libraries(test_serv PRIVATE ${ZLIB_LIBRARIES})
target_link_libraries(test_serv PRIVATE GTest::gtest_main)
target_link_libraries(test_serv PRIVATE ${PostgreSQL_LIBRARY_DIRS}/libpq.lib)
target_link_libraries(test_serv PRIVATE ${PostgreSQL_LIBRARY_DIRS}/libpgtypes.lib)
target_link_libraries(test_serv PRIVATE ${PostgreSQL_LIBRARY_DIRS}/libpgport.lib)
target_link_libraries(test_serv PRIVATE ${PostgreSQL_LIBRARY_DIRS}/libpgcommon.lib)

target_link_libraries(test_serv PRIVATE absl::log)
target_link_libraries(test_serv PRIVATE absl::log_internal_check_op)
target_link_libraries(test_serv PRIVATE utf8_range::utf8_range)
target_link_libraries(test_serv PRIVATE utf8_range::utf8_validity)
target_link_libraries(test_serv PRIVATE gRPC::grpc++)

if (WIN32)
target_link_libraries(test_serv PRIVATE ws2_32)
target_link_libraries(test_serv PRIVATE mswsock)
target_link_libraries(test_serv PRIVATE secur32)
target_link_libraries(test_serv PRIVATE crypt32)
endif()

foreach (pqfile ${PostgreSQL_DEPENDENCIES})
    add_custom_command(
            TARGET test_serv
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ARGS ${pqfile} "${CMAKE_CURRENT_BINARY_DIR}"
    )
endforeach ()

include(GoogleTest)
gtest_discover_tests(test_serv)

