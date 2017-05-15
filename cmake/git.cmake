
find_program (git_executable "git")

# get a revision count for the current 
macro (git_revision object repo_root revision)

	if (git_executable STREQUAL "git_executable-NOTFOUND")
		message ( WARNING "Git executable not found. Revision count set to empty.")
		set (revision "")
	else ()
		if (EXISTS "${repo_root}/.git")
			execute_process (
				COMMAND ${git_executable} rev-list --count ${object}
				WORKING_DIRECTORY ${repo_root}
				RESULT_VARIABLE git_result
				OUTPUT_VARIABLE ${revision}
				OUTPUT_STRIP_TRAILING_WHITESPACE
			)

			if (NOT git_result STREQUAL "0")
				message (WARNING "Git execution failed with result '${git_result}'")
				set (revision "")
			endif ()

		else ()
			message ( WARNING "Git root for project at path '${repo_root}' not found. Revision count set to empty." )
			set (revision "")
		endif ()
	endif()
endmacro (git_revision)

macro (git_revision_hash object repo_root revision)

	if (git_executable STREQUAL "git_executable-NOTFOUND")
		message ( WARNING "Git executable not found. Revision hash set to empty.")
		set (revision "")
	else ()
		if (EXISTS "${repo_root}/.git")
			execute_process (
				COMMAND ${git_executable} rev-parse --short ${object}
				WORKING_DIRECTORY ${repo_root}
				RESULT_VARIABLE git_result
				OUTPUT_VARIABLE ${revision}
				OUTPUT_STRIP_TRAILING_WHITESPACE
			)

			if (NOT git_result STREQUAL "0")
				message (WARNING "Git execution failed with result '${git_result}'")
				set (revision "")
			endif ()

		else ()
			message ( WARNING "Git root for project at path '${repo_root}' not found. Revision hash set to empty." )
			set (revision "")
		endif ()
	endif ()
endmacro (git_revision_hash)