TEMPLATE = subdirs
SUBDIRS += editor
SUBDIRS  += lib plugins
# The metadata editor (ananas-designer) is part of the Qt6 build since Phase 6.
SUBDIRS += designer
SUBDIRS += ananas admin
SUBDIRS += extensions

#TRANSLATIONS = \
#    ../translations/ananas-en.ts \
#    ../translations/ananas-ru.ts 
