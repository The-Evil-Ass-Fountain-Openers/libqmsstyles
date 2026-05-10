qt_add_qml_module(qmsstylesvsplugin
    URI qmsstyles.style
    VERSION 1.0
    SHARED
    PLUGIN_TARGET qmsstylesvsplugin
    CLASS_NAME QmsstylesVSPlugin
    NAMESPACE VisualStyle
    SOURCES qmsstylesvswrapper.cpp ../../style/style.h ../../style/class.h
)

target_link_libraries(qmsstylesvsplugin
    PRIVATE
        Qt6::Qml
        qmsstyles
)

install(TARGETS qmsstylesvsplugin DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt6/qml/qmsstyles/style)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qmldir DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt6/qml/qmsstyles/style)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qmsstylesvsplugin.qmltypes DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt6/qml/qmsstyles/style)
