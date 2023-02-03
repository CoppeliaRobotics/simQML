cmake_minimum_required(VERSION 3.5)
project(simExtQML)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_MACOSX_RPATH 1)

find_package(Boost REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets PrintSupport Qml Quick QuickWidgets QuickControls2 REQUIRED)
if(Qt5_VERSION VERSION_GREATER_EQUAL 5.15)
    find_package(Qt5 COMPONENTS Quick3D)
endif()

if(NOT COPPELIASIM_INCLUDE_DIR)
    if(DEFINED ENV{COPPELIASIM_ROOT_DIR})
        set(COPPELIASIM_INCLUDE_DIR $ENV{COPPELIASIM_ROOT_DIR}/programming/include)
    else()
        set(COPPELIASIM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)
    endif()
endif()
list(APPEND CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules
    ${COPPELIASIM_INCLUDE_DIR}/cmake)
find_package(CoppeliaSim 4.5.0.0 REQUIRED)

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR}/generated)

coppeliasim_generate_stubs(${CMAKE_CURRENT_BINARY_DIR}/generated XML_FILE ${CMAKE_CURRENT_SOURCE_DIR}/callbacks.xml LUA_FILE ${CMAKE_CURRENT_SOURCE_DIR}/simQML.lua)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set(SOURCES
    UI.cpp
    SIM.cpp
    Bridge.cpp
    plugin.cpp
)

set(LIBRARIES
    Boost::boost
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::PrintSupport
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickWidgets
    #Qt5::QuickControls2
)

if(Qt5_Quick3D_FOUND)
    list(APPEND SOURCES Geometry.cpp)
    list(APPEND LIBRARIES Qt5::Quick3D)
endif()

coppeliasim_add_plugin(simExtQML SOURCES ${SOURCES})
if(Qt5_Quick3D_FOUND)
    target_compile_options(simExtQML PRIVATE Qt5_Quick3D_FOUND)
endif()
target_link_libraries(simExtQML ${LIBRARIES})
coppeliasim_add_resource_directory(qml)
