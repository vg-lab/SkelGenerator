source_group( \\ FILES CMakeLists.txt )


set( SKELGENERATOR_SOURCES
        main.cpp
        MainWindow.cpp
        messagebox.cpp
        )

set( SKELGENERATOR_HEADERS
        MainWindow.ui
        MainWindow.h
        messagebox.h
        )

set( SKELGENERATOR_LINK_LIBRARIES
        Qt5::Core
        Qt5::Widgets
        skelgeneratorutil
        )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${PROJECT_BINARY_DIR}/include ${PROJECT_BINARY_DIR}/src )
include_directories( ${EIGEN_INCLUDE_DIR})

common_application( SkelGenerator )
