#ifndef TESTDOWNLOADITEM_H
#define TESTDOWNLOADITEM_H

#include <QObject>
#include <QtTest>

class TestDownloadItem : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testCreation();
    void testSettersAndGetters();
    void testStatusTransitions();
    void testValidation();
    void testProgressCalculation();
};

#endif // TESTDOWNLOADITEM_H