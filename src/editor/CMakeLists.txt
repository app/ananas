# qtscriptedit (editor.pro / editor.pri)

qt_add_library(qtscriptedit SHARED
    scripthighlighter.cpp scripthighlighter.h
    textedit.cpp textedit.h
    tabsettings.cpp tabsettings.h
    scriptedit.cpp scriptedit.h)

set_target_properties(qtscriptedit PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
    LIBRARY_OUTPUT_DIRECTORY ${ANANAS_LIB_DIR})

target_link_libraries(qtscriptedit PRIVATE Qt6::Widgets)

install(TARGETS qtscriptedit LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
