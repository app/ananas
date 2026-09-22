# ananasplugin (plugins.pro + plugins/*/*.pri)

set(ANANASPLUGIN_HEADERS
    aform.h atoolbar.h awidgets_plugin.h eaddobj.h engine.h qwidgetplugin.h
    actionbutton/eactionbutton.h actionbutton/wactionbutton.h
    actionbutton/wactionbutton_plugin.h actionbutton/wactionbutton_taskmenu.h
    catalogue/catalogform.h catalogue/catalogformwidgets.h
    catalogue/ecatalogue.h catalogue/wcatalogue.h catalogue/wcatalogue_plugin.h
    catalogue/wcatalogue_taskmenu.h
    combobox/acombobox.h combobox/acombobox_plugin.h
    dbfield/edbfield.h dbfield/wdbfield.h dbfield/wdbfield_plugin.h
    dbfield/wdbfield_taskmenu.h
    dbtable/edbtable.h dbtable/wdbtable.h dbtable/wdbtable_plugin.h
    dbtable/wdbtable_taskmenu.h
    document/edocument.h document/eselectdoctype.h document/wdocument.h
    document/wdocument_plugin.h document/wdocument_taskmenu.h
    field/efield.h field/addfdialog.h field/wfield.h field/wdateedit.h
    field/wcatalogeditor.h field/wfield_plugin.h field/wfield_taskmenu.h
    grouptree/wgrouptree.h grouptree/wgrouptree_plugin.h
    journal/ejournal.h journal/wjournal.h journal/wjournal_plugin.h
    journal/wjournal_taskmenu.h
    report/ereport.h report/wreport.h report/wreport_plugin.h
    report/wreport_taskmenu.h
    table/etable.h table/wtable.h table/wtable_plugin.h)

set(ANANASPLUGIN_SOURCES
    aform.cpp atoolbar.cpp awidgets_plugin.cpp engine.cpp eaddobj.cpp
    actionbutton/eactionbutton.cpp actionbutton/wactionbutton.cpp
    actionbutton/wactionbutton_plugin.cpp actionbutton/wactionbutton_taskmenu.cpp
    catalogue/catalogform.cpp catalogue/catalogformwidgets.cpp
    catalogue/ecatalogue.cpp catalogue/wcatalogue.cpp
    catalogue/wcatalogue_plugin.cpp catalogue/wcatalogue_taskmenu.cpp
    combobox/acombobox.cpp combobox/acombobox_plugin.cpp
    dbfield/edbfield.cpp dbfield/wdbfield.cpp dbfield/wdbfield_plugin.cpp
    dbfield/wdbfield_taskmenu.cpp
    dbtable/edbtable.cpp dbtable/wdbtable.cpp dbtable/wdbtable_plugin.cpp
    dbtable/wdbtable_taskmenu.cpp
    document/edocument.cpp document/eselectdoctype.cpp document/wdocument.cpp
    document/wdocument_plugin.cpp document/wdocument_taskmenu.cpp
    field/efield.cpp field/addfdialog.cpp field/wfield.cpp field/wdateedit.cpp
    field/wcatalogeditor.cpp field/wfield_plugin.cpp field/wfield_taskmenu.cpp
    grouptree/wgrouptree.cpp grouptree/wgrouptree_plugin.cpp
    journal/ejournal.cpp journal/wjournal.cpp journal/wjournal_plugin.cpp
    journal/wjournal_taskmenu.cpp
    report/ereport.cpp report/wreport.cpp report/wreport_plugin.cpp
    report/wreport_taskmenu.cpp
    table/etable.cpp table/wtable.cpp table/wtable_plugin.cpp)

set(ANANASPLUGIN_FORMS
    eaddobj.ui
    actionbutton/eactionbutton.ui
    catalogue/catalogform.ui catalogue/ecatalogue.ui
    dbfield/edbfield.ui
    dbtable/edbtable.ui
    document/edocument.ui document/eselectdoctype.ui
    field/efield.ui field/addfdialog.ui
    journal/ejournal.ui
    report/ereport.ui
    table/etable.ui)

qt_add_library(ananasplugin SHARED
    ${ANANASPLUGIN_HEADERS} ${ANANASPLUGIN_SOURCES} ${ANANASPLUGIN_FORMS}
    plugins.qrc)

set_target_properties(ananasplugin PROPERTIES
    OUTPUT_NAME ananasplugin
    LIBRARY_OUTPUT_DIRECTORY ${ANANAS_DESIGNER_DIR}
    BUILD_RPATH ${ANANAS_LIB_DIR})

target_link_libraries(ananasplugin PRIVATE
    ananaslib ananas_common Qt6::Designer qdataschema)

# Expose the plugin AUTOUIC headers to consumers (symmetry with libananas).
target_include_directories(ananas_common INTERFACE
    ${CMAKE_CURRENT_BINARY_DIR}/ananasplugin_autogen/include)

install(TARGETS ananasplugin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/ananas/designer)
install(FILES aform.h atoolbar.h awidgets_plugin.h eaddobj.h engine.h
        qwidgetplugin.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ananas)
