# libqdataschema (bundled subproject of the ananas monorepo).
#
# Provides the QDataSchema database-schema synchronisation library used by the
# engine and the designer, plus the qdsadm command line tool.

set(QDATASCHEMA_SOURCES
    lib/qdataschema.cpp
    lib/qdataschemadriver.cpp
    lib/qdataschemaglobal.cpp
    lib/qdataschemadriver-mysql.cpp
    lib/qdataschemadriver-postgresql.cpp
    lib/qdataschemadriver-sqlite.cpp)

set(QDATASCHEMA_HEADERS
    lib/qdataschema.h
    lib/qdataschemadriver.h
    lib/qdataschemaglobal.h
    lib/qdataschemadriver-mysql.h
    lib/qdataschemadriver-postgresql.h
    lib/qdataschemadriver-sqlite.h)

qt_add_library(qdataschema SHARED
    ${QDATASCHEMA_SOURCES} ${QDATASCHEMA_HEADERS})

set_target_properties(qdataschema PROPERTIES
    OUTPUT_NAME qdataschema
    VERSION 1.1.0
    SOVERSION 1
    LIBRARY_OUTPUT_DIRECTORY ${ANANAS_LIB_DIR})

target_include_directories(qdataschema PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_link_libraries(qdataschema PUBLIC Qt6::Sql Qt6::Xml)

# qdsadm: command line database schema administration tool.
qt_add_executable(qdsadm adm/main.cpp)
target_link_libraries(qdsadm PRIVATE qdataschema)
set_target_properties(qdsadm PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ANANAS_BIN_DIR})

install(TARGETS qdataschema LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${QDATASCHEMA_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qdataschema)
install(TARGETS qdsadm RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
