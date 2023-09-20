file(GLOB PUZZLE_TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/*/*test.cpp")

##########################################
# "make check-tests"
##########################################
add_custom_target(build-tests COMMAND ${CMAKE_CTEST_COMMAND} --show-only)
add_custom_target(check-tests COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

##########################################
# "make XYZ_test"
##########################################

foreach (puzzle_test_source ${PUZZLE_TEST_SOURCES})
    # Create a human readable name.
    get_filename_component(puzzle_test_filename ${puzzle_test_source} NAME)
    string(REPLACE ".cpp" "" puzzle_test_name ${puzzle_test_filename})

    # Add the test target separately and as part of "make check-tests".
    add_executable(${puzzle_test_name} EXCLUDE_FROM_ALL ${puzzle_test_source})
    add_dependencies(build-tests ${puzzle_test_name})
    add_dependencies(check-tests ${puzzle_test_name})

    target_link_libraries(${puzzle_test_name} ${PUZZLE_LIBS} gtest gmock_main)

    # Set test target properties and dependencies.
    set_target_properties(${puzzle_test_name}
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
            COMMAND ${puzzle_test_name}
            )

    # Add the test under CTest.
    add_test(${puzzle_test_name} ${CMAKE_BINARY_DIR}/test/${puzzle_test_name} --gtest_color=yes
            --gtest_output=xml:${CMAKE_BINARY_DIR}/test/${puzzle_test_name}.xml)
endforeach()