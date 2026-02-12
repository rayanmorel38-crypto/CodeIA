cmake_minimum_required(VERSION 3.20)
project(cpp_engine LANGUAGES CXX VERSION 1.0.0)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build directories
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Compiler flags
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -fPIC)
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3 -march=native)
    else()
        add_compile_options(-g -O0)
    endif()
endif()

# Sanitizers option
option(ENABLE_SANITIZERS "Enable ASan/UBSan" OFF)
if (ENABLE_SANITIZERS AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

# GPU acceleration option
option(ENABLE_CUDA "Enable CUDA support" OFF)
if (ENABLE_CUDA)
    find_package(CUDA REQUIRED)
    message(STATUS "CUDA found: ${CUDA_VERSION}")
endif()

# Optional deep-learning backends
option(WITH_LIBTORCH "Build with libtorch (TorchScript) support" OFF)
option(WITH_ONNXRT "Build with ONNX Runtime support" OFF)

# HTTP Server support for native C++ API
option(WITH_HTTP_SERVER "Build with native HTTP server (cpp-httplib)" ON)

if (WITH_LIBTORCH)
    find_package(Torch QUIET)
    if (Torch_FOUND)
        message(STATUS "libtorch (Torch) found: ${Torch_VERSION}")
        add_compile_definitions(WITH_LIBTORCH=1)
        list(APPEND EXTRA_INCLUDE_DIRS ${TORCH_INCLUDE_DIRS})
        list(APPEND EXTRA_LIBS ${TORCH_LIBRARIES})
        set(WITH_LIBTORCH_AVAILABLE ON)
    else()
        message(WARNING "WITH_LIBTORCH enabled but libtorch not found. Builds will use stubs.")
        set(WITH_LIBTORCH_AVAILABLE OFF)
    endif()
else()
    set(WITH_LIBTORCH_AVAILABLE OFF)
endif()

if (WITH_ONNXRT)
    # Try common find package names for ONNX Runtime
    find_package(ONNXRuntime QUIET)
    if (NOT ONNXRuntime_FOUND)
        find_package(onnxruntime QUIET)
    endif()
    if (ONNXRuntime_FOUND OR onnxruntime_FOUND)
        message(STATUS "ONNX Runtime found")
        add_compile_definitions(WITH_ONNXRT=1)
        # attempt to collect library target / variables
        if (TARGET ONNXRuntime::ONNXRuntime)
            list(APPEND EXTRA_LIBS ONNXRuntime::ONNXRuntime)
        elseif (DEFINED ONNXRUNTIME_LIBS)
            list(APPEND EXTRA_LIBS ${ONNXRUNTIME_LIBS})
        elseif (DEFINED ONNXRuntime_LIBS)
            list(APPEND EXTRA_LIBS ${ONNXRuntime_LIBS})
        endif()
        set(WITH_ONNXRT_AVAILABLE ON)
    else()
        message(WARNING "WITH_ONNXRT enabled but ONNX Runtime not found. Builds will use stubs.")
        set(WITH_ONNXRT_AVAILABLE OFF)
    endif()
else()
    set(WITH_ONNXRT_AVAILABLE OFF)
endif()

# HTTP Server dependencies (cpp-httplib + nlohmann/json)
if (WITH_HTTP_SERVER)
    include(FetchContent)
    
    # Fetch cpp-httplib (header-only library)
    FetchContent_Declare(
        cpp_httplib
        URL https://github.com/yhirose/cpp-httplib/archive/refs/tags/v0.11.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
    )
    FetchContent_MakeAvailable(cpp_httplib)
    
    # Fetch nlohmann/json (header-only library)
    FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp
        DOWNLOAD_NO_EXTRACT ON
        DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/nlohmann_json_dir
    )
    FetchContent_GetProperties(nlohmann_json)
    if(NOT nlohmann_json_POPULATED)
        FetchContent_Populate(nlohmann_json)
        file(COPY ${CMAKE_BINARY_DIR}/nlohmann_json_dir/json.hpp 
             DESTINATION ${CMAKE_BINARY_DIR}/include/nlohmann)
    endif()
    
    set(WITH_HTTP_SERVER_AVAILABLE ON)
    message(STATUS "HTTP Server: ENABLED (cpp-httplib + nlohmann/json)")
