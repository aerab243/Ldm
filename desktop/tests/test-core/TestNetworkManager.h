#ifndef TESTNETWORKMANAGER_H
#define TESTNETWORKMANAGER_H

#include <QObject>
#include <QtTest>

class TestNetworkManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testDownloadRequest();
    void testProxyConfiguration();
    void testAuthentication();
    void testErrorHandling();
    void testResumeCapability();
};

#endif // TESTNETWORKMANAGER_H