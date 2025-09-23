#include "DownloadTableWidget.h"
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>
#include <QProgressBar>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QItemDelegate>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QDrag>
#include <QFileInfo>
#include <QDateTime>
#include <QSettings>

DownloadTableWidget::DownloadTableWidget(QWidget *parent)
    : QTableWidget(parent)
{
    setupTable();
    setupContextMenu();
    
    // Connect signals directly
    connect(this, &QTableWidget::customContextMenuRequested,
            this, &DownloadTableWidget::showContextMenu);
    connect(this, &QTableWidget::itemDoubleClicked,
            this, &DownloadTableWidget::onItemDoubleClicked);
}

DownloadTableWidget::~DownloadTableWidget()
{
}

void DownloadTableWidget::setupTable()
{
    // Set up columns
    setColumnCount(8);
    QStringList headers = {"#", "File Name", "Size", "Progress", "Speed", "Status", "Time Left", "URL"};
    setHorizontalHeaderLabels(headers);
    
    // Configure table appearance
    setAlternatingRowColors(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSortingEnabled(true);
    
    // Configure header
    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents); // #
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);          // File Name
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents); // Size
    horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);            // Progress
    horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents); // Speed
    horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents); // Status
    horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents); // Time Left
    
    // Set column widths
    setColumnWidth(3, 150); // Progress column
    
    // Hide vertical header
    verticalHeader()->hide();
}

void DownloadTableWidget::setupContextMenu()
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    
    m_contextMenu = new QMenu(this);
    
    m_resumeAction = m_contextMenu->addAction("Resume");
    m_pauseAction = m_contextMenu->addAction("Pause");
    m_stopAction = m_contextMenu->addAction("Stop");
    m_contextMenu->addSeparator();
    m_removeAction = m_contextMenu->addAction("Remove");
    m_contextMenu->addSeparator();
    m_propertiesAction = m_contextMenu->addAction("Properties");
}



int DownloadTableWidget::addDownload(const QString &url, const QString &filename, qint64 totalSize)
{
    int id = rowCount() + 1; // Simple ID generation
    int row = rowCount();
    insertRow(row);
    
    // Add items
    setItem(row, 0, new QTableWidgetItem(QString::number(id)));
    setItem(row, 1, new QTableWidgetItem(filename));
    setItem(row, 2, new QTableWidgetItem(formatFileSize(totalSize)));
    
    // Progress bar
    QProgressBar *progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    setCellWidget(row, 3, progressBar);
    
    setItem(row, 4, new QTableWidgetItem("0 KB/s"));
    setItem(row, 5, new QTableWidgetItem("Queued"));
    setItem(row, 6, new QTableWidgetItem("--"));
    setItem(row, 7, new QTableWidgetItem(url));
    
    // Store download ID
    item(row, 0)->setData(Qt::UserRole, id);
    
    return id;
}

QList<int> DownloadTableWidget::getSelectedDownloadIds() const
{
    QList<int> ids;
    QList<QTableWidgetItem*> items = selectedItems();
    for (QTableWidgetItem* item : items) {
        if (item->column() == 0) { // ID column
            ids.append(item->data(Qt::UserRole).toInt());
        }
    }
    return ids;
}

int DownloadTableWidget::getDownloadCount() const
{
    return rowCount();
}

void DownloadTableWidget::selectCompleted()
{
    QTableWidget::clearSelection();
    for (int row = 0; row < rowCount(); ++row) {
        if (item(row, 5) && item(row, 5)->text() == "Completed") {
            selectRow(row);
        }
    }
}

void DownloadTableWidget::selectUncompleted()
{
    QTableWidget::clearSelection();
    for (int row = 0; row < rowCount(); ++row) {
        if (item(row, 5) && item(row, 5)->text() != "Completed") {
            selectRow(row);
        }
    }
}

void DownloadTableWidget::invertSelection()
{
    for (int row = 0; row < rowCount(); ++row) {
        if (item(row, 0) && item(row, 0)->isSelected()) {
            item(row, 0)->setSelected(false);
        } else {
            selectRow(row);
        }
    }
}

void DownloadTableWidget::removeCompletedDownloads()
{
    for (int row = rowCount() - 1; row >= 0; --row) {
        if (item(row, 5) && item(row, 5)->text() == "Completed") {
            removeRow(row);
        }
    }
}

