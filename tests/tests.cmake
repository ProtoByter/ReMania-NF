set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/tests)
set(CTEST_BINARY_DIRECTORY ${PROJECT_BINARY_DIR}/tests)

file(GLOB files "tests/test_*.cpp")
file(GLOB libFiles "src/main_app/*.cpp")
add_library(testLib ${libFiles})

foreach(file ${files})
    string(REGEX REPLACE "(^.*/|\\.[^.]*$)" "" file_without_ext ${file})
    add_executable(${file_without_ext} ${file})
    target_link_libraries(${file_without_ext} glfw libglew_shared mbedtls testLib)
    target_include_directories(${file_without_ext} PUBLIC src/main_app)
    add_test(${file_without_ext} ${file_without_ext})
    set_tests_properties(${file_without_ext}
            PROPERTIES
            PASS_REGULAR_EXPRESSION "Test passed")
    set_tests_properties(${file_without_ext}
            PROPERTIES
            FAIL_REGULAR_EXPRESSION "(Exception|Test failed)")
    set_tests_properties(${file_without_ext}
            PROPERTIES
            TIMEOUT 120)
endforeach()