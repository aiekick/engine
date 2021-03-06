set(SRCS
	backward.cpp
	backward.h
)

engine_add_library(
	LIB backward
	SRCS ${SRCS}
)

###############################################################################
# OPTIONS
###############################################################################

set(STACK_WALKING_UNWIND TRUE CACHE BOOL "Use compiler's unwind API")
set(STACK_WALKING_BACKTRACE FALSE CACHE BOOL "Use backtrace from (e)glibc for stack walking")
set(STACK_DETAILS_AUTO_DETECT TRUE CACHE BOOL "Auto detect backward's stack details dependencies")
set(STACK_DETAILS_BACKTRACE_SYMBOL FALSE CACHE BOOL "Use backtrace from (e)glibc for symbols resolution")
set(STACK_DETAILS_DW FALSE CACHE BOOL "Use libdw to read debug info")
set(STACK_DETAILS_BFD FALSE CACHE BOOL "Use libbfd to read debug info")

unset(BACKWARD_LIBRARIES CACHE)
unset(BACKWARD_INCLUDE_DIRS CACHE)

###############################################################################
# CONFIGS
###############################################################################
if (${STACK_DETAILS_AUTO_DETECT})
	include(FindPackageHandleStandardArgs)

	# find libdw
	find_path(LIBDW_INCLUDE_DIR NAMES "elfutils/libdw.h" "elfutils/libdwfl.h")
	find_library(LIBDW_LIBRARY dw)
	set(LIBDW_INCLUDE_DIRS ${LIBDW_INCLUDE_DIR} )
	set(LIBDW_LIBRARIES ${LIBDW_LIBRARY} )
	find_package_handle_standard_args(libdw DEFAULT_MSG LIBDW_LIBRARY LIBDW_INCLUDE_DIR)
	mark_as_advanced(LIBDW_INCLUDE_DIR LIBDW_LIBRARY)

	# find libbfd
	find_path(LIBBFD_INCLUDE_DIR NAMES "bfd.h")
	find_path(LIBDL_INCLUDE_DIR NAMES "dlfcn.h")
	find_library(LIBBFD_LIBRARY bfd)
	find_library(LIBDL_LIBRARY dl)
	set(LIBBFD_INCLUDE_DIRS ${LIBBFD_INCLUDE_DIR} ${LIBDL_INCLUDE_DIR})
	set(LIBBFD_LIBRARIES ${LIBBFD_LIBRARY} ${LIBDL_LIBRARY})
	find_package_handle_standard_args(libbfd DEFAULT_MSG LIBBFD_LIBRARY LIBBFD_INCLUDE_DIR LIBDL_LIBRARY LIBDL_INCLUDE_DIR)
	mark_as_advanced(LIBBFD_INCLUDE_DIR LIBBFD_LIBRARY LIBDL_INCLUDE_DIR LIBDL_LIBRARY)

	if (LIBDW_FOUND)
		list(APPEND BACKWARD_INCLUDE_DIRS ${LIBDW_INCLUDE_DIRS})
		list(APPEND BACKWARD_LIBRARIES ${LIBDW_LIBRARIES})
		set(STACK_DETAILS_DW TRUE)
		set(STACK_DETAILS_BFD FALSE)
		set(STACK_DETAILS_BACKTRACE_SYMBOL FALSE)
	elseif(LIBBFD_FOUND)
		list(APPEND BACKWARD_INCLUDE_DIRS ${LIBBFD_INCLUDE_DIRS})
		list(APPEND BACKWARD_LIBRARIES ${LIBBFD_LIBRARIES})
		set(STACK_DETAILS_DW FALSE)
		set(STACK_DETAILS_BFD TRUE)
		set(STACK_DETAILS_BACKTRACE_SYMBOL FALSE)
	else()
		set(STACK_DETAILS_DW FALSE)
		set(STACK_DETAILS_BFD FALSE)
		set(STACK_DETAILS_BACKTRACE_SYMBOL TRUE)
	endif()
else()
	if (STACK_DETAILS_DW)
		list(APPEND BACKWARD_LIBRARIES dw)
	endif()

	if (STACK_DETAILS_BFD)
		list(APPEND BACKWARD_LIBRARIES bfd dl)
	endif()
endif()

macro(map_definitions var_prefix define_prefix)
	foreach(def ${ARGN})
		if (${${var_prefix}${def}})
			list(APPEND BACKWARD_DEFINITIONS "${define_prefix}${def}=1")
		else()
			list(APPEND BACKWARD_DEFINITIONS "${define_prefix}${def}=0")
		endif()
	endforeach()
endmacro()

map_definitions("STACK_WALKING_" "BACKWARD_HAS_" UNWIND BACKTRACE)
map_definitions("STACK_DETAILS_" "BACKWARD_HAS_" BACKTRACE_SYMBOL DW BFD)

foreach(def ${BACKWARD_DEFINITIONS})
	message(STATUS "${def}")
endforeach()

list(APPEND BACKWARD_INCLUDE_DIRS .)

target_include_directories(backward PUBLIC ${BACKWARD_INCLUDE_DIRS})
target_compile_definitions(backward PUBLIC ${BACKWARD_DEFINITIONS})
target_link_libraries(backward ${BACKWARD_LIBRARIES})
mark_as_advanced(BACKWARD_INCLUDE_DIR BACKWARD_DEFINITIONS BACKWARD_LIBRARIES)
