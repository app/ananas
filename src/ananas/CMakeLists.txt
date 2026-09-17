# ananas (ananas.pro)

qt_add_executable(ananas
    main.cpp mainform.cpp mainform.h
    ananas.qrc)

set_target_properties(ananas PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${ANANAS_BIN_DIR}
    BUILD_RPATH "${ANANAS_LIB_DIR};${ANANAS_DESIGNER_DIR}")

target_link_libraries(ananas PRIVATE
    ananaslib ananasplugin ananas_common ${QDATASCHEMA_LIBRARY})

# libananasplugin.so lives under <libdir>/ananas/designer, which is not in the
# default loader path.
set_target_properties(ananas PROPERTIES INSTALL_RPATH "/usr/lib/ananas/designer")

install(TARGETS ananas RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
