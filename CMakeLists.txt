#cmake .. -DCMAKE_PREFIX_PATH=/usr/local/Cellar/qt5/5.3.1/

cmake_minimum_required(VERSION 3.1.0)

project(cmakeTest)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(Qt5Core_DIR "E:/.conan/f5c28a0/1/lib/cmake/Qt5Core")
set(Qt5_DIR "E:/.conan/f5c28a0/1/lib/cmake/Qt5")
set(QT_QMAKE_EXECUTABLE "E:/.conan/f5c28a0/1/bin/qmake.exe")

find_package(Qt5 COMPONENTS Widgets REQUIRED)

include(${CMAKE_CURRENT_SOURCE_DIR}/Src/App/App.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/Lib/Widget/Widget.cmake)
