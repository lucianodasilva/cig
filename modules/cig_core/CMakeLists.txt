
if (MSVC)
	# add natvis definitions
	target_sources(cig_core PUBLIC ${cig_extra_path}/vs/cig.natvis)
endif()

# deps/catch/include

add_module_dependencies (cig_core ${core_dependencies})

set (cig_core_path ${CMAKE_CURRENT_LIST_DIR})
include(${CMAKE_CURRENT_LIST_DIR}/test/CMakeLists.txt)