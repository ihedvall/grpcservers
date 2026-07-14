# Copyright 2021 Ingemar Hedvall
# SPDX-License-Identifier: MIT

include (CMakePrintHelpers)
if (NOT OPENSSL_FOUND)
    set(OPENSSL_USE_STATIC_LIBS TRUE)
    find_package(OpenSSL COMPONENTS SSL)
endif()

cmake_print_variables( OpenSSL_FOUND
        OpenSSL_VERSION
        OPENSSL_INCLUDE_DIR
        OPENSSL_LIBRARIES
        OPENSSL_CRYPTO_LIBRARIES
        OPENSSL_CRYPTO_LIBRARY
        OPENSSL_SSL_LIBRARIES
        OPENSSL_SSL_LIBRARY
        OPENSSL_LIBRARIES)