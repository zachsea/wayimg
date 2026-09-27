cmake_minimum_required(VERSION 3.20)
project(sdl3project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
  src/main.cpp
  src/application.cpp
  src/image_window.cpp
)
target_link_libraries(
  ${PROJECT_NAME} PRIVATE
  SDL3::SDL3
  SDL3_image::SDL3_image
)

