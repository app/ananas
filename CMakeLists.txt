# CMake build for the Qt6 Ananas tree.
#
# This is an alternative to the qmake build (src/*.pro), added during the
# Qt6 port. Both build systems coexist; qmake remains in place.
#
# Configure/build out of source, e.g.:
#   cmake -S . -B cmake-build -DCMAKE_BUILD_TYPE=Debug
#   cmake --build cmake-build -j
#
# The build tree mirrors the qmake output layout (lib/, lib/designer/, bin/).

cmake_minimum_required(VERSION 3.16)

project(ananas VERSION 0.9.7 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Qt6 uic defaults to pointer-to-member connections, which fail to compile for
# forms whose slots live in the derived class (not in the base widget). Keep
# the string-based connection syntax, like the qmake build does.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_OPTIONS -c string)

# Some generated ui_*.h headers #include local headers of their own target
# (e.g. admin's ui_mainform.h includes usersform.h); keep the current source
# directory on the include path.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core Gui Widgets PrintSupport Sql Xml Qml Designer Test)

# Common include paths and Qt modules: the CMake analog of ananas.pri,
# lib.pri and plugins.pri.
add_library(ananas_common INTERFACE)
target_include_directories(ananas_common INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/metadata
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/objects
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/report
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/sys
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/actionbutton
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/catalogue
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/combobox
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/dbfield
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/dbtable
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/document
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/field
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/grouptree
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/journal
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/report
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/table)
target_link_libraries(ananas_common INTERFACE
    Qt6::Widgets Qt6::PrintSupport Qt6::Sql Qt6::Xml Qt6::Qml)
target_compile_features(ananas_common INTERFACE cxx_std_17)

# Output layout mirroring qmake (lib/, lib/designer/, bin/).
set(ANANAS_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(ANANAS_DESIGNER_DIR ${ANANAS_LIB_DIR}/designer)
set(ANANAS_BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)

include(GNUInstallDirs)

add_subdirectory(src/qdataschema)
add_subdirectory(src/lib)
add_subdirectory(src/editor)
add_subdirectory(src/plugins)
add_subdirectory(src/ananas)
add_subdirectory(src/admin)
add_subdirectory(src/designer)
add_subdirectory(src/extensions)
add_subdirectory(src/test)
