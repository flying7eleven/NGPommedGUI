cmake_minimum_required( VERSION 2.8.7 )
project( "Next Generation pommed GUI" )

# do not allow in-source builds
if( CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR )
	message( FATAL_ERROR "In-source builds are not allowed." )
endif()

#
include( FindPkgConfig )

# set some specific compiler flags we need
set( CMAKE_CXX_FLAGS "-Wall -std=c++11" CACHE STRING "Flags used by the compiler during all build types." FORCE )

#
set( NGPOMMEDGUI_SOURCE_FILES ${NGPOMMEDGUI_SOURCE_FILES} source/main.cxx )

#
add_executable( ngpommedgui ${NGPOMMEDGUI_SOURCE_FILES} ${NGPOMMEDGUI_HEADER_FILES} )
