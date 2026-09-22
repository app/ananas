# ananas-test (test.pro)

qt_add_executable(ananas-test
    main.cpp testametadata.cpp
    test.h testametadata.h)

set_target_properties(ananas-test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${ANANAS_BIN_DIR}
    BUILD_RPATH "${ANANAS_LIB_DIR};${ANANAS_DESIGNER_DIR}")

target_link_libraries(ananas-test PRIVATE
    ananaslib ananasplugin ananas_common Qt6::Test qdataschema)
