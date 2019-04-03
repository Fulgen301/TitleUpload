cmake_minimum_required(VERSION 3.0.0)

project(TitleUpload VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Network REQUIRED)

add_subdirectory(lib/cc4group EXCLUDE_FROM_ALL)
include_directories(lib/cc4group/src)

add_executable(titleupload src/main.cpp src/titleupload.cpp)
target_link_libraries(titleupload Qt5::Core Qt5::Gui Qt5::Network cppc4group)
