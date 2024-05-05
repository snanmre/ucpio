# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  cpio_newc_test
  cpio_read.cc
  wrong-format.cc
)

target_compile_definitions(cpio_newc_test PRIVATE TEST_FILES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test-files")
target_link_libraries(cpio_newc_test
  PRIVATE
  ucpio::ucpio
  archive
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(cpio_newc_test)