void DownloadTableWidget::updateDownload(int downloadId, qint64 bytesReceived, qint64 totalSize,
                                        int speed, const QString &status, int timeLeft)
{
    int row = findRowByDownloadId(downloadId);
    if (row == -1) return;
    
    // Calculate progress percentage
    int progress = (totalSize > 0) ? (int)((bytesReceived * 100) / totalSize) : 0;
    
    // Update progress
    QProgressBar *progressBar = qobject_cast<QProgressBar*>(cellWidget(row, 3));
    if (progressBar) {
        progressBar->setValue(progress);
    }
    
    // Update speed
    item(row, 4)->setText(formatSpeed(speed));
    
    // Update status
    item(row, 5)->setText(status);
    
    // Update time left
    if (timeLeft > 0) {
        item(row, 6)->setText(formatTimeRemaining(timeLeft));
    } else {
        item(row, 6)->setText("--");
    }
}

void DownloadTableWidget::removeDownload(int id)
{
    int row = findRowByDownloadId(id);
    if (row != -1) {
        removeRow(row);
    }
}

void DownloadTableWidget::removeAllDownloads()
{
    setRowCount(0);
}

void DownloadTableWidget::sortByColumn(int column, Qt::SortOrder order)
{
    QTableWidget::sortByColumn(column, order);
}

void DownloadTableWidget::filterByCategory(const QString &category)
{
    // Simple implementation - show/hide rows based on category
    // In a real implementation, you'd filter based on download categories
    Q_UNUSED(category)
    
    for (int row = 0; row < rowCount(); ++row) {
        setRowHidden(row, false); // Show all for now
    }
}

int DownloadTableWidget::findRowByDownloadId(int downloadId) const
{
    for (int row = 0; row < rowCount(); ++row) {
        if (item(row, 0) && item(row, 0)->data(Qt::UserRole).toInt() == downloadId) {
            return row;
        }
    }
    return -1;
}

QString DownloadTableWidget::formatFileSize(qint64 bytes) const
{
    if (bytes < 1024) {
        return QString("%1 B").arg(bytes);
    } else if (bytes < 1024 * 1024) {
        return QString("%1 KB").arg(bytes / 1024.0, 0, 'f', 1);
    } else if (bytes < 1024 * 1024 * 1024) {
        return QString("%1 MB").arg(bytes / (1024.0 * 1024.0), 0, 'f', 1);
    } else {
        return QString("%1 GB").arg(bytes / (1024.0 * 1024.0 * 1024.0), 0, 'f', 1);
    }
}

QString DownloadTableWidget::formatSpeed(int bytesPerSecond) const
{
    if (bytesPerSecond == 0) {
        return "0 KB/s";
    }
    return formatFileSize(bytesPerSecond) + "/s";
}

QString DownloadTableWidget::formatTimeRemaining(int seconds) const
{
    if (seconds < 60) {
        return QString("%1s").arg(seconds);
    } else if (seconds < 3600) {
        return QString("%1m %2s").arg(seconds / 60).arg(seconds % 60);
    } else {
        int hours = seconds / 3600;
        int mins = (seconds % 3600) / 60;
        return QString("%1h %2m").arg(hours).arg(mins);
    }
}

void DownloadTableWidget::showContextMenu(const QPoint &pos)
{
    QTableWidgetItem *item = itemAt(pos);
    if (!item) return;
    
    int row = item->row();
    int downloadId = this->item(row, 0)->data(Qt::UserRole).toInt();
    
    // Enable/disable actions based on download status
    QString status = this->item(row, 5)->text();
    m_resumeAction->setEnabled(status == "Paused" || status == "Stopped");
    m_pauseAction->setEnabled(status == "Downloading");
    m_stopAction->setEnabled(status == "Downloading" || status == "Paused");
    
    QAction *action = m_contextMenu->exec(mapToGlobal(pos));
    if (action == m_resumeAction) {
        emit downloadResumeRequested(downloadId);
    } else if (action == m_pauseAction) {
        emit downloadPauseRequested(downloadId);
    } else if (action == m_stopAction) {
        emit downloadStopRequested(downloadId);
    } else if (action == m_removeAction) {
        emit downloadRemoveRequested(downloadId);
    } else if (action == m_propertiesAction) {
        emit downloadPropertiesRequested(downloadId);
    }
}

void DownloadTableWidget::onItemDoubleClicked(QTableWidgetItem *item)
{
    if (!item) return;
    
    int row = item->row();
    int downloadId = this->item(row, 0)->data(Qt::UserRole).toInt();
    
    emit downloadDoubleClicked(downloadId);
}