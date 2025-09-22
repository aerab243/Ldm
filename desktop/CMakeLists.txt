cmake_minimum_required(VERSION 3.16)

# Desktop application
project(ldm-desktop VERSION 1.0.0 LANGUAGES CXX)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network Sql Multimedia MultimediaWidgets Charts LinguistTools Concurrent HttpServer Test)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBCURL REQUIRED libcurl)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)
pkg_check_modules(FFMPEG REQUIRED libavcodec libavformat libavutil libswscale)
pkg_check_modules(OPENSSL REQUIRED openssl)
# pkg_check_modules(CLAMAV REQUIRED libclamav)

# Include directories
include_directories(${LIBCURL_INCLUDE_DIRS})
include_directories(${SQLITE3_INCLUDE_DIRS})
include_directories(${FFMPEG_INCLUDE_DIRS})
include_directories(${OPENSSL_INCLUDE_DIRS})
# include_directories(${CLAMAV_INCLUDE_DIRS})

# Source files
set(SOURCES
    src/main.cpp
    src/core/DownloadItem.cpp
    src/core/NetworkManager.cpp
    src/core/Database.cpp
    src/core/Category.cpp
    src/core/Settings.cpp
    src/core/DownloadEngine.cpp
    src/core/SegmentManager.cpp
    src/core/SpeedCalculator.cpp
    src/core/Scheduler.cpp
    src/api/ApiServer.cpp
    src/ui/MainWindow.cpp
    src/ui/DownloadListWidget.cpp
    src/ui/AddUrlDialog.cpp
    src/ui/ProgressWidget.cpp
    src/ui/StatisticsWidget.cpp
    src/ui/CategoryManager.cpp
    src/ui/SettingsDialog.cpp
    src/ui/ThemeManager.cpp
    src/ui/MediaPreviewWidget.cpp
    src/utils/Logger.cpp
    src/utils/MemoryMappedFile.cpp
    src/utils/MetadataCache.cpp
    src/core/DownloadItem.h
    src/core/NetworkManager.h
    src/core/Database.h
    src/core/Category.h
    src/core/Settings.h
    src/core/DownloadEngine.h
    src/core/SegmentManager.h
    src/core/SpeedCalculator.h
    src/core/Scheduler.h
    src/ui/MainWindow.h
    src/ui/DownloadListWidget.h
    src/ui/AddUrlDialog.h
    src/ui/ProgressWidget.h
    src/ui/StatisticsWidget.h
    src/ui/CategoryManager.h
    src/ui/SettingsDialog.h
    src/ui/ThemeManager.h
    src/ui/MediaPreviewWidget.h
    src/utils/Logger.h
)

set(HEADERS
    src/core/DownloadItem.h
    src/core/NetworkManager.h
    src/core/Database.h
    src/core/Category.h
    src/core/Settings.h
    src/core/DownloadEngine.h
    src/core/SegmentManager.h
    src/core/SpeedCalculator.h
    src/core/Scheduler.h
    src/ui/MainWindow.h
    src/ui/DownloadListWidget.h
    src/ui/AddUrlDialog.h
    src/ui/ProgressWidget.h
    src/ui/StatisticsWidget.h
    src/ui/CategoryManager.h
    src/ui/SettingsDialog.h
    src/ui/ThemeManager.h
    src/ui/MediaPreviewWidget.h
    src/utils/Logger.h
)

set(UI_FILES
)

set(RESOURCE_FILES
    resources/resources.qrc
)

# Translations
set(TS_FILES
    resources/translations/ldm_fr.ts
    resources/translations/ldm_es.ts
    resources/translations/ldm_de.ts
    resources/translations/ldm_zh.ts
)

qt6_add_translation(QM_FILES ${TS_FILES})

# Add Qt resources
qt6_add_resources(RESOURCE_FILES resources/resources.qrc)

# Create executable
add_executable(ldm-desktop ${SOURCES} ${HEADERS} ${UI_FILES} ${RESOURCE_FILES} ${QM_FILES})

# Link libraries
target_link_libraries(ldm-desktop
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::Sql
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    Qt6::Charts
    Qt6::Concurrent
    Qt6::HttpServer
    ${LIBCURL_LIBRARIES}
    ${SQLITE3_LIBRARIES}
    ${FFMPEG_LIBRARIES}
    ${OPENSSL_LIBRARIES}
)
#     ${CLAMAV_LIBRARIES}
#     7zip
#     protobuf
#     webrtc

# Include current directory for headers
target_include_directories(ldm-desktop PRIVATE src)

# Install
install(TARGETS ldm-desktop
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# CLI executable
add_executable(ldm-cli
    src/cli/main.cpp
    src/core/DownloadItem.cpp
    src/core/NetworkManager.cpp
    src/core/Database.cpp
    src/core/Category.cpp
    src/core/Settings.cpp
    src/core/DownloadEngine.cpp
    src/core/SegmentManager.cpp
    src/core/SpeedCalculator.cpp
    src/core/Scheduler.cpp
    src/api/ApiServer.cpp
    src/utils/Logger.cpp
    src/utils/MemoryMappedFile.cpp
    src/utils/MetadataCache.cpp
)
target_link_libraries(ldm-cli
    Qt6::Core
    Qt6::Network
    Qt6::Sql
    Qt6::HttpServer
    Qt6::Concurrent
    ${LIBCURL_LIBRARIES}
    ${SQLITE3_LIBRARIES}
    ${FFMPEG_LIBRARIES}
    ${OPENSSL_LIBRARIES}
)
target_include_directories(ldm-cli PRIVATE src)

# Tests
enable_testing()
add_subdirectory(tests)
