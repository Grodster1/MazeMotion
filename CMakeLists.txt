cmake_minimum_required(VERSION 3.19)
project(MazeMotion LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets SerialPort Charts)

qt_standard_project_setup()


qt_add_executable(MazeMotion
    WIN32 MACOSX_BUNDLE
    main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    src/gamewidget.h
    src/gamewidget.cpp
    src/sensorreader.h
    src/sensorreader.cpp
    src/chartpanel.h src/chartpanel.cpp
)

target_include_directories(MazeMotion PRIVATE src)

target_link_libraries(MazeMotion
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::SerialPort
        Qt6::Charts
)

install(TARGETS MazeMotion
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET MazeMotion
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
