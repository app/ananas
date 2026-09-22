include(../plugins/plugins.pri)
include(../lib/lib.pri)
TARGET = ananas-designer
TEMPLATE = app

DESTDIR = ../../bin

INCLUDEPATH += . ./designer6 ../lib ../../tmp/ui/ananas ../plugins ../editor
LIBS += -L../../lib -lananas -L../../lib/designer -lananasplugin -lqdataschema -L../editor -lqtscriptedit
unix {
    LIBS += -lQt6DesignerComponents
}
win32{
    LIBS += -lQt6DesignerComponents
}

MOC_DIR = ../../tmp/moc/$$TARGET
OBJECTS_DIR = ../../tmp/obj/$$TARGET
UI_DIR = ../../tmp/ui/$$TARGET

#LIBS	+= -L../lib -L../plugins -lananas -lananasplugin -lqsa -ldesignercore -lqassistantclient
#LIBS	+= -L. -L../lib -L../plugins -lananas -lananasplugin -lqsa -lformdesigner -lqassistantclient
#INCLUDEPATH	+= ../lib ../lib/tmp/ui ../lib/.ui $(QTDIR)/tools/designer/designer ../../qt-headers/tools/designer/designer /usr/lib
#INCLUDEPATH	+= ../lib ../lib/tmp/ui ../lib/.ui ./formdesigner /usr/lib

include ( ../ananas.pri )
QT += designer
QT += qml

HEADERS	= \
    actiontree.h \
    aliaseditor.h \
    atreeitems.h \
    cfgform.h \
#    cfgform.ui.h \
    deditaction.h \
#    deditaction.ui.h \
    deditareg.h \
#    deditareg.ui.h \
    deditcat.h \
#    deditcat.ui.h \
    deditcfg.h \
#    deditcfg.ui.h \
    deditcolumns.h \
#    deditcolumns.ui.h \
    deditcommand.h \
#    deditcommand.ui.h \
    deditdialog.h \
#    deditdialog.ui.h \
    deditdoc.h \
#    deditdoc.ui.h \
    deditfield.h \
#    deditfield.ui.h \
    deditimagecollection.h \
#    deditimagecollection.ui.h \
    deditireg.h \
#    deditireg.ui.h \
    deditjournal.h \
#    deditjournal.ui.h \
    deditlang.h \
#    deditlang.ui.h \
    deditreport.h \
#    deditreport.ui.h \
    deditrole.h \
#    deditrole.ui.h \
    dedittoolbar.h \
#    dedittoolbar.ui.h \
    deditwebform.h \
#    deditwebform.ui.h \
    formdesigner.h \
    interfacetree.h \
    mainform.h \
#    mainform.ui.h \
    mdtree.h \
    pixmappreview.h \
    roleeditor.h     \
    designer6/aworkbench.h

SOURCES	= \
    actiontree.cpp \
    aliaseditor.cpp \
    atreeitems.cpp \
    cfgform.cpp \
    deditaction.cpp \
    deditareg.cpp \
    deditcat.cpp \
    deditcfg.cpp \
    deditcolumns.cpp \
    deditcommand.cpp \
    deditdialog.cpp \
    deditdoc.cpp \
    deditfield.cpp \
    deditimagecollection.cpp \
    deditireg.cpp \
    deditjournal.cpp \
    deditlang.cpp \
    deditreport.cpp \
    deditrole.cpp \
    dedittoolbar.cpp \
    deditwebform.cpp \
    formdesigner.cpp \
    interfacetree.cpp \
    main.cpp \
    mainform.cpp \
    mdtree.cpp \
    roleeditor.cpp     \
    designer6/aworkbench.cpp

FORMS = \
    cfgform.ui \
    deditaction.ui \
    deditareg.ui \
    deditcat.ui \
    deditcfg.ui \
    deditcolumns.ui \
    deditcommand.ui \
    deditdialog.ui \
    deditdoc.ui \
    deditfield.ui \
    deditimagecollection.ui \
    deditireg.ui \
    deditjournal.ui \
    deditlang.ui \
    deditreport.ui \
    deditrole.ui \
    dedittoolbar.ui \
    deditwebform.ui \
    mainform.ui

RESOURCES = \
    designer.qrc 
#    formdesigner/designer.qrc

TRANSLATIONS = \
    ../../translations/ananas-designer-en.ts \
    ../../translations/ananas-designer-ru.ts 

unix {
	target.path = /$(BINDIR)
#	adesigner.files = ananas-designer
#	acfg.path = /etc/ananas
#	acfg.files= testwrite.cfg
     }	

INSTALLS += target


