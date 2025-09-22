#ifndef TESTBASICDOWNLOAD_H
#define TESTBASICDOWNLOAD_H

#include <QObject>
#include <QtTest>

class TestBasicDownload : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testBasicDownloadFlow();
    void testDownloadWithProgress();
    void testDownloadCompletion();
    void testDownloadErrorHandling();
};

#endif // TESTBASICDOWNLOAD_H