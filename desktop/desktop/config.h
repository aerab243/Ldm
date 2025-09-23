#ifndef CONFIG_H
#define CONFIG_H

/* Project information */
#define PROJECT_NAME "Linux Download Manager"
#define PROJECT_VERSION "1.0.0"
#define PROJECT_VERSION_MAJOR 1
#define PROJECT_VERSION_MINOR 0
#define PROJECT_VERSION_PATCH 0
#define PROJECT_DESCRIPTION ""
#define PROJECT_HOMEPAGE "https://github.com/aerab243/ldm"
#define PROJECT_CONTACT "Anna-el Gerard RABENANDRASANA <aerabenandrasana@gmail.com>"

/* Build configuration */
#define CMAKE_BUILD_TYPE "Debug"
#define CMAKE_SYSTEM_NAME "Linux"
#define CMAKE_SYSTEM_VERSION "6.17.0-0.rc6.49.fc43.x86_64"
#define CMAKE_SYSTEM_PROCESSOR "x86_64"

/* Qt configuration */
#define QT_VERSION_STR "6.9.2"
#define HAVE_QT6_CHARTS

/* Feature availability */
/* #undef HAVE_LIBCURL */
/* #undef HAVE_SQLITE3 */
/* #undef HAVE_FFMPEG */
/* #undef HAVE_OPENSSL */
/* #undef HAVE_CLAMAV */
/* #undef HAVE_PROTOBUF */
/* #undef HAVE_7ZIP */
/* #undef HAVE_WEBRTC */

/* Optional features */
/* #undef ENABLE_BROWSER_INTEGRATION */
/* #undef ENABLE_NATIVE_MESSAGING */
/* #undef ENABLE_STATISTICS */
/* #undef ENABLE_ADVANCED_THEMES */
/* #undef ENABLE_MEDIA_PREVIEW */
/* #undef ENABLE_VIRUS_SCANNING */
/* #undef ENABLE_COMPRESSION */
/* #undef ENABLE_VIDEO_CONVERSION */

/* Debug features */
/* #undef ENABLE_DEBUG_LOGGING */
/* #undef ENABLE_PERFORMANCE_MONITORING */
/* #undef ENABLE_MEMORY_DEBUGGING */

/* Installation paths */
#define INSTALL_PREFIX "/usr/local"
#define INSTALL_BINDIR "bin"
#define INSTALL_LIBDIR "lib64"
#define INSTALL_DATADIR ""

/* Resource paths */
#define RESOURCE_DIR "/usr/local//ldm"
#define TRANSLATION_DIR "/usr/local//ldm/translations"
#define THEME_DIR "/usr/local//ldm/themes"
#define PLUGIN_DIR "/usr/local/lib64/ldm/plugins"

/* Default configuration */
#define DEFAULT_DOWNLOAD_DIR "/usr/local/Downloads"
#define DEFAULT_DATABASE_NAME "ldm.db"
#define DEFAULT_CONFIG_FILE "ldm.conf"

/* Network configuration */
#define DEFAULT_MAX_CONNECTIONS 8
#define DEFAULT_MAX_SEGMENTS 4
#define DEFAULT_TIMEOUT_SECONDS 30
#define DEFAULT_RETRY_COUNT 3

/* UI configuration */
#define DEFAULT_WINDOW_WIDTH 1024
#define DEFAULT_WINDOW_HEIGHT 768
#define DEFAULT_THEME "default"
#define DEFAULT_LANGUAGE "en"

#endif // CONFIG_H
