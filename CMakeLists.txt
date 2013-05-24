cmake_minimum_required (VERSION 2.6)
project (elliptics-wrapper)

FILE (READ "${CMAKE_CURRENT_SOURCE_DIR}/debian/changelog" DEBCHANGELOG)

string(REGEX MATCH "([0-9]+\\.[0-9]+\\.[0-9]+)" DEBFULLVERSION "${DEBCHANGELOG}")
STRING (REGEX MATCH "([0-9]+\\.[0-9]+)" WOOKIE_VERSION_MAJOR "${DEBFULLVERSION}")
STRING (REGEX MATCH "([0-9]+$)" WOOKIE_VERSION_MINOR "${DEBFULLVERSION}")

set(CMAKE_CXX_FLAGS "-g -std=c++0x -W -Wall -Wextra -fstack-protector-all")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(ELLIPTICS_WRAPPER_CPP
	src/wrapper.h
	src/wrapper.cpp
)

add_library(elliptics-wrapper MODULE src/wrapper)

install(TARGETS elliptics-wrapper
	LIBRARY DESTINATION lib/elliptics)

set_target_properties(elliptics-wrapper PROPERTIES
	PREFIX ""
	SUFFIX ".plugin")

target_link_libraries(elliptics-wrapper elliptics_cpp)
