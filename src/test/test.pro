TARGET	= ananas-test 
include(../lib/lib.pri)
include(../plugins/plugins.pri)
include(../ananas.pri )

SOURCES	+= \
	    main.cpp \ 
	    testametadata.cpp

HEADERS	+=  \
	    test.h \
	    testametadata.h
TRANSLATES = 

CONFIG +=qtestlib
QT += script scripttools
FORMS	= 
TEMPLATE	=app
INCLUDEPATH	+= ../lib ../plugins
LIBS	+= -L../../lib -lananas
LIBS	+= -L../../lib/designer -lananasplugin -lqdataschema
