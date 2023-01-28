cmake_minimum_required (VERSION 3.0)
project (test_case1)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")

set (srcs
	main.cpp
)

set (hdrs
)

add_library ( ${PROJECT_NAME} SHARED ${hdrs} ${srcs} ${rsrc})

INSTALL( TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT ${PROJECT_NAME} )
