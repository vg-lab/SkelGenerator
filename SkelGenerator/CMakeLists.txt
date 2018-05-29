source_group( \\ FILES CMakeLists.txt )

set( Name SkelGenerator )
set( NAME SKELGENERATOR )
set( namespace skelgenerator )
set( NAMESPACE SKELGENERATOR )
set( INCLUDE_NAME skelGenerator )
set( PROJECT_VERSION_ABI ${${PROJECT_NAME}_VERSION_ABI} )

configure_file( ${CMAKE_SOURCE_DIR}/CMake/common/cpp/api.h
        ${PROJECT_BINARY_DIR}/include/skelGenerator/api.h @ONLY )
configure_file( ${CMAKE_SOURCE_DIR}/CMake/common/cpp/version.h
        ${PROJECT_BINARY_DIR}/include/skelGenerator/version.h @ONLY )
configure_file( ${CMAKE_SOURCE_DIR}/CMake/common/cpp/version.cpp
        ${PROJECT_BINARY_DIR}/src/version.cpp @ONLY )

set( SKELGENERATOR_SOURCES
        ${PROJECT_BINARY_DIR}/src/version.cpp

        main.cpp
        MainWindow.cpp
        messagebox.cpp
        NeuronRepresentation/Neuron.cpp
        NeuronRepresentation/SamplePoint.cpp
        NeuronRepresentation/Dendrite.cpp
        NeuronRepresentation/Section.cpp
        NeuronRepresentation/SubDendrite.cpp
        NeuronRepresentation/Spine.cpp
        VRMLReader.cpp

        )

set( SKELGENERATOR_HEADERS
        ${PROJECT_BINARY_DIR}/include/skelGenerator/version.h
        MainWindow.ui
        MainWindow.h
        messagebox.h
        Types.h
        NeuronRepresentation/Neuron.h
        NeuronRepresentation/SamplePoint.h
        NeuronRepresentation/Dendrite.h
        NeuronRepresentation/Section.h
        NeuronRepresentation/SubDendrite.h
        NeuronRepresentation/Spine.h
        VRMLReader.h
        )

set( SKELGENERATOR_LINK_LIBRARIES
        Qt5::Core
        Qt5::Widgets
        ${Boost_LIBRARIES}
        )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${PROJECT_BINARY_DIR}/include ${PROJECT_BINARY_DIR}/src )

add_definitions("-w")

if( MSVC )
    add_definitions( "-W4" )
    add_definitions( -DSKELGENERATOR_STATIC )
endif( )

common_application( SkelGenerator )