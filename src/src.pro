TEMPLATE = subdirs
SUBDIRS += editor
SUBDIRS  += lib plugins
# Designer is excluded from the first porting milestone (see tools/docs/PORTING.md).
#SUBDIRS += designer/formdesigner designer
SUBDIRS += ananas admin
SUBDIRS += extensions

#TRANSLATIONS = \
#    ../translations/ananas-en.ts \
#    ../translations/ananas-ru.ts 
