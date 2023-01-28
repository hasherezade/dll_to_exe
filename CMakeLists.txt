cmake_minimum_required ( VERSION 3.0 )

# project name:
project ( dll_to_exe_proj )

option (DLL2EXE_BUILD_TESTING "enable testing" OFF)

# libs
# modules:
set ( M_PARSER "libpeconv/libpeconv" )

# modules paths:
set (PECONV_DIR "${CMAKE_SOURCE_DIR}/${M_PARSER}" CACHE PATH "PEConv main path")
add_subdirectory ( ${PECONV_DIR} )
set ( PECONV_LIB $<TARGET_FILE:libpeconv> CACHE PATH "PEConvLib library path" )

# Add sub-directories
#
add_subdirectory ( dll_to_exe )

# Setup testing
if(DLL2EXE_BUILD_TESTING)
	enable_testing()
	add_subdirectory ( tests )
endif()
