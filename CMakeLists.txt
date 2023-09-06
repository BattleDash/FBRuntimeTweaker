cmake_minimum_required(VERSION 3.15)

project(
	FBRuntimeTweaker
	DESCRIPTION "A runtime loader for Frosty mods"
	HOMEPAGE_URL "https://github.com/BattleDash/FBRuntimeTweaker"
	LANGUAGES C CXX
)

# Disable MSVC's _s warnings
add_compile_definitions(_CRT_SECURE_NO_DEPRECATE)

# Main DLL
include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCE_FILES
	source/array.c
	source/entry_point.c
	source/frosty_mod.c
	source/frosty_resource.c
	source/hook_man.c
	source/parse_common.c
	source/runtime_tweaker.c
	source/util.c
)

add_subdirectory(vendor/minhook)
add_subdirectory(vendor/zstd/build/cmake)

# Global to include testing
link_libraries(libzstd_static)
include_directories(vendor/zstd/lib)

add_library(FBRuntimeTweaker MODULE ${SOURCE_FILES})
target_link_libraries(FBRuntimeTweaker minhook)

# Testing
enable_testing()

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(TEST_SOURCE_FILES
	test/char_array_test.cpp
	test/int_array_test.cpp
	test/frosty_test.cpp
	test/log_test.cpp
)

add_custom_target(copy_test_files ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/test/data
	${CMAKE_BINARY_DIR}/test_data)

add_executable(FBRTTest ${TEST_SOURCE_FILES} ${SOURCE_FILES})
target_link_libraries(FBRTTest GTest::gtest_main minhook)

include(GoogleTest)
gtest_discover_tests(FBRTTest)