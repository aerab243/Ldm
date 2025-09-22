#include <QApplication>
#include <QtTest>

// Include test headers
#include "test-core/TestDownloadItem.h"
#include "test-core/TestNetworkManager.h"
#include "test-ui/TestBasicDownload.h"
#include "test-api/TestApiServer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    int status = 0;

    // Run core tests
    TestDownloadItem testDownloadItem;
    status |= QTest::qExec(&testDownloadItem, argc, argv);

    TestNetworkManager testNetworkManager;
    status |= QTest::qExec(&testNetworkManager, argc, argv);

    // Run UI tests
    TestBasicDownload testBasicDownload;
    status |= QTest::qExec(&testBasicDownload, argc, argv);

    // Run API tests
    TestApiServer testApiServer;
    status |= QTest::qExec(&testApiServer, argc, argv);

    return status;
}