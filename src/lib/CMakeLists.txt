# libananas (qmake target "ananas", lib.pro + lib/*.pri)

set(ANANAS_LIB_HEADERS
    acfg.h acfgrc.h acmanifest.h acontainer.h adataexchange.h adatafield.h
    aextension.h aextensionplugin.h aextensionfactory.h afilter.h alog.h
    amenubar.h amoney.h ananas.h ananasglobal.h arole.h aservice.h asqlfield.h
    asqltable.h atests.h atime.h auser.h rclistviewitem.h
    metadata/ametadata.h metadata/ametaobject.h metadata/ametaobjectgroup.h
    metadata/ametafield.h metadata/ametainfo.h metadata/ametaglobal.h
    metadata/ametadocument.h metadata/ametacatalogue.h
    metadata/ametainforegister.h metadata/ametaaccregister.h
    metadata/ametareport.h metadata/ametajournal.h metadata/ametaform.h
    metadata/ametadataio.h metadata/ametadataioxml.h
    dialogs/deditrc.h dialogs/dhelpcfgform.h dialogs/dimportdb.h
    dialogs/dlogin.h dialogs/dselectdb.h dialogs/messageswindow.h
    dialogs/awindowslist.h
    objects/aobject.h objects/aaregister.h objects/acatalogue.h
    objects/adocjournal.h objects/adocument.h objects/airegister.h
    objects/aobjectlist.h
    widgets/awidget.h
    report/acalctemplate.h report/aootemplate.h report/areport.h
    report/atemplate.h report/itemplate.h
    sys/adatabase.h sys/aapplication.h)

set(ANANAS_LIB_SOURCES
    acfg.cpp acfgrc.cpp acmanifest.cpp acontainer.cpp adataexchange.cpp
    adatafield.cpp aextension.cpp aextensionplugin.cpp aextensionfactory.cpp
    afilter.cpp alog.cpp amenubar.cpp amoney.cpp ananas.cpp arole.cpp
    aservice.cpp asqlfield.cpp asqltable.cpp atests.cpp atime.cpp auser.cpp
    rclistviewitem.cpp
    metadata/ametadata.cpp metadata/ametaobject.cpp metadata/ametaobjectgroup.cpp
    metadata/ametafield.cpp metadata/ametainfo.cpp metadata/ametaglobal.cpp
    metadata/ametadocument.cpp metadata/ametacatalogue.cpp
    metadata/ametainforegister.cpp metadata/ametaaccregister.cpp
    metadata/ametareport.cpp metadata/ametajournal.cpp metadata/ametaform.cpp
    metadata/ametadataio.cpp metadata/ametadataioxml.cpp
    dialogs/awindowslist.cpp dialogs/deditrc.cpp dialogs/dhelpcfgform.cpp
    dialogs/dimportdb.cpp dialogs/dlogin.cpp dialogs/dselectdb.cpp
    dialogs/messageswindow.cpp
    objects/aobject.cpp objects/aaregister.cpp objects/acatalogue.cpp
    objects/adocjournal.cpp objects/adocument.cpp objects/airegister.cpp
    objects/aobjectlist.cpp
    widgets/awidget.cpp
    report/acalctemplate.cpp report/aootemplate.cpp report/areport.cpp
    report/atemplate.cpp report/itemplate.cpp
    sys/adatabase.cpp sys/aapplication.cpp)

set(ANANAS_LIB_FORMS
    dialogs/deditrc.ui dialogs/dhelpcfgform.ui dialogs/dlogin.ui
    dialogs/dselectdb.ui)

qt_add_library(ananaslib SHARED
    ${ANANAS_LIB_HEADERS} ${ANANAS_LIB_SOURCES} ${ANANAS_LIB_FORMS}
    lib.qrc)

set_target_properties(ananaslib PROPERTIES
    OUTPUT_NAME ananas
    VERSION 1.0.1
    SOVERSION 1
    LIBRARY_OUTPUT_DIRECTORY ${ANANAS_LIB_DIR})

target_link_libraries(ananaslib PRIVATE ananas_common ${QDATASCHEMA_LIBRARY})

# Public libananas headers (e.g. dselectdb.h) include generated ui_*.h headers;
# expose the AUTOUIC include directory to every consumer of ananas_common.
target_include_directories(ananas_common INTERFACE
    ${CMAKE_CURRENT_BINARY_DIR}/ananaslib_autogen/include)

install(TARGETS ananaslib LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${ANANAS_LIB_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ananas)
