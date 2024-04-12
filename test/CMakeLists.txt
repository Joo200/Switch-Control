idf_component_register(
        SRCS
         testRunner.cpp
        INCLUDE_DIRS
        .
        WHOLE_ARCHIVE)

include(FetchContent)

set(INSTALL_GTEST
        OFF
        CACHE BOOL "" FORCE)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)
# list(APPEND COMPONENTS test)
list(APPEND EXTRA_COMPONENT_DIRS test $ENV{IDF_PATH}/tools/mocks/driver)

target_link_libraries(${COMPONENT_LIB} PRIVATE GTest::gmock GTest::gtest)

add_test(NAME unit_tests COMMAND "./weichensteuerung.elf" WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
