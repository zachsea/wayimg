cmake_minimum_required(VERSION 3.20)
project(wayimg LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp src/application.cpp
                               src/image_window.cpp src/window.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(FILES res/cafe.zach.${PROJECT_NAME}.desktop
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)

install(FILES res/cafe.zach.${PROJECT_NAME}.svg
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps)

install(FILES res/cafe.zach.${PROJECT_NAME}.metainfo.xml
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo)
