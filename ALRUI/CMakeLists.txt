cmake_minimum_required(VERSION 3.28)
project(ALR LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use ccache if available
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

# Default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

# Detect platform
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND (CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64"))
    add_compile_definitions(BUILD_ON_PI)
else()
    add_compile_definitions(BUILD_ON_PC)
endif()

# Prevent in-source builds
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR 
        "In-source builds are not allowed!\n"
        "Please create a separate build directory and run cmake from there:\n"
        "  mkdir build\n"
        "  cd build\n"
        "  cmake ..\n"
        "Current source dir: ${CMAKE_SOURCE_DIR}\n"
        "Current binary dir: ${CMAKE_BINARY_DIR}")
endif()

# Add build dir macro
add_compile_definitions(CMAKE_BUILD_DIR="${CMAKE_CURRENT_BINARY_DIR}")

# ===== Qt5 Setup =====
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Add Qt Network module (required for QTcpSocket / networking)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui Network)

# Make Qt5 available in global var to share in subprojects
set(GLOBAL_LIBS Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)

# ===== Enable Tests =====
option(ENABLE_TESTS "Build unit tests" ON)
if (ENABLE_TESTS)
  enable_testing()
endif()

# ===== Subdirectories =====
add_subdirectory(libs)
add_subdirectory(apps)
