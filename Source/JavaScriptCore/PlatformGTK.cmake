set(JavaScriptCore_OUTPUT_NAME javascriptcoregtk-${WEBKITGTK_API_VERSION})

list(APPEND JavaScriptCore_UNIFIED_SOURCE_LIST_FILES
    "SourcesGTK.txt"
)

list(APPEND JavaScriptCore_PRIVATE_INCLUDE_DIRECTORIES
    "${JAVASCRIPTCORE_DIR}/inspector/remote/glib"
)

list(APPEND JavaScriptCore_PRIVATE_FRAMEWORK_HEADERS
    inspector/remote/glib/RemoteInspectorServer.h
    inspector/remote/glib/RemoteInspectorUtils.h
)

configure_file(javascriptcoregtk.pc.in ${CMAKE_BINARY_DIR}/Source/JavaScriptCore/javascriptcoregtk-${WEBKITGTK_API_VERSION}.pc @ONLY)
configure_file(JavaScriptCore.gir.in ${CMAKE_BINARY_DIR}/JavaScriptCore-${WEBKITGTK_API_VERSION}.gir @ONLY)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/JavaScriptCore-${WEBKITGTK_API_VERSION}.typelib
    DEPENDS ${CMAKE_BINARY_DIR}/JavaScriptCore-${WEBKITGTK_API_VERSION}.gir
    COMMAND ${INTROSPECTION_COMPILER} ${CMAKE_BINARY_DIR}/JavaScriptCore-${WEBKITGTK_API_VERSION}.gir -o ${CMAKE_BINARY_DIR}/JavaScriptCore-${WEBKITGTK_API_VERSION}.typelib
)

ADD_TYPELIB(${CMAKE_BINARY_DIR}/JavaScriptCore-${WEBKITGTK_API_VERSION}.typelib)

install(FILES "${CMAKE_BINARY_DIR}/Source/JavaScriptCore/javascriptcoregtk-${WEBKITGTK_API_VERSION}.pc"
        DESTINATION "${LIB_INSTALL_DIR}/pkgconfig"
)

install(FILES ${JavaScriptCore_PUBLIC_FRAMEWORK_HEADERS}
        DESTINATION "${WEBKITGTK_HEADER_INSTALL_DIR}/JavaScriptCore"
)

if (ENABLE_INTROSPECTION)
    install(FILES ${CMAKE_BINARY_DIR}/JavaScriptCore-${WEBKITGTK_API_VERSION}.gir
            DESTINATION ${INTROSPECTION_INSTALL_GIRDIR}
    )
    install(FILES ${CMAKE_BINARY_DIR}/JavaScriptCore-${WEBKITGTK_API_VERSION}.typelib
            DESTINATION ${INTROSPECTION_INSTALL_TYPELIBDIR}
    )
endif ()

add_definitions(-DSTATICALLY_LINKED_WITH_WTF)
add_definitions(-DLIBDIR="${LIB_INSTALL_DIR}")

list(APPEND JavaScriptCore_LIBRARIES
    ${GLIB_LIBRARIES}
)
list(APPEND JavaScriptCore_SYSTEM_INCLUDE_DIRECTORIES
    ${GLIB_INCLUDE_DIRS}
)
