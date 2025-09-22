cmake_minimum_required(VERSION 3.16)

project(LDM VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network Sql Multimedia Charts LinguistTools)

# Enable Qt6 automoc, autorcc, autouic
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Add subdirectories
add_subdirectory(desktop)
add_subdirectory(native-messaging/host)

# Install targets
include(GNUInstallDirs)
