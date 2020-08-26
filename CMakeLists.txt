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

