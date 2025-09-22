QT += core widgets

CONFIG += c++20
CONFIG += console

TARGET = test_addurl_simple
TEMPLATE = app

SOURCES += test_addurl_simple.cpp

# Compiler flags
QMAKE_CXXFLAGS += -Wall -Wextra

# Debug/Release configuration
CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += -g -DDEBUG
    TARGET = $$TARGET"_debug"
} else {
    QMAKE_CXXFLAGS += -O2 -DNDEBUG
}

# Application information
VERSION = 1.0.0
QMAKE_TARGET_PRODUCT = "LDM AddURL Test"
QMAKE_TARGET_DESCRIPTION = "Test application for AddURL functionality with automatic filename extraction"
QMAKE_TARGET_COPYRIGHT = "LDM Project 2024"

# Output directory
DESTDIR = .

# Clean target
QMAKE_CLEAN += $$TARGET

# Install target
target.path = /usr/local/bin
INSTALLS += target
