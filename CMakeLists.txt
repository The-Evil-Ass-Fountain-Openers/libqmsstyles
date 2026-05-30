cmake_minimum_required(VERSION 3.16)

project(libqmsstyles VERSION 2.0.0 LANGUAGES CXX)

set(QT_MIN_VERSION "6.10.0")
set(QT_MAJOR_VERSION 6)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

option(BUILD_TOOL OFF)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(FeatureSummary)

find_package(LibWres REQUIRED)
find_package(Qt${QT_MAJOR_VERSION} REQUIRED COMPONENTS Core Quick Widgets)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

add_subdirectory(src)
