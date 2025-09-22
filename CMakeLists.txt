cmake_minimum_required(VERSION 3.16)

project(LDM VERSION 1.0.0 LANGUAGES CXX)
set(PROJECT_DESCRIPTION "Linux Download Manager - A modern download manager for Linux")
set(PROJECT_HOMEPAGE "https://github.com/aerab243/ldm")
set(PROJECT_CONTACT "Anna-el Gerard RABENANDRASANA <aerabenandrasana@gmail.com>")

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

# CPack configuration for packaging
include(CPack)

# Set package information
set(CPACK_PACKAGE_NAME "LDM")
set(CPACK_PACKAGE_VENDOR "Anna-el Gerard RABENANDRASANA")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Linux Download Manager - A modern download manager for Linux")
set(CPACK_PACKAGE_DESCRIPTION "Linux Download Manager (LDM) is a modern, feature-rich download manager built with Qt6 and C++20. It supports multiple protocols, pause/resume functionality, browser integration, and advanced download management features.")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "LDM")
set(CPACK_PACKAGE_EXECUTABLES "ldm;LDM")

# Resource files
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

# Platform-specific configurations
if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_DISPLAY_NAME "LDM - Linux Download Manager")
    set(CPACK_NSIS_PACKAGE_NAME "LDM")
    set(CPACK_NSIS_CONTACT "aerabenandrasana@gmail.com")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/aerab243/ldm")
    set(CPACK_NSIS_HELP_LINK "https://github.com/aerab243/ldm/wiki")
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_CREATE_ICONS_EXTRA
        "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\LDM.lnk' '$INSTDIR\\\\bin\\\\ldm.exe'"
    )
    set(CPACK_NSIS_DELETE_ICONS_EXTRA
        "Delete '$SMPROGRAMS\\\\$START_MENU\\\\LDM.lnk'"
    )
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop;TGZ")
    set(CPACK_DMG_VOLUME_NAME "LDM")
    set(CPACK_DMG_FORMAT "UDZO")
else()
    # Linux
    set(CPACK_GENERATOR "DEB;RPM;TGZ")

    # DEB package configuration
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Anna-el Gerard RABENANDRASANA <aerabenandrasana@gmail.com>")
    set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/aerab243/ldm")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6core6, libqt6widgets6, libqt6network6, libqt6sql6, libqt6multimedia6, libqt6charts6, libcurl4, libssl3, libsqlite3-0, libprotobuf23")
    set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "ffmpeg, p7zip-full, clamav")
    set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/installer/debian/postinst;${CMAKE_CURRENT_SOURCE_DIR}/installer/debian/prerm")

    # RPM package configuration
    set(CPACK_RPM_PACKAGE_SUMMARY "Linux Download Manager - Modern download manager for Linux")
    set(CPACK_RPM_PACKAGE_GROUP "Applications/Internet")
    set(CPACK_RPM_PACKAGE_LICENSE "GPL")
    set(CPACK_RPM_PACKAGE_URL "https://github.com/aerab243/ldm")
    set(CPACK_RPM_PACKAGE_REQUIRES "qt6-qtbase >= 6.5, qt6-qtmultimedia >= 6.5, qt6-qtcharts >= 6.5, libcurl >= 7.0, openssl >= 1.1, sqlite >= 3.0, protobuf >= 3.0")
    set(CPACK_RPM_PACKAGE_SUGGESTS "ffmpeg, p7zip, clamav")
    set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/installer/rpm/postinstall.sh")
    set(CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/installer/rpm/preuninstall.sh")
endif()

# Component-based packaging
set(CPACK_COMPONENTS_ALL applications libraries headers)
set(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME "Linux Download Manager")
set(CPACK_COMPONENT_APPLICATIONS_DESCRIPTION "Main Linux Download Manager application and browser integration")
set(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "Runtime Libraries")
set(CPACK_COMPONENT_LIBRARIES_DESCRIPTION "Required runtime libraries for Linux Download Manager")
set(CPACK_COMPONENT_HEADERS_DISPLAY_NAME "Development Headers")
set(CPACK_COMPONENT_HEADERS_DESCRIPTION "Development headers for Linux Download Manager plugins")

# Archive configuration
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
