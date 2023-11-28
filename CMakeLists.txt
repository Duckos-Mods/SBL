cmake_minimum_required(VERSION 3.0.0)
project(SBL VERSION 0.1.0 LANGUAGES C CXX)
# set C++ version to 20
set(CMAKE_CXX_STANDARD 20)

# Collect headers and sources
file(GLOB_RECURSE SBL_HEADERS "src/*.h")
file(GLOB_RECURSE SBL_HEADERS "src/*.hpp")
file(GLOB_RECURSE SBL_SOURCES "src/*.cpp")

# add to executable
add_executable(SBL ${SBL_HEADERS} ${SBL_SOURCES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
