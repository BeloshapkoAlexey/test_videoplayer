cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

project( VideoPlayer )

find_package( OpenCV REQUIRED )

set(CMAKE_AUTOMOC ON)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

FILE(GLOB VPh *.h)
FILE(GLOB VPcpp *.cpp)

add_executable( Videoplayer
${VPh}
${VPcpp}
)
target_link_libraries( Videoplayer ${OpenCV_LIBS})
target_link_libraries( Videoplayer ${QT_QTGUI_LIBRARIES})
qt5_use_modules(Videoplayer Core)
qt5_use_modules(Videoplayer Gui)
qt5_use_modules(Videoplayer Widgets)