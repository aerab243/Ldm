# Tests CMakeLists.txt
set(TEST_SOURCES
    test-core/TestDownloadItem.cpp
    test-core/TestNetworkManager.cpp
    test-api/TestApiServer.cpp
    test-ui/TestBasicDownload.cpp
    main.cpp
    ../src/core/DownloadItem.cpp
    ../src/core/NetworkManager.cpp
    ../src/core/Database.cpp
    ../src/api/ApiServer.cpp
    ../src/core/DownloadEngine.cpp
    ../src/core/SegmentManager.cpp
    ../src/core/SpeedCalculator.cpp
    ../src/core/Scheduler.cpp
    ../src/utils/Logger.cpp
    ../src/utils/MemoryMappedFile.cpp
    ../src/utils/MetadataCache.cpp
)

set(TEST_HEADERS
    test-core/TestDownloadItem.h
    test-core/TestNetworkManager.h
    test-api/TestApiServer.h
    test-ui/TestBasicDownload.h
)

# Create test executable
add_executable(ldm-tests ${TEST_SOURCES} ${TEST_HEADERS})

# Link libraries
target_link_libraries(ldm-tests
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::Sql
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    Qt6::Charts
    Qt6::Concurrent
    Qt6::HttpServer
    Qt6::Test
    ${LIBCURL_LIBRARIES}
    ${SQLITE3_LIBRARIES}
    ${FFMPEG_LIBRARIES}
    ${OPENSSL_LIBRARIES}
)

# Include directories
target_include_directories(ldm-tests PRIVATE ../src)

# Add test
add_test(NAME ldm-tests COMMAND ldm-tests)
