#ifndef TESTPERFORMANCE_H
#define TESTPERFORMANCE_H

#include <QObject>
#include <QtTest>

class TestPerformance : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testDownloadSpeed();
    void testMemoryUsage();
    void testConcurrentDownloads();
    void testLargeFileHandling();
};

#endif // TESTPERFORMANCE_H