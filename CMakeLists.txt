cmake_minimum_required(VERSION 3.5)
set (CMAKE_CXX_STANDARD 14)
option(BUILD_SHARED_LIBS "Build shared libraries." OFF)
project (DNSClient CXX)
add_subdirectory (lib)