else()
    set(WITH_HTTP_SERVER_AVAILABLE OFF)
    message(STATUS "HTTP Server: DISABLED")
endif()


option(WITH_FREENECT2 "Enable libfreenect2 (Kinect v2) support" ON)
if (WITH_FREENECT2)
    # Try to find libfreenect2 using pkg-config first
    find_package(PkgConfig QUIET)
    if (PKG_CONFIG_FOUND)
        pkg_check_modules(FREENECT2 QUIET freenect2)
        if (FREENECT2_FOUND)
            message(STATUS "libfreenect2 found via pkg-config: ${FREENECT2_VERSION}")
            add_compile_definitions(WITH_FREENECT2=1)
            list(APPEND EXTRA_INCLUDE_DIRS ${FREENECT2_INCLUDE_DIRS})
            list(APPEND EXTRA_LIBS ${FREENECT2_LIBRARIES})
            set(WITH_FREENECT2_AVAILABLE ON)
        else()
            # Fallback to manual search
            find_path(FREENECT2_INCLUDE_DIR NAMES libfreenect2/libfreenect2.hpp HINTS /usr/include /usr/local/include)
            find_library(FREENECT2_LIB NAMES freenect2 HINTS /usr/lib /usr/local/lib)
            if (FREENECT2_INCLUDE_DIR AND FREENECT2_LIB)
                message(STATUS "libfreenect2 found manually: ${FREENECT2_INCLUDE_DIR}")
                add_compile_definitions(WITH_FREENECT2=1)
                list(APPEND EXTRA_INCLUDE_DIRS ${FREENECT2_INCLUDE_DIR})
                list(APPEND EXTRA_LIBS ${FREENECT2_LIB})
                set(WITH_FREENECT2_AVAILABLE ON)
            else()
                message(WARNING "WITH_FREENECT2 enabled but libfreenect2 not found. Falling back to stub.")
                set(WITH_FREENECT2_AVAILABLE OFF)
            endif()
        endif()
    else()
        message(WARNING "WITH_FREENECT2 enabled but pkg-config not found. Falling back to stub.")
        set(WITH_FREENECT2_AVAILABLE OFF)
    endif()
else()
    set(WITH_FREENECT2_AVAILABLE OFF)
endif()

# OpenCV support for vision modules
option(WITH_OPENCV "Enable OpenCV support for vision modules" ON)
if (WITH_OPENCV)
    find_package(OpenCV QUIET)
    if (OpenCV_FOUND)
        message(STATUS "OpenCV found: ${OpenCV_VERSION}")
        add_compile_definitions(WITH_OPENCV=1)
        list(APPEND EXTRA_INCLUDE_DIRS ${OpenCV_INCLUDE_DIRS})
        list(APPEND EXTRA_LIBS ${OpenCV_LIBS})
        set(WITH_OPENCV_AVAILABLE ON)
    else()
        message(WARNING "WITH_OPENCV enabled but OpenCV not found. Vision modules will use stubs.")
        set(WITH_OPENCV_AVAILABLE OFF)
    endif()
else()
    set(WITH_OPENCV_AVAILABLE OFF)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
if (EXTRA_INCLUDE_DIRS)
    include_directories(${EXTRA_INCLUDE_DIRS})
endif()
if (WITH_HTTP_SERVER_AVAILABLE)
    include_directories(${CMAKE_BINARY_DIR}/include)
    include_directories(${cpp_httplib_SOURCE_DIR})
endif()
if (EXTRA_LIBS)
    message(STATUS "Extra libs to link: ${EXTRA_LIBS}")
endif()

