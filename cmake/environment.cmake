
macro (check_environment)

	if (CMAKE_SYSTEM_NAME MATCHES "Windows")
		add_definitions (-Dcig_OS_WINDOWS)
	elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
		add_definitions (-Dcig_OS_LINUX)
	elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
		add_definitions (-Dcig_OS_DARWIN)
	elseif (CMAKE_SYSTEM_NAME MATCHES "Sun")
		add_definitions (-Dcig_OS_SUN)
	elseif (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
		add_definitions (-Dcig_OS_FREEBSD)
	endif ()

	if (UNIX)
		add_definitions (-Dcig_API_UNIX)
	elseif (WIN32)
		add_definitions (-Dcig_API_WIN32)
	endif ()

	if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
		add_definitions (-Dcig_COMPILER_MSVC)
	elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		add_definitions (-Dcig_COMPILER_CLANG)
	elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
		add_definitions (-Dcig_COMPILER_GNU)
	elseif (CMAKE_CXX_COMPILER_ID MATCHES "SunPro")
		add_definitions (-Dcig_COMPILER_SUNPRO)
	endif ()

endmacro ()

macro(set_runtime_to_static)
  if (MSVC)
    foreach (
		flag_var
        CMAKE_CXX_FLAGS 
		CMAKE_CXX_FLAGS_DEBUG 
		CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL 
		CMAKE_CXX_FLAGS_RELWITHDEBINFO
	) 
		string(REPLACE "/MD" "-MT" ${flag_var} "${${flag_var}}")
    endforeach()
  endif()
endmacro()
