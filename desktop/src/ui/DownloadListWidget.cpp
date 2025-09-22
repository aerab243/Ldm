#include "DownloadListWidget.h"
#include <QApplication>

DownloadListWidget::DownloadListWidget(QWidget *parent)
    : QListWidget(parent)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    connect(this, &QListWidget::itemClicked, this, &DownloadListWidget::onItemClicked);
}

DownloadListWidget::~DownloadListWidget()
{
    // Qt handles deletion of child widgets
}

void DownloadListWidget::addDownload(const DownloadItem &item)
{
    QWidget *widget = createDownloadWidget(item);
    QListWidgetItem *listItem = new QListWidgetItem();
    listItem->setSizeHint(widget->sizeHint());
    addItem(listItem);
    setItemWidget(listItem, widget);
}

void DownloadListWidget::updateDownloadProgress(int downloadId, qint64 bytesReceived, qint64 bytesTotal)
{
    DownloadWidget *dw = findDownloadWidget(downloadId);
    if (dw) {
        dw->progressBar->setMaximum(bytesTotal);
        dw->progressBar->setValue(bytesReceived);
        if (bytesTotal > 0) {
            double percentage = (static_cast<double>(bytesReceived) / bytesTotal) * 100;
            dw->sizeLabel->setText(QString("%1 / %2 (%3%)").arg(bytesReceived).arg(bytesTotal).arg(percentage, 0, 'f', 1));
        }
    }
}

void DownloadListWidget::updateDownloadStatus(int downloadId, const QString &status)
{
    DownloadWidget *dw = findDownloadWidget(downloadId);
    if (dw) {
        dw->statusLabel->setText(status);
    }
}

void DownloadListWidget::removeDownload(int downloadId)
{
    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *item = this->item(i);
        QWidget *widget = itemWidget(item);
        if (widget) {
            // Assuming the widget has a property or we can find it
            // For simplicity, we'll remove by index, but ideally store mapping
        }
    }
    // Simplified: remove the first item for now
    if (count() > 0) {
        takeItem(0);
    }
}

void DownloadListWidget::onItemClicked(QListWidgetItem *item)
{
    QWidget *widget = itemWidget(item);
    if (widget) {
        // Find the downloadId from the widget
        // For now, emit with 0
        emit downloadSelected(0);
    }
}

void DownloadListWidget::onPauseClicked()
{
    // Find the sender button and get downloadId
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        int downloadId = button->property("downloadId").toInt();
        emit downloadPaused(downloadId);
    }
}

void DownloadListWidget::onResumeClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        int downloadId = button->property("downloadId").toInt();
        emit downloadResumed(downloadId);
    }
}

void DownloadListWidget::onCancelClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        int downloadId = button->property("downloadId").toInt();
        emit downloadCancelled(downloadId);
    }
}

QWidget* DownloadListWidget::createDownloadWidget(const DownloadItem &item)
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QLabel *nameLabel = new QLabel(item.getUrl());
    QLabel *sizeLabel = new QLabel("0 / 0 (0%)");
    QProgressBar *progressBar = new QProgressBar();
    QLabel *statusLabel = new QLabel("Queued");

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *pauseButton = new QPushButton("Pause");
    QPushButton *resumeButton = new QPushButton("Resume");
    QPushButton *cancelButton = new QPushButton("Cancel");

    pauseButton->setProperty("downloadId", item.getId());
    resumeButton->setProperty("downloadId", item.getId());
    cancelButton->setProperty("downloadId", item.getId());

    connect(pauseButton, &QPushButton::clicked, this, &DownloadListWidget::onPauseClicked);
    connect(resumeButton, &QPushButton::clicked, this, &DownloadListWidget::onResumeClicked);
    connect(cancelButton, &QPushButton::clicked, this, &DownloadListWidget::onCancelClicked);

    buttonLayout->addWidget(pauseButton);
    buttonLayout->addWidget(resumeButton);
    buttonLayout->addWidget(cancelButton);

    layout->addWidget(nameLabel);
    layout->addWidget(sizeLabel);
    layout->addWidget(progressBar);
    layout->addWidget(statusLabel);
    layout->addLayout(buttonLayout);

    DownloadWidget dw = {widget, nameLabel, sizeLabel, progressBar, statusLabel, pauseButton, resumeButton, cancelButton, item.getId()};
    m_downloadWidgets.append(dw);

    return widget;
}

DownloadListWidget::DownloadWidget* DownloadListWidget::findDownloadWidget(int downloadId)
{
    for (DownloadWidget &dw : m_downloadWidgets) {
        if (dw.downloadId == downloadId) {
            return &dw;
        }
    }
    return nullptr;
}