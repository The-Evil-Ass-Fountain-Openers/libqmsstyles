qt_add_qml_module(qmsstylesplugin
    URI qmsstyles
    VERSION 1.0
    SHARED
    PLUGIN_TARGET qmsstylesplugin
    CLASS_NAME QmsstylesPlugin
    SOURCES qmsstyleswrapper.cpp
)

target_link_libraries(qmsstylesplugin
    PRIVATE
        Qt6::Qml
        qmsstyles
)

install(TARGETS qmsstylesplugin DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt6/qml/qmsstyles/)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qmldir DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt6/qml/qmsstyles/)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qmsstylesplugin.qmltypes DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt6/qml/qmsstyles/)

add_subdirectory(style)
