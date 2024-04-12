# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

if (IDF_TARGET STREQUAL linux)
    set(SDKCONFIG "${CMAKE_CURRENT_LIST_DIR}/sdkconfig.utest")
    enable_testing()
    list(APPEND COMPONENTS main test)
    list(APPEND EXTRA_COMPONENT_DIRS test $ENV{IDF_PATH}/tools/mocks/driver)
endif()

set(PROJECT_VER "1.0.0")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(switch-control VERSION ${PROJECT_VER})
