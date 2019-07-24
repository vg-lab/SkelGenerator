source_group(\\ FILES CMakeLists.txt)

set(SKELGENERATORUTIL_SOURCES
        Dendrite.cpp
        Neuron.cpp
        SamplePoint.cpp
        Section.cpp
        Spine.cpp
        SubDendrite.cpp
        VRMLReader.cpp
        )

set(SKELGENERATORUTIL_HEADERS
        Dendrite.h
        Neuron.h
        SamplePoint.h
        Section.h
        Spine.h
        SubDendrite.h
        VRMLReader.h
        Types.h
        )

set(SKELGENERATORUTIL_LINK_LIBRARIES
        ${Boost_LIBRARIES}
        )

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PROJECT_BINARY_DIR}/include ${PROJECT_BINARY_DIR}/src)

if (MSVC)
    add_definitions(-D_USE_MATH_DEFINES)
endif ()

common_library(skelgeneratorutil)
