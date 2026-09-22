# ananas-designer (designer.pro)
#
# The metadata/business-scheme editor. It embeds the Qt6 form designer built on
# the public Designer API (designer6/aworkbench.*); libQt6DesignerComponents has
# no CMake package on Ubuntu, so it is located with find_library.

find_library(QT6_DESIGNER_COMPONENTS_LIBRARY NAMES Qt6DesignerComponents REQUIRED)

set(ANANAS_DESIGNER_SOURCES
    actiontree.cpp aliaseditor.cpp atreeitems.cpp cfgform.cpp
    deditaction.cpp deditareg.cpp deditcat.cpp deditcfg.cpp deditcolumns.cpp
    deditcommand.cpp deditdialog.cpp deditdoc.cpp deditfield.cpp
    deditimagecollection.cpp deditireg.cpp deditjournal.cpp deditlang.cpp
    deditreport.cpp deditrole.cpp dedittoolbar.cpp deditwebform.cpp
    formdesigner.cpp interfacetree.cpp main.cpp mainform.cpp mdtree.cpp
    roleeditor.cpp
    designer6/aworkbench.cpp)

set(ANANAS_DESIGNER_HEADERS
    actiontree.h aliaseditor.h atreeitems.h cfgform.h
    deditaction.h deditareg.h deditcat.h deditcfg.h deditcolumns.h
    deditcommand.h deditdialog.h deditdoc.h deditfield.h
    deditimagecollection.h deditireg.h deditjournal.h deditlang.h
    deditreport.h deditrole.h dedittoolbar.h deditwebform.h
    formdesigner.h interfacetree.h mainform.h mdtree.h pixmappreview.h
    roleeditor.h
    designer6/aworkbench.h)

set(ANANAS_DESIGNER_FORMS
    cfgform.ui deditaction.ui deditareg.ui deditcat.ui deditcfg.ui
    deditcolumns.ui deditcommand.ui deditdialog.ui deditdoc.ui deditfield.ui
    deditimagecollection.ui deditireg.ui deditjournal.ui deditlang.ui
    deditreport.ui deditrole.ui dedittoolbar.ui deditwebform.ui mainform.ui)

qt_add_executable(ananas-designer
    ${ANANAS_DESIGNER_SOURCES} ${ANANAS_DESIGNER_HEADERS}
    ${ANANAS_DESIGNER_FORMS} designer.qrc)

set_target_properties(ananas-designer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${ANANAS_BIN_DIR}
    BUILD_RPATH "${ANANAS_LIB_DIR};${ANANAS_DESIGNER_DIR};${CMAKE_CURRENT_SOURCE_DIR}/../editor"
    INSTALL_RPATH "/usr/lib/ananas/designer")

target_include_directories(ananas-designer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../editor)

target_link_libraries(ananas-designer PRIVATE
    ananaslib ananasplugin qtscriptedit ananas_common
    Qt6::Designer ${QT6_DESIGNER_COMPONENTS_LIBRARY} qdataschema)

install(TARGETS ananas-designer RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Standalone smoke test for the form-designer wrapper (tools/scripts/smoke-designer.sh).
qt_add_executable(designer-smoke
    designer6/designer_smoke.cpp
    designer6/aworkbench.cpp
    formdesigner.cpp)
set_target_properties(designer-smoke PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${ANANAS_BIN_DIR}
    BUILD_RPATH "${ANANAS_LIB_DIR};${ANANAS_DESIGNER_DIR}")
target_include_directories(designer-smoke PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(designer-smoke PRIVATE
    ananas_common Qt6::Designer ${QT6_DESIGNER_COMPONENTS_LIBRARY})
