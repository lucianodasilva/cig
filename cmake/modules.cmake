
include ("${CMAKE_CURRENT_LIST_DIR}/glob_code.cmake")

macro (add_module module_name module_path)
	set ( module_succeded FALSE )

	# check build option for this module
	set ( library_switch_name "${module_name}_build")
	option(${library_switch_name} "Include ${module_name}" ON)

	if (${${library_switch_name}})
		# validate module interdependencies
		set (module_deps ${ARGN})
		list (LENGTH module_deps module_dep_count)

		if (${module_dep_count} GREATER 0)
			set (check_deps TRUE)

			foreach (dep ${module_deps})
				if (NOT TARGET ${dep})
					set (check_deps FALSE)
					message (" - Module dependency '${dep}' not found.")
				endif ()
			endforeach ()

			if (NOT ${check_deps})
				message ("Module '${module_name}' not added. Missing dependency.")
				return ()
			endif ()
		endif ()

		# evaluate and glob parser files
		if (EXISTS ${module_path})

			project (${module_name})

			set ( include_path ${module_path}/include )
			set ( source_path ${module_path}/src )

			if (EXISTS ${include_path})
				include_directories ( ${include_path} )
				glob_code ( ${include_path} header_files )
			endif ()

			if (EXISTS ${source_path})
				glob_code ( ${source_path} source_files )
			endif ()

			set ( module_succeded TRUE )

		else ()
			message ( WARNING "Module path '${module_path}' does not exist")
		endif ()
	endif ()

endmacro ()

macro (eval_cmakelists module_path)
	if (EXISTS ${module_path}/CMakeLists.txt)
		include (${module_path}/CMakeLists.txt)
	endif ()
endmacro ()

macro (add_module_deps module_name)
	set ( module_deps_name "${module_name}_deps")
	foreach (dep ${module_deps})
		set ( dep_dep_name "${dep}_deps")
		list (APPEND ${module_deps_name} ${dep} ${${dep_dep_name}})
	endforeach ()

	target_link_libraries ( ${module_name} ${${module_deps_name}})
endmacro ()

macro (add_custom_module module_name module_path)
	set (${module_name}_modules ${ARGN})
	add_module ( ${module_name} ${module_path} ${ARGN})

	if (${module_succeded})
		eval_cmakelists (${module_path})
	endif ()

endmacro ()

macro (add_static_module module_name module_path)
	set (${module_name}_modules ${ARGN})
	add_module ( ${module_name} ${module_path} ${ARGN})

	if (${module_succeded})
		add_library ( ${module_name} STATIC ${source_files} ${header_files} )
		eval_cmakelists (${module_path})
		add_module_deps (${module_name})
	endif ()

endmacro ()

macro (add_shared_module module_name module_path)
	set (${module_name}_modules ${ARGN})
	add_module ( ${module_name} ${module_path} ${ARGN})

	if (${module_succeded})
		add_library ( ${module_name} SHARED ${source_files} ${header_files} )
		eval_cmakelists (${module_path})
		add_module_deps (${module_name})
	endif ()

endmacro ()

macro (add_executable_module module_name module_path)
	set (${module_name}_modules ${ARGN})
	add_module ( ${module_name} ${module_path} ${ARGN})

	if (${module_succeded})
		add_executable ( ${module_name} ${source_files} ${header_files} )
		eval_cmakelists (${module_path})
		add_module_deps (${module_name})
	endif ()

endmacro ()

macro (add_module_dependencies module_name)
	set ( module_deps_name "${module_name}_deps")
	list (APPEND ${module_deps_name} ${ARGN})
endmacro ()
