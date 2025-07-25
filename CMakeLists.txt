cmake_minimum_required(VERSION 3.15)
project(lxPS VERSION 0.1.0)

cmake_policy(SET CMP0167 NEW)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Boost REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(UDEV REQUIRED libudev)

find_package(Notcurses REQUIRED)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.c)

add_executable(lxPS ${SOURCES})
target_include_directories(lxPS PUBLIC ${PROJECT_SOURCE_DIR}/headers)
target_compile_options(lxPS PRIVATE -Wall -Wextra -Wshadow -Werror -g -O0)

target_link_libraries(lxPS PRIVATE ${Boost_LIBRARIES} ${UDEV_LIBRARIES})
