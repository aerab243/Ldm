#include "TestBasicDownload.h"
#include "../../src/core/DownloadItem.h"
#include "../../src/core/NetworkManager.h"
#include "../../src/ui/MainWindow.h"

void TestBasicDownload::initTestCase()
{
    // Initialize test case - setup any required resources
}

void TestBasicDownload::cleanupTestCase()
{
    // Cleanup test case - remove temporary files, etc.
}

void TestBasicDownload::testBasicDownloadFlow()
{
    // Test the complete download flow from start to finish
    DownloadItem item(1, "http://httpbin.org/uuid", "test.txt");
    NetworkManager manager;
    
    // Start download
    item.setStatus("downloading");
    QCOMPARE(item.getStatus(), QString("downloading"));
    
    // Simulate progress
    item.setDownloadedSize(50);
    item.setTotalSize(100);
    QCOMPARE(item.getProgress(), 0.5);
    
    // Complete download
    item.setDownloadedSize(100);
    item.setStatus("completed");
    QCOMPARE(item.getStatus(), QString("completed"));
}

void TestBasicDownload::testDownloadWithProgress()
{
    DownloadItem item;
    item.setTotalSize(1000);
    
    // Test progress updates
    item.setDownloadedSize(250);
    QCOMPARE(item.getProgress(), 0.25);
    
    item.setDownloadedSize(750);
    QCOMPARE(item.getProgress(), 0.75);
}

void TestBasicDownload::testDownloadCompletion()
{
    DownloadItem item;
    item.setUrl("http://example.com/file.zip");
    item.setStatus("downloading");
    
    // Simulate completion
    item.setDownloadedSize(1024);
    item.setTotalSize(1024);
    item.setStatus("completed");
    
    QCOMPARE(item.getStatus(), QString("completed"));
    QCOMPARE(item.getProgress(), 1.0);
}

void TestBasicDownload::testDownloadErrorHandling()
{
    DownloadItem item;
    item.setUrl("http://invalid.url");
    item.setStatus("downloading");
    
    // Simulate error
    item.setStatus("failed");
    item.setErrorMessage("Network error");
    
    QCOMPARE(item.getStatus(), QString("failed"));
    QVERIFY(!item.getErrorMessage().isEmpty());
}