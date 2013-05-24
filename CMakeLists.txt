cmake_minimum_required (VERSION 2.6)
project (elliptics-awrapper)

FILE (READ "${CMAKE_CURRENT_SOURCE_DIR}/debian/changelog" DEBCHANGELOG)

string(REGEX MATCH "([0-9]+\\.[0-9]+\\.[0-9]+)" DEBFULLVERSION "${DEBCHANGELOG}")
STRING (REGEX MATCH "([0-9]+\\.[0-9]+)" WOOKIE_VERSION_MAJOR "${DEBFULLVERSION}")
STRING (REGEX MATCH "([0-9]+$)" WOOKIE_VERSION_MINOR "${DEBFULLVERSION}")

set(CMAKE_CXX_FLAGS "-g -std=c++0x -W -Wall -Wextra -fstack-protector-all")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

add_library(elliptics-awrapper MODULE src/wrapper)

install(TARGETS elliptics-awrapper
	LIBRARY DESTINATION lib/elliptics)

set_target_properties(elliptics-awrapper PROPERTIES
	PREFIX ""
	SUFFIX ".plugin")

target_link_libraries(elliptics-awrapper elliptics_cpp)
