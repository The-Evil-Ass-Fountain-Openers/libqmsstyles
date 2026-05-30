set(qmsstyles_FILES
    qmsstyles_global.h
    qmsstyles.h qmsstyles.cpp
)

set(visualstyle_FILES
    style/style.h style/style.cpp
    style/class.h style/class.cpp
    style/part.h style/part.cpp
    style/state.h style/state.cpp
    style/property.h style/property.cpp

    style/definitions.h
    style/visualpartsmap.h style/visualstatesmap.h style/visualenumsmap.h
)

# library
add_library(qmsstyles SHARED
    ${qmsstyles_FILES}
    ${visualstyle_FILES}
)
set_target_properties(qmsstyles PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})
add_definitions(-DHAVE_DIRENT_H=1)
add_definitions(-D_GNU_SOURCE=1)
target_compile_definitions(qmsstyles PRIVATE QMSSTYLES_LIBRARY)
target_link_libraries(qmsstyles
    PRIVATE
        Qt${QT_MAJOR_VERSION}::Core
        Qt${QT_MAJOR_VERSION}::Widgets
        Qt${QT_MAJOR_VERSION}::Quick
        wres
)

if(BUILD_TOOL)
    add_subdirectory(tool)
endif()

# installation
install(
    FILES
        qmsstyles.h
        qmsstyles_global.h
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/qmsstyles
)
install(
    FILES
        style/style.h
        style/class.h
        style/part.h
        style/state.h
        style/property.h

        style/definitions.h
        style/visualenumsmap.h
        style/visualstatesmap.h
        style/visualpartsmap.h
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/qmsstyles/style/
)

# cmake package config
install(
    TARGETS
        qmsstyles
    EXPORT
        Qmsstyles-targets
    LIBRARY DESTINATION
        ${CMAKE_INSTALL_LIBDIR}
)

install(
    EXPORT
        Qmsstyles-targets
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/Qmsstyles
)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/QmsstylesConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/QmsstylesConfig.cmake"

    INSTALL_DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/Qmsstyles
)
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/QmsstylesConfig.cmake"
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/Qmsstyles
)
