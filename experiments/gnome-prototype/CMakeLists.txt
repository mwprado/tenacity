cmake_minimum_required(VERSION 3.20)

project(TenacityGnomePrototype LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(LIBADWAITA REQUIRED IMPORTED_TARGET libadwaita-1)


add_executable(tenacity-gnome-prototype
   TenacityGnomePrototype.cpp
   PrototypeModel.cpp
   PrototypeModel.h
)


target_link_libraries(tenacity-gnome-prototype
   PRIVATE
      PkgConfig::GTK4
      PkgConfig::LIBADWAITA
      m
)
