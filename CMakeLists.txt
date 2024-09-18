# (C) 2020 The University of Chicago
# See COPYRIGHT in top-level directory.
cmake_minimum_required (VERSION 3.8)
project (bedrock-module-api C CXX)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
enable_testing ()


option (ENABLE_EXAMPLES "Build examples" OFF)

# library version set here (e.g. for shared libs).
set (BEDROCK_MODULE_API_VERSION_MAJOR 0)
set (BEDROCK_MODULE_API_VERSION_MINOR 2)
set (BEDROCK_MODULE_API_VERSION_PATCH 0)
set (BEDROCK_MODULE_API_VERSION
    "${BEDROCK_MODULE_API_VERSION_MAJOR}.${BEDROCK_MODULE_API_VERSION_MINOR}.${BEDROCK_MODULE_API_VERSION_PATCH}")
add_definitions ("-DBEDROCK_MODULE_API_VERSION=${BEDROCK_MODULE_API_VERSION}")

# link shared lib with full rpath
set (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# setup cache variables for ccmake
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE RelWithDebInfo
         CACHE STRING "Choose the type of build." FORCE)
    set_property (CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
                  "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif ()
set (CMAKE_PREFIX_PATH "" CACHE STRING "External dependencies path")
set (BUILD_SHARED_LIBS "ON" CACHE BOOL "Build a shared library")

# search fo thallium
find_package (thallium REQUIRED)
# search for spdlog
find_package (spdlog REQUIRED)
# search for fmt
find_package (fmt REQUIRED)
# search for nlohmann_json
find_package (nlohmann_json REQUIRED)

add_subdirectory (src)

if (ENABLE_EXAMPLES)
    add_subdirectory (examples)
endif ()
