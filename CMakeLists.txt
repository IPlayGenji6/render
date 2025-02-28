cmake_minimum_required(VERSION 3.15)

project(Renderer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Renderer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        image.h image.cpp
        model.h model.cpp
        gl.h gl.cpp
    )

else()
    if(ANDROID)
        add_library(Renderer SHARED
            ${PROJECT_SOURCES}
        )

    else()
        add_executable(Renderer
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Renderer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

target_include_directories(Renderer PRIVATE
"${CMAKE_SOURCE_DIR}"
"${CMAKE_SOURCE_DIR}/include"
)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Renderer)
endif()
set_target_properties(Renderer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Renderer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Renderer)
endif()
