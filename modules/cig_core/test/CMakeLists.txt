
option(CIG_CORE_RUN_TESTS "Run CIS core tests")

if (${CIG_CORE_RUN_TESTS})

    include_directories(${cig_core_path}/deps/catch/include/)

    glob_code (${CMAKE_CURRENT_LIST_DIR} cig_test_sources)

    project(cig_core_test)

    include(CTest)
    enable_testing()

    add_executable(cig_core_test ${cig_test_sources})
    target_link_libraries(cig_core_test cig_core ${core_dependencies})

	add_test (cig_core_test cig_core_test)
    
endif()