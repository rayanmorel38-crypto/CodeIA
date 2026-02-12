cmake_minimum_required(VERSION 3.16)
project(cpp_engine_tests)

# Use include directory from parent project
include_directories(${CMAKE_SOURCE_DIR}/include)

# Find packages
find_package(OpenCV REQUIRED)
find_package(Catch2 REQUIRED)

# Build test executables and link against the cpp_engine library target
add_executable(test_executor test_executor_main.cpp)
target_link_libraries(test_executor PRIVATE cpp_engine)
target_compile_features(test_executor PRIVATE cxx_std_17)

add_executable(test_tasks test_tasks_main.cpp)
target_link_libraries(test_tasks PRIVATE cpp_engine)
target_compile_features(test_tasks PRIVATE cxx_std_17)

# Main test suite with Catch2
add_executable(cpp_engine_test
    main.cpp
    test_utils.cpp
    test_sandbox.cpp
)
target_link_libraries(cpp_engine_test PRIVATE
    cpp_engine
    Catch2::Catch2
    ${OpenCV_LIBS}
)
target_compile_features(cpp_engine_test PRIVATE cxx_std_17)

# Register tests with CTest
enable_testing()
add_test(NAME test_executor COMMAND test_executor)
add_test(NAME test_tasks COMMAND test_tasks)
add_test(NAME cpp_engine_test COMMAND cpp_engine_test)

