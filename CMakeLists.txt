cmake_minimum_required( VERSION 3.1 )
set( PROJECT_VERSION_MAJOR "0" )
set( PROJECT_VERSION_MINOR "0" )
set( PROJECT_VERSION_PATCH "1" )
set( PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH} )
project( SkelGenerator VERSION ${PROJECT_VERSION} )
set( skelgenerator_VERSION_ABI 1 )

# Disable in source building
if( "${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}" )
    message( FATAL "no in source building allowed." )
endif( )

if( NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/VCG/vcg/complex/all_types.h )
    message( FATAL_ERROR "VCG missing, run: git submodule update --init")
endif( )

set (VCG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/VCG")

list( APPEND CMAKE_MODULE_PATH
        ${PROJECT_SOURCE_DIR}/CMake
        ${CMAKE_SOURCE_DIR}/CMake/common
        )

# Default build type is Debug
if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Debug )
endif( NOT CMAKE_BUILD_TYPE )

if ( CMAKE_BUILD_TYPE MATCHES "Debug" )
    add_definitions( -DDEBUG )
endif( )

include( GitExternal )

set( SKELGENERATOR_DESCRIPTION "SkelGenerator" )
set( COMMON_PROJECT_DOMAIN gmrv.es )

include( Common )

common_find_package(VCG SYSTEM REQUIRED)
common_find_package( Qt5Core SYSTEM REQUIRED )
common_find_package( Qt5Widgets SYSTEM REQUIRED )
common_find_package(Eigen3 SYSTEM REQUIRED)
common_find_package(Boost COMPONENTS filesystem REQUIRED)

common_find_package_post( )

list( APPEND SKELGENERATOR_DEPENDENT_LIBRARIES
        Qt5::Core
        Qt5::Widgets
        Eigen3
        VCG
        meshreconstructwrapper
        ${Boost_LIBRARIES}
        )
add_subdirectory( SkelGenerator )
add_subdirectory(SkelGeneratorUtil)

if( MSVC )
    # Set SkelGenerator as startup project on Visual Studio, CMake v3.6
    if( CMAKE_VERSION VERSION_GREATER 3.6 )
        set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT SkelGenerator )
    endif( )
endif( )

set( DOXYGEN_MAINPAGE_MD README.md )
set( DOXYGEN_EXTRA_INPUT ${PROJECT_SOURCE_DIR}/README.md )
include( DoxygenRule )

include( CPackConfig )
