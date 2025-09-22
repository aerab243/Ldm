cmake_minimum_required(VERSION 3.16)

project(ldm-native-host VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Network)

# Enable Qt6 automoc
set(CMAKE_AUTOMOC ON)

# Source files
set(SOURCES
    NativeHost.cpp
    MessageParser.cpp
)

set(HEADERS
    NativeHost.h
    MessageParser.h
)

# Create executable
add_executable(ldm-native-host ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(ldm-native-host
    Qt6::Core
    Qt6::Network
)

# Install
install(TARGETS ldm-native-host
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)