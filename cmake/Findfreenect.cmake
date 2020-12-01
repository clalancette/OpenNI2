# - try to find the freenect library
#
# Cache Variables: (probably not for direct use in your scripts)
#  FREENECT_INCLUDE_DIR
#  FREENECT_SOURCE_DIR
#  FREENECT_LIBRARY
#
# Non-cache variables you might use in your CMakeLists.txt:
#  FREENECT_FOUND
#  FREENECT_INCLUDE_DIRS
#  FREENECT_LIBRARIES
#
# Requires these CMake modules:
#  FindPackageHandleStandardArgs (known included with CMake >=2.6.2)

set(FREENECT_ROOT_DIR
    "${FREENECT_ROOT_DIR}"
	CACHE
	PATH
    "Directory to search for freenect")

find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  message(STATUS "CHRIS: Found pkg_config")
	pkg_check_modules(PC_LIBFREENECT libfreenect)
endif()

find_library(FREENECT_LIBRARY
	NAMES
	freenect
	PATHS
	${PC_LIBFREENECT_LIBRARY_DIRS}
	${PC_LIBFREENECT_LIBDIR}
	HINTS
	"${FREENECT_ROOT_DIR}"
	PATH_SUFFIXES
	lib
	)

get_filename_component(_libdir "${FREENECT_LIBRARY}" PATH)

message(STATUS "INCLUDE_DIRS: ${PC_LIBFREENECT_INCLUDE_DIRS}")
message(STATUS "INCLUDEDIR: ${PC_LIBFREENECT_INCLUDEDIR}")
find_path(FREENECT_INCLUDE_DIR
	NAMES
	libfreenect.h
	PATHS
	${PC_LIBFREENECT_INCLUDE_DIRS}
	${PC_LIBFREENECT_INCLUDEDIR}
	HINTS
	"${_libdir}"
	"${_libdir}/.."
	"${FREENECT_ROOT_DIR}"
	PATH_SUFFIXES
	include
	)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FREENECT
	DEFAULT_MSG
	FREENECT_LIBRARY
	FREENECT_INCLUDE_DIR
	)

if(FREENECT_FOUND)
	list(APPEND FREENECT_LIBRARIES ${FREENECT_LIBRARY})
	list(APPEND FREENECT_INCLUDE_DIRS ${FREENECT_INCLUDE_DIR} ${PC_LIBFREENECT_INCLUDE_DIRS})
	mark_as_advanced(FREENECT_ROOT_DIR)
endif()

mark_as_advanced(FREENECT_INCLUDE_DIR
	FREENECT_LIBRARY)
