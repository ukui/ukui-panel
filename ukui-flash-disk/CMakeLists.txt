cmake_minimum_required(VERSION 3.1.0)

project(ukui-flash-disk)

#判断编译器类型,如果是gcc编译器,则在编译选项中加入c++11支持
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    message(STATUS "optional:-std=c++11")
endif(CMAKE_COMPILER_IS_GNUCXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets Network REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(KF5WindowSystem REQUIRED)

pkg_check_modules(GLIB2 REQUIRED glib-2.0 gio-2.0 udisks2)
pkg_check_modules(QGS REQUIRED gsettings-qt)


include_directories(${GLIB2_INCLUDE_DIRS})
include_directories(${QGS_INCLUDE_DIRS})
add_executable(ukui-flash-disk
    ukui-flash-disk.qrc
    disk-resources/ukui-flash-disk_zh_CN.ts
    disk-resources/ukui-flash-disk_tr.ts
    main.cpp
    mainwindow.cpp
    qclickwidget.cpp
    qclickwidget.h
    UnionVariable.cpp
    UnionVariable.h
    ejectInterface.cpp
    ejectInterface.h
    clickLabel.h
    device-operation.h
    device-operation.cpp
    clickLabel.cpp
    device-manager.h
    device-manager.cpp
    repair-dialog-box.h
    repair-dialog-box.cpp
    MainController.h
    MainController.cpp
    MacroFile.h
    interactivedialog.cpp
    interactivedialog.h
    gpartedinterface.cpp
    gpartedinterface.h
        mainwindow.ui
        mainwindow.h
    fdapplication.cpp
    fdapplication.h
    fdframe.cpp
    fdframe.h
    flashdiskdata.cpp
    flashdiskdata.h
    fdclickwidget.cpp
    fdclickwidget.h
    QtSingleApplication/qtsingleapplication.h
    QtSingleApplication/qtsingleapplication.cpp
    QtSingleApplication/qtlocalpeer.h
    QtSingleApplication/qtlocalpeer.cpp
    datacdrom.h
    datacdrom.cpp
)

add_executable(test-repair-dialog
    device-manager.h
    device-manager.cpp
    repair-dialog-box.h
    repair-dialog-box.cpp
    device-operation.h
    device-operation.cpp
    test-repair-dialog.cpp
)
target_link_libraries(test-repair-dialog  Qt5::Widgets Qt5::DBus ${GLIB2_LIBRARIES} ${QGS_LIBRARIES} KF5::WindowSystem)

add_definitions(-DQT_NO_KEYWORDS)
add_definitions(-DQT_MESSAGELOGCONTEXT)

target_link_libraries(${PROJECT_NAME}  Qt5::Widgets Qt5::DBus Qt5::Network ${GLIB2_LIBRARIES} ${QGS_LIBRARIES} KF5::WindowSystem -lukui-log4qt)

install(TARGETS ukui-flash-disk DESTINATION bin)

#install(FILES
#        disk-resources/org.ukui.ukui-flash-disks-pkexec.policy
#        DESTINATION "/usr/share/polkit-1/actions/"
#        COMPONENT Runtime
#)

#install(FILES
#        disk-resources/ukui-flash-disks-pkexec.sh
#        DESTINATION "/usr/bin/"
#        COMPONENT Runtime
#        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE
#)

install(FILES
    disk-resources/ukui-flash-disk.desktop
    DESTINATION "/etc/xdg/autostart/"
    COMPONENT Runtime
)

install(FILES
    ukui-flash-disk.qrc disk-resources/ukui-flash-disk_zh_CN.qm disk-resources/ukui-flash-disk_tr.qm
    DESTINATION "/usr/share/ukui/ukui-panel/"
    COMPONENT Runtime
)

install(FILES
    disk-resources/org.ukui.flash-disk.autoload.gschema.xml
    DESTINATION "/usr/share/glib-2.0/schemas"
    COMPONENT Runtime
)
