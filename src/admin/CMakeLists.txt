# ananas-administrator (admin.pro)

qt_add_executable(ananas-administrator
    main.cpp atreeitems.cpp mdtree.cpp mainform.cpp usersform.cpp rolesform.cpp
    deditpermissions.cpp dselectuser.cpp deditrole.cpp dedituser.cpp
    dselectrole.cpp
    atreeitems.h mdtree.h mainform.h usersform.h rolesform.h deditpermissions.h
    dselectuser.h deditrole.h dedituser.h dselectrole.h
    mainform.ui usersform.ui rolesform.ui deditpermissions.ui dselectuser.ui
    deditrole.ui dedituser.ui dselectrole.ui
    admin.qrc)

set_target_properties(ananas-administrator PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${ANANAS_BIN_DIR}
    BUILD_RPATH "${ANANAS_LIB_DIR};${ANANAS_DESIGNER_DIR}"
    INSTALL_RPATH "/usr/lib/ananas/designer")

target_link_libraries(ananas-administrator PRIVATE
    ananaslib ananasplugin ananas_common qdataschema)

install(TARGETS ananas-administrator RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
