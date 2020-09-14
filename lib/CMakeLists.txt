# 
# SPDX-License-Identifier: MIT
# 
# The MIT License (MIT)
# 
# Copyright (c) <2020> Matthias Loy, Stephan Gatzka
# 
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
# 
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
# BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

cmake_minimum_required(VERSION 3.9)
project(scramjet_peer VERSION 0.0.1 LANGUAGES CXX)

find_package(Boost 1.71.0 REQUIRED system QUIET)

add_library(${PROJECT_NAME}
    scramjet/error_code.hpp
    scramjet/jet_connection.cpp
    scramjet/jet_connection.hpp
    scramjet/jet_peer.cpp
    scramjet/jet_peer.hpp
    scramjet/protocol_version.cpp
    scramjet/protocol_version.hpp
    scramjet/socket_jet_connection.cpp
    scramjet/socket_jet_connection.hpp
)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_11)

