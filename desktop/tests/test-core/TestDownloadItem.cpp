#include "TestDownloadItem.h"
#include "../../src/core/DownloadItem.h"

void TestDownloadItem::initTestCase()
{
    // Initialize test case
}

void TestDownloadItem::cleanupTestCase()
{
    // Cleanup test case
}

void TestDownloadItem::testCreation()
{
    DownloadItem item(1, "http://example.com/file.zip", "file.zip");
    QCOMPARE(item.getId(), 1);
    QCOMPARE(item.getUrl(), QString("http://example.com/file.zip"));
    QCOMPARE(item.getFilename(), QString("file.zip"));
    QCOMPARE(item.getStatus(), QString("queued"));
}

void TestDownloadItem::testSettersAndGetters()
{
    DownloadItem item;
    item.setUrl("http://test.com/test.txt");
    item.setFilename("test.txt");
    item.setFilepath("/tmp/test.txt");
    item.setTotalSize(1024);
    item.setDownloadedSize(512);
    item.setSpeed(100);
    item.setEta(10);

    QCOMPARE(item.getUrl(), QString("http://test.com/test.txt"));
    QCOMPARE(item.getFilename(), QString("test.txt"));
    QCOMPARE(item.getFilepath(), QString("/tmp/test.txt"));
    QCOMPARE(item.getTotalSize(), 1024LL);
    QCOMPARE(item.getDownloadedSize(), 512LL);
    QCOMPARE(item.getSpeed(), 100);
    QCOMPARE(item.getEta(), 10);
}

void TestDownloadItem::testStatusTransitions()
{
    DownloadItem item;
    QCOMPARE(item.getStatus(), QString("queued"));

    item.setStatus("downloading");
    QCOMPARE(item.getStatus(), QString("downloading"));

    item.setStatus("completed");
    QCOMPARE(item.getStatus(), QString("completed"));
}

void TestDownloadItem::testValidation()
{
    DownloadItem item;
    // Test setting URL
    item.setUrl("http://valid.com/file");
    QCOMPARE(item.getUrl(), QString("http://valid.com/file"));
}

void TestDownloadItem::testProgressCalculation()
{
    DownloadItem item;
    item.setTotalSize(1000);
    item.setDownloadedSize(500);
    QCOMPARE(item.getProgress(), 0.5);

    item.setDownloadedSize(1000);
    QCOMPARE(item.getProgress(), 1.0);
}