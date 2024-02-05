# Copyright 2021 Ingemar Hedvall
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.20)
project(grpcservers
        VERSION 1.0
        DESCRIPTION "Various gRPC servers"
        LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Static libraries are preferred." OFF)
option(SERV_DOC "Build documentation." OFF)
option(SERV_TOOLS "Create stand-alone RPC servers" OFF)
option(SERV_TEST "Run unit tests." OFF)

if (NOT COMP_DIR)
    set(COMP_DIR "k:")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_DEBUG_POSTFIX d)

include ("script/utillib.cmake")
#include ("script/mdflib.cmake")
include ("script/workflowlib.cmake")
include ("script/odslib.cmake")
include("script/boost.cmake")
include("script/zlib.cmake")
include("script/sqlite3.cmake")
include("script/postgresql.cmake")
include("script/protobuf.cmake")
include("script/grpc.cmake")

if (SERV_TEST)
    include("script/googletest.cmake")
endif()

if (SERV_DOC)
    include("script/doxygen.cmake")
endif()


set(PROTO_FILES proto/syslogservice.proto )

add_library(serv
        src/syslogrpcserver.cpp src/syslogrpcserver.h
        src/syslogservice.cpp src/syslogservice.h
        ${PROTO_FILES}
        src/syslogrpcclient.cpp src/syslogrpcclient.h)

protobuf_generate(
        TARGET serv
        OUT_VAR PROTO_GENERATED_FILES
        LANGUAGE cpp
        #IMPORT_DIRS ${CMAKE_CURRENT_DIR}/proto
        #PROTOS proto/syslogservice.proto
        PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

cmake_print_variables(PROTO_GENERATED_FILES)
#protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
#        proto/syslogservice.proto)

protobuf_generate(
        TARGET serv
        OUT_VAR GRPC_GENERATED_FILES
        LANGUAGE grpc
        GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
        PLUGIN "protoc-gen-grpc=\$<TARGET_FILE:gRPC::grpc_cpp_plugin>"
        #PLUGIN_OPTIONS "generate_mock_code=true"
        #IMPORT_DIRS ${CMAKE_CURRENT_DIR}/proto
        #PROTOS proto/syslogservice.proto
        PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR} )

cmake_print_variables(GRPC_GENERATED_FILES)
cmake_print_variables(utillib_SOURCE_DIR)

target_include_directories(serv PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(serv PRIVATE ${SQLite3_INCLUDE_DIRS})
target_include_directories(serv PRIVATE ${PostgreSQL_INCLUDE_DIRS})
target_include_directories(serv PRIVATE ${utillib_SOURCE_DIR}/include)
#target_include_directories(serv PRIVATE ${mdflib_SOURCE_DIR}/include)
target_include_directories(serv PRIVATE ${workflowlib_SOURCE_DIR}/include)
target_include_directories(serv PRIVATE ${odslib_SOURCE_DIR}/include)
target_include_directories(serv PRIVATE ${odslib_SOURCE_DIR}/src)
target_include_directories(serv PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(serv PRIVATE ${Protobuf_INCLUDE_DIRS})
target_include_directories(serv PRIVATE ${gRPC_INCLUDE_DIRS} )
target_include_directories(serv PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(serv PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )

target_compile_definitions(serv PRIVATE XML_STATIC)

set(ODS_PUBLIC_HEADERS include/ods/itable.h)

set_target_properties(ods PROPERTIES PUBLIC_HEADER "${ODS_PUBLIC_HEADERS}")
get_target_property(PH3 ods PUBLIC_HEADER)
message( STATUS "ODS Target Includes: " "${PH3}")

install(TARGETS serv
        EXPORT OdsLibTargets
        LIBRARY DESTINATION serv/lib
        ARCHIVE DESTINATION serv/lib
        RUNTIME DESTINATION serv/bin
        PUBLIC_HEADER DESTINATION serv/include/ods)

if (SERV_TOOLS)
    add_subdirectory(eventlogrpcserverd)
endif()

if (SERV_TEST)
    enable_testing()
    add_subdirectory(test)
endif()

if (DOXYGEN_FOUND AND SERV_DOC AND (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease") )
    #set(DOXYGEN_RECURSIVE YES)
    set(DOXYGEN_RECURSIVE NO)
    set(DOXYGEN_REPEAT_BRIEF NO)
    set(DOXYGEN_PROJECT_NAME "gRPC Servers")
    # set(DOXYGEN_HTML_EXTRA_STYLESHEET doxygen/utillib.css)
    # set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs/manual)
    doxygen_add_docs(doc_libserv ALL
            include/serv
            COMMENT "ODS Library generating API documentation with Doxygen")
endif()