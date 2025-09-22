QT += core gui widgets network sql multimedia charts concurrent
CONFIG += c++20
TARGET = ldm-desktop
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/Application.cpp \
    src/core/DownloadItem.cpp \
    src/core/NetworkManager.cpp \
    src/core/Database.cpp \
    src/core/Category.cpp \
    src/core/Settings.cpp \
    src/core/DownloadEngine.cpp \
    src/core/SegmentManager.cpp \
    src/core/ProxyManager.cpp \
    src/core/SpeedCalculator.cpp \
    src/ui/MainWindow.cpp \
    src/ui/DownloadListWidget.cpp \
    src/ui/AddUrlDialog.cpp \
    src/ui/ProgressWidget.cpp \
    src/ui/StatisticsWidget.cpp \
    src/ui/SettingsDialog.cpp \
    src/ui/CategoryManager.cpp \
    src/ui/ThemeManager.cpp \
    src/ui/ScheduleDialog.cpp \
    src/utils/Logger.cpp \
    src/utils/Encryption.cpp \
    src/utils/FileUtils.cpp \
    src/utils/NetworkUtils.cpp \
    src/utils/SystemUtils.cpp \
    src/utils/JsonConfig.cpp \
    src/utils/ThreadPool.cpp \
    src/utils/FormatConverter.cpp \
    src/plugins/PluginManager.cpp \
    src/integrations/BrowserIntegration.cpp \
    src/integrations/ChromeIntegration.cpp \
    src/integrations/FirefoxIntegration.cpp \
    src/integrations/EdgeIntegration.cpp \
    src/api/ApiServer.cpp \
    src/cli/main_cli.cpp

HEADERS += \
    src/Application.h \
    src/core/DownloadItem.h \
    src/core/NetworkManager.h \
    src/core/Database.h \
    src/core/Category.h \
    src/core/Settings.h \
    src/core/DownloadEngine.h \
    src/core/SegmentManager.h \
    src/core/ProxyManager.h \
    src/core/SpeedCalculator.h \
    src/ui/MainWindow.h \
    src/ui/DownloadListWidget.h \
    src/ui/AddUrlDialog.h \
    src/ui/ProgressWidget.h \
    src/ui/StatisticsWidget.h \
    src/ui/SettingsDialog.h \
    src/ui/CategoryManager.h \
    src/ui/ThemeManager.h \
    src/ui/ScheduleDialog.h \
    src/utils/Logger.h \
    src/utils/Encryption.h \
    src/utils/FileUtils.h \
    src/utils/NetworkUtils.h \
    src/utils/SystemUtils.h \
    src/utils/JsonConfig.h \
    src/utils/ThreadPool.h \
    src/utils/FormatConverter.h \
    src/plugins/PluginManager.h \
    src/plugins/PluginInterface.h \
    src/integrations/BrowserIntegration.h \
    src/integrations/ChromeIntegration.h \
    src/integrations/FirefoxIntegration.h \
    src/integrations/EdgeIntegration.h \
    src/api/ApiServer.h \
    src/cli/main_cli.h

FORMS += \
    src/ui/MainWindow.ui \
    src/ui/AddUrlDialog.ui \
    src/ui/SettingsDialog.ui \
    src/ui/CategoryManager.ui \
    src/ui/ScheduleDialog.ui

RESOURCES += \
    resources/resources.qrc

TRANSLATIONS += \
    resources/translations/ldm_en.ts \
    resources/translations/ldm_fr.ts \
    resources/translations/ldm_es.ts \
    resources/translations/ldm_de.ts \
    resources/translations/ldm_zh.ts

# External libraries (assuming installed)
LIBS += -lcurl -lsqlite3 -lavcodec -lavformat -lavutil -lswscale -lssl -lcrypto -lclamav -l7zip -lprotobuf -lwebrtc

INCLUDEPATH += /usr/include /usr/local/include
