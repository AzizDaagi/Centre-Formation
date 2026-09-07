cmake_minimum_required(VERSION 3.16)

project(centre_formation VERSION 0.1 LANGUAGES CXX)

configure_file(settings.ini "${CMAKE_BINARY_DIR}/settings.ini" COPYONLY)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql Charts PrintSupport Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Charts PrintSupport Network)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(centre_formation
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        db.h db.cpp
        emailnotifier.h emailnotifier.cpp
        ollamaassistant.h ollamaassistant.cpp
        reservation.h reservation.cpp
        salle.h salle.cpp
        formateur.h formateur.cpp
        cours.h cours.cpp
        stagiaire.h stagiaire.cpp
        passwordutils.h passwordutils.cpp
        authentification.h authentification.cpp
        sallewidget.h sallewidget.cpp
        formateurwidget.h formateurwidget.cpp
        courswidget.h courswidget.cpp
        stagiairewidget.h stagiairewidget.cpp
        style.qss
        resources.qrc
        dashboardwidget.h dashboardwidget.cpp
        moduletools.h moduletools.cpp
        roleworkspace.h roleworkspace.cpp
        incidentresolutiondialog.h incidentresolutiondialog.cpp
        userprofiledialog.h userprofiledialog.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET centre_formation APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(centre_formation SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(centre_formation
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(centre_formation PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Charts Qt${QT_VERSION_MAJOR}::PrintSupport Qt${QT_VERSION_MAJOR}::Network)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.centre_formation)
endif()
set_target_properties(centre_formation PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS centre_formation
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(centre_formation)
endif()
