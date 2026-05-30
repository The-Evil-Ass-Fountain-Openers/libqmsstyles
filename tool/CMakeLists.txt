qt_add_executable(qmsstylestool main.cpp)

qt_add_qml_module(qmsstylestool
    URI userinterface
    RESOURCE_PREFIX /qt/qml
    QML_FILES
        Main.qml
    SOURCES
        qmsstylesbackend.cpp
        styletreemodel.cpp styletreeitem.cpp
        propertiesmodel.cpp
        imagefileitem.cpp
    RESOURCES
        res.qrc
)

target_link_libraries(qmsstylestool
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Quick
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets

        qmsstyles
)

install(TARGETS qmsstylestool
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
