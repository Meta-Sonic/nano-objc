cmake_minimum_required(VERSION 3.22.0)

# Add nano cmake.
# execute_process(COMMAND wget -O - https://nano-cmake.onrender.com/nano.cmake OUTPUT_VARIABLE NANO_CMAKE ERROR_QUIET)
# cmake_language(EVAL CODE ${NANO_CMAKE})
include(FetchContent)

project(nano-objc VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(NANO_OBJC_BUILD_TESTS "Build tests." OFF)
option(NANO_OBJC_DEV "Development build" OFF)

# Fetch nano-common.
if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../nano-common")
    set(FETCHCONTENT_SOURCE_DIR_NANO_COMMON "${CMAKE_CURRENT_SOURCE_DIR}/../nano-common")
endif()

FetchContent_Declare(nano_common GIT_REPOSITORY "https://github.com/Meta-Sonic/nano-common.git")
FetchContent_MakeAvailable(nano_common)

# nano-objc lib.
set(NANO_OBJC_NAME objc)
set(NANO_OBJC_MODULE_NAME nano-objc)

set(NANO_OBJC_SRC_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/nano")

file(GLOB_RECURSE NANO_OBJC_SOURCE_FILES
    "${NANO_OBJC_SRC_DIRECTORY}/*.h"
    "${NANO_OBJC_SRC_DIRECTORY}/*.cpp")

add_library(${NANO_OBJC_MODULE_NAME} STATIC ${NANO_OBJC_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${NANO_OBJC_SOURCE_FILES})
target_include_directories(${NANO_OBJC_MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${NANO_OBJC_MODULE_NAME} PUBLIC nano::common)

add_library(nano::${NANO_OBJC_NAME} ALIAS ${NANO_OBJC_MODULE_NAME})

set_target_properties(${NANO_OBJC_MODULE_NAME} PROPERTIES XCODE_GENERATE_SCHEME OFF)

if (APPLE) 
    target_link_libraries(${NANO_OBJC_MODULE_NAME} PUBLIC
        "-framework CoreFoundation"
    )
else()
    # target_link_libraries(${PROJECT_NAME} PUBLIC
    #     Gdiplus.lib
    #     d2d1
    # )
    
endif()

if (NANO_OBJC_DEV)
    set(NANO_OBJC_BUILD_TESTS ON)
    # nano_clang_format(${NANO_OBJC_MODULE_NAME} ${NANO_OBJC_SOURCE_FILES})
endif()

if (NANO_OBJC_BUILD_TESTS)
    nano_add_module(test DEV_MODE)

    file(GLOB_RECURSE TEST_SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.h")

    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/tests" FILES ${TEST_SOURCE_FILES})

    set(TEST_NAME nano-${NANO_OBJC_NAME}-tests)
    add_executable(${TEST_NAME} ${TEST_SOURCE_FILES})
    target_include_directories(${TEST_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    target_link_libraries(${TEST_NAME} PUBLIC nano::test ${NANO_OBJC_MODULE_NAME})

    set(CLANG_OPTIONS -Weverything -Wno-c++98-compat)
    set(MSVC_OPTIONS /W4)

    target_compile_options(${TEST_NAME} PUBLIC
        "$<$<CXX_COMPILER_ID:Clang,AppleClang>:${CLANG_OPTIONS}>"
        "$<$<CXX_COMPILER_ID:MSVC>:${MSVC_OPTIONS}>")

    # set_target_properties(${TEST_NAME} PROPERTIES CXX_STANDARD 20)
endif()