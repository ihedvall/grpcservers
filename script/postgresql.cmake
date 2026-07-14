# Copyright 2023 Ingemar Hedvall
# SPDX-License-Identifier: MIT
include(CMakePrintHelpers)

if (NOT PostgreSQL_FOUND)
    if (COMP_DIR)
        set(PostgreSQL_ROOT ${COMP_DIR}/PostgreSQL/latest)
    endif()

    if (NOT PostgreSQL_ROOT)
        set(PostgreSQL_ROOT ${COMP_DIR}/PostgreSQL/latest)
    endif()

    find_package(PostgreSQL REQUIRED)
endif()


file(GLOB PostgreSQL_DEPENDENCIES
        "${PostgreSQL_LIBRARY_DIRS}/../bin/lib*.dll"
        "${PostgreSQL_LIBRARY_DIRS}/../bin/lib*.so" )

cmake_print_variables( PostgreSQL_FOUND
        PostgreSQL_VERSION
        PostgreSQL_INCLUDE_DIRS
        PostgreSQL_LIBRARY_DIRS
        PostgreSQL_LIBRARIES
        PostgreSQL_TYPE_INCLUDE_DIR
        PostgreSQL_DEPENDENCIES)
