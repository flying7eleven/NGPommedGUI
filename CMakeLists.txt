cmake_minimum_required( VERSION 2.8.7 )
project( "Next Generation pommed GUI" )

# do not allow in-source builds
if( CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR )
	message( FATAL_ERROR "In-source builds are not allowed." )
endif()

# add some options for building the project
option( BUILD_TESTS "Build the test cases for this project. You can run them with 'make tests'." OFF )
set( NGPOMMEDGUI_PID_FILE "/var/run/ngpommed.pid" CACHE STRING "The default location of the pid file for the daemon." )

# set some generic include directories
include_directories( SYSTEM ${CMAKE_BINARY_DIR} )
include_directories( SYSTEM source/application )

# find some required boost libraries
find_package( Boost 1.55.0 REQUIRED COMPONENTS system )
include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )

# we need this for using pkg-config for finding libraries
include( FindPkgConfig )

# check if systemd (for logging) is available, if yes: use it
pkg_search_module( SYSTEMD_JOURNAL QUIET libsystemd-journal )
if( SYSTEMD_JOURNAL_FOUND )
	include_directories( SYSTEM ${SYSTEMD_JOURNAL_INCLUDE_DIRS} )
endif()

# process the input header and write the header with the configuration options to the new header file
configure_file( source/application/ngpommedgui.hxx.in ${CMAKE_BINARY_DIR}/ngpommedgui.hxx )

# set some specific compiler flags we need
set( CMAKE_CXX_FLAGS "-Wall -std=c++11" CACHE STRING "Flags used by the compiler during all build types." FORCE )

# a list of all source files of the main executable
set( NGPOMMEDGUI_SOURCE_FILES ${NGPOMMEDGUI_SOURCE_FILES} source/application/main.cxx )

# a list of all header files for the main executable
set( NPGOMMEDGUI_HEADER_FILE ${NPGOMMEDGUI_HEADER_FILE} ${CMAKE_BINARY_DIR}/ngpommedgui.hxx )

# build the main executable
add_executable( ngpommedgui ${NGPOMMEDGUI_SOURCE_FILES} ${NGPOMMEDGUI_HEADER_FILES} ${NPGOMMEDGUI_HEADER_FILE} )
target_link_libraries( ngpommedgui ${Boost_LIBRARIES} ${SYSTEMD_JOURNAL_LIBRARIES} )
install( TARGETS ngpommedgui RUNTIME DESTINATION bin )
