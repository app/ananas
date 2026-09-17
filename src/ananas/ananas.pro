include (../plugins/plugins.pri)
include (../lib/lib.pri)
include (../ananas.pri)
#load(qsa)
QT += qml


TARGET = ananas
TEMPLATE = app

DESTDIR = ../../bin

INCLUDEPATH += ../lib ../../tmp/ui/ananas ../plugins
#LIBS += -L../../lib -lananas -L../../lib/designer -lananasplugin 
LIBS += -L../../lib -lananas -L../../lib/designer -lananasplugin -lqdataschema 

# libananasplugin.so is installed under <LIBDIR>/ananas/designer, which is not
# in the default loader path; bake the install location into the binary.
unix:QMAKE_RPATHDIR += /usr/lib/ananas/designer

HEADERS = \
    mainform.h

SOURCES = \
    main.cpp \
    mainform.cpp

#FORMS = qadocjournal.ui

RESOURCE = \
    ananas.qrc
    
TRANSLATIONS = \
    ../../translations/ananas-engine-en.ts \
    ../../translations/ananas-engine-ru.ts 
    
unix {
    target.path = /$(BINDIR)
#    ananas.files = ananas
}
win32{
#    ananas.path =.
#    ananas.extra = CALL create_base.bat
}    

INSTALLS += target

