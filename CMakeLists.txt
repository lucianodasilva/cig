cmake_minimum_required(VERSION 3.0)

project (cig)

set ( cig_path ${CMAKE_CURRENT_LIST_DIR})

include (${cig_path}/cmake/environment.cmake)
include (${cig_path}/cmake/modules.cmake)

if (MSVC)
	add_definitions(
		-D_SCL_SECURE_NO_WARNINGS=1
	)
endif()

# setup environment
check_environment ()

#fix runtime to static in windows
set_runtime_to_static ()

set(CMAKE_C_STANDARD 14)
set(CMAKE_CXX_STANDARD 14)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# -----------------
set (cig_extra_path ${CMAKE_CURRENT_LIST_DIR}/extra)

add_static_module (cig_core ${cig_path}/modules/cig_core)
add_static_module (cig_clang ${cig_path}/modules/cig_clang cig_core)
add_executable_module (cig ${cig_path}/modules/cig cig_core cig_clang)

project (cig)
