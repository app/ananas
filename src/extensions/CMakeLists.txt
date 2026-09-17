# Extension plugins (extensions.pro)

function(ananas_add_extension name)
    cmake_parse_arguments(EXT "" "" "SOURCES" ${ARGN})
    qt_add_library(${name} SHARED ${EXT_SOURCES})
    set_target_properties(${name} PROPERTIES
        OUTPUT_NAME ${name}
        LIBRARY_OUTPUT_DIRECTORY ${ANANAS_LIB_DIR}
        BUILD_RPATH ${ANANAS_LIB_DIR})
    target_link_libraries(${name} PRIVATE
        ananaslib ananas_common ${QDATASCHEMA_LIBRARY})
    install(TARGETS ${name}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/ananas)
endfunction()

ananas_add_extension(aextsql     SOURCES aextsql/aextsql.cpp aextsql/aextsql.h)
ananas_add_extension(aextexample SOURCES example/aextexample.cpp example/aextexample.h)
ananas_add_extension(aext_meta   SOURCES meta/aext_meta.cpp meta/aext_meta.h)
ananas_add_extension(aextservice SOURCES service/aextservice.cpp service/aextservice.h)
ananas_add_extension(aexttext    SOURCES text/aexttext.cpp text/aexttext.h)
ananas_add_extension(aextxml     SOURCES xml/aextxml.cpp xml/aextxml.h)
