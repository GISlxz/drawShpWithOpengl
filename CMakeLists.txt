cmake_minimum_required(VERSION 3.5)

project(YuiGIS VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        gisview.h
        gisview.cpp
        crsfile.h
        crsfile.cpp
        crsfilemodel.h
        crsfilemodel.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(YuiGIS
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        RES.qrc
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET YuiGIS APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(YuiGIS SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(YuiGIS
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(YuiGIS PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(YuiGIS PRIVATE Qt6::OpenGLWidgets)

# 包含头文件目录
target_include_directories(YuiGIS PUBLIC "D:\\coding\\libs\\poly2tri-master\\poly2tri")
# 链接第三方库
target_link_libraries(YuiGIS PUBLIC "D:\\coding\\libs\\poly2tri-master\\build\\poly2tri.lib")

# 包含头文件目录
target_include_directories(YuiGIS PRIVATE "D:\\coding\\gdal_compile\\GDAL_353_DEBUG\\include")
# 链接第三方库
target_link_libraries(YuiGIS PRIVATE "D:\\coding\\gdal_compile\\GDAL_353_DEBUG\\lib\\gdal.lib")

set_target_properties(YuiGIS PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS YuiGIS
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(YuiGIS)
endif()