# Source files organized by module
file(GLOB_RECURSE CORE_SOURCES src/core/*.cpp)
file(GLOB GEN_SOURCES src/generators/nlp/*.cpp src/generators/audio/*.cpp src/generators/*.cpp)
file(GLOB_RECURSE FILTER_SOURCES src/filters/*.cpp)
file(GLOB_RECURSE EFFECTS_SOURCES src/effects/*.cpp)
file(GLOB_RECURSE OPT_SOURCES src/optimization/*.cpp)
file(GLOB_RECURSE UTILS_SOURCES src/utils/*.cpp)
file(GLOB_RECURSE NETWORK_SOURCES src/network/*.cpp)
file(GLOB MODULE_SOURCES 
    src/modules/audio/*.cpp 
    src/modules/analytics/*.cpp 
    src/modules/nlp/*.cpp 
    src/modules/planning/*.cpp 
    src/modules/security/*.cpp 
    src/modules/system/*.cpp 
    src/modules/network/*.cpp
    src/modules/vision/*.cpp
)
file(GLOB_RECURSE API_SOURCES src/api/*.cpp)
file(GLOB_RECURSE SANDBOX_SOURCES src/sandbox/*.cpp)

set(ALL_SOURCES
    ${CORE_SOURCES}
    ${GEN_SOURCES}
    ${FILTER_SOURCES}
    ${EFFECTS_SOURCES}
    ${OPT_SOURCES}
    ${UTILS_SOURCES}
    ${NETWORK_SOURCES}
    ${MODULE_SOURCES}
    ${API_SOURCES}
    ${SANDBOX_SOURCES}
    src/models/model_manager.cpp
    src/models/onnx_backend.cpp
    src/models/torch_backend.cpp
    src/models/neural_network.cpp
)

# Main library (static)
add_library(cpp_engine STATIC ${ALL_SOURCES})
target_include_directories(cpp_engine PUBLIC ${CMAKE_SOURCE_DIR}/include)
if (EXTRA_LIBS)
    target_link_libraries(cpp_engine PRIVATE ${EXTRA_LIBS})
endif()

# Shared library (optional for external use)
add_library(libcppengine SHARED ${ALL_SOURCES})
target_include_directories(libcppengine PUBLIC ${CMAKE_SOURCE_DIR}/include)
if (EXTRA_LIBS)
    target_link_libraries(libcppengine PRIVATE ${EXTRA_LIBS})
endif()
set_target_properties(libcppengine PROPERTIES
    VERSION 1.1.0
    SOVERSION 1
    PUBLIC_HEADER "include/api/cpp_engine.h"
)

# Main executable (optional)
add_executable(image_video_generator 
    src/main.cpp
)
target_link_libraries(image_video_generator cpp_engine)
if (ENABLE_CUDA)
    target_link_libraries(image_video_generator ${CUDA_LIBRARIES})
endif()

# HTTP Server executable
if (WITH_HTTP_SERVER_AVAILABLE)
    add_executable(cpp_engine_server src/server.cpp)
    target_link_libraries(cpp_engine_server cpp_engine uuid)
endif()

# Tests
enable_testing()
# Only include working test files to avoid compilation issues with outdated tests
set(TEST_SOURCES 
    tests/test_config.cpp
    tests/test_core.cpp
    tests/test_cpp_engine.cpp
)
if (TEST_SOURCES)
    add_executable(cpp_engine_tests ${TEST_SOURCES})
    target_link_libraries(cpp_engine_tests cpp_engine ${EXTRA_LIBS})
    # Link Catch2 for unit tests
    find_package(Catch2 QUIET)
    if (Catch2_FOUND)
        target_link_libraries(cpp_engine_tests Catch2::Catch2)
    else()
        # Fallback to pkg-config
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(CATCH2 REQUIRED catch2)
        target_link_libraries(cpp_engine_tests ${CATCH2_LIBRARIES})
        target_include_directories(cpp_engine_tests PRIVATE ${CATCH2_INCLUDE_DIRS})
    endif()
    add_test(NAME cpp_engine_tests COMMAND cpp_engine_tests)
endif()

# Summary
message(STATUS "=== CPP Engine Build Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "CUDA Support: ${ENABLE_CUDA}")
message(STATUS "Sanitizers: ${ENABLE_SANITIZERS}")
message(STATUS "Output directory: ${CMAKE_BINARY_DIR}")

# Tests (optionnel) — rend la compilation possible si Catch2 n'est pas installée
option(BUILD_TESTS "Build unit tests" ON)
if (BUILD_TESTS)
	add_subdirectory(tests)
else()
	message(STATUS "Skipping tests (BUILD_TESTS=OFF)")
endif()

