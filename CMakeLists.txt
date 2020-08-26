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

cmake_minimum_required(VERSION 3.14)

include(FetchContent)

FetchContent_Declare(GatzkaToolchainFiles
    GIT_REPOSITORY https://github.com/gatzka/cmake-toolchain-files.git
    SOURCE_DIR ${CMAKE_BINARY_DIR}/toolchains
)
FetchContent_MakeAvailable(GatzkaToolchainFiles)

project(scramjet CXX)

find_package(Threads)
find_package(Boost 1.71.0 REQUIRED system thread)

add_executable(scramjet_peer
    protocol_version.cpp
    scramjet_peer.cpp
)

target_link_libraries(scramjet_peer
    ${CMAKE_THREAD_LIBS_INIT}
)

