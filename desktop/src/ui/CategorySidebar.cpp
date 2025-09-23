#include "CategorySidebar.h"
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QLineEdit>

CategorySidebar::CategorySidebar(QWidget *parent)
    : QWidget(parent)
{
    setFixedWidth(200);
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(5);
    
    // Search box
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Search categories...");
    layout->addWidget(m_searchEdit);
    
    // Category tree
    m_categoryTree = new QTreeWidget(this);
    m_categoryTree->setHeaderHidden(true);
    m_categoryTree->setRootIsDecorated(false);
    m_categoryTree->setIndentation(10);
    layout->addWidget(m_categoryTree);
    
    // Add default categories
    QTreeWidgetItem *item;
    
    item = new QTreeWidgetItem(QStringList("All Downloads"));
    item->setData(0, Qt::UserRole, "all");
    m_categoryTree->addTopLevelItem(item);
    
    item = new QTreeWidgetItem(QStringList("Downloading"));
    item->setData(0, Qt::UserRole, "downloading");
    m_categoryTree->addTopLevelItem(item);
    
    item = new QTreeWidgetItem(QStringList("Completed"));
    item->setData(0, Qt::UserRole, "completed");
    m_categoryTree->addTopLevelItem(item);
    
    item = new QTreeWidgetItem(QStringList("Paused"));
    item->setData(0, Qt::UserRole, "paused");
    m_categoryTree->addTopLevelItem(item);
    
    item = new QTreeWidgetItem(QStringList("Failed"));
    item->setData(0, Qt::UserRole, "failed");
    m_categoryTree->addTopLevelItem(item);
    
    // Select first item
    if (m_categoryTree->topLevelItemCount() > 0) {
        m_categoryTree->setCurrentItem(m_categoryTree->topLevelItem(0));
    }
    
    // Connect signals
    connect(m_categoryTree, &QTreeWidget::currentItemChanged,
            this, [this](QTreeWidgetItem *current, QTreeWidgetItem *previous) {
                Q_UNUSED(previous)
                if (!current) return;
                QString category = current->data(0, Qt::UserRole).toString();
                if (!category.isEmpty()) {
                    emit categoryChanged("", category); // old, new
                }
            });
    
    setLayout(layout);
}

CategorySidebar::~CategorySidebar()
{
}

QString CategorySidebar::getCurrentCategory() const
{
    QTreeWidgetItem *current = m_categoryTree->currentItem();
    if (!current) return "all";
    return current->data(0, Qt::UserRole).toString();
}

void CategorySidebar::setCurrentCategory(const QString &category)
{
    for (int i = 0; i < m_categoryTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = m_categoryTree->topLevelItem(i);
        if (item && item->data(0, Qt::UserRole).toString() == category) {
            m_categoryTree->setCurrentItem(item);
            break;
        }
    }
}

void CategorySidebar::refreshCategoryCounters()
{
    // Stub implementation
}

void CategorySidebar::updateCategoryCount(const QString &category, int count)
{
    for (int i = 0; i < m_categoryTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = m_categoryTree->topLevelItem(i);
        if (item && item->data(0, Qt::UserRole).toString() == category) {
            QString baseName = item->text(0).split(" (").first();
            QString text = baseName;
            if (count > 0) {
                text += QString(" (%1)").arg(count);
            }
            item->setText(0, text);
            break;
        }
    }
}

// Stub implementations for missing methods
void CategorySidebar::addCustomCategory(const QString &, const QIcon &) { /* stub */ }
void CategorySidebar::removeCustomCategory(const QString &) { /* stub */ }
void CategorySidebar::renameCategory(const QString &, const QString &) { /* stub */ }
QStringList CategorySidebar::getCustomCategories() const { return QStringList(); }
void CategorySidebar::setSearchText(const QString &) { /* stub */ }
QString CategorySidebar::getSearchText() const { return QString(); }
void CategorySidebar::clearSearch() { /* stub */ }
void CategorySidebar::saveSettings() { /* stub */ }
void CategorySidebar::loadSettings() { /* stub */ }
void CategorySidebar::resetToDefaults() { /* stub */ }
void CategorySidebar::contextMenuEvent(QContextMenuEvent *) { /* stub */ }
void CategorySidebar::paintEvent(QPaintEvent *) { /* stub */ }
void CategorySidebar::resizeEvent(QResizeEvent *) { /* stub */ }

// Slot stubs
void CategorySidebar::onCategorySelectionChanged() { /* stub */ }
void CategorySidebar::onCategoryDoubleClicked(QTreeWidgetItem *, int) { /* stub */ }
void CategorySidebar::onSearchTextChanged(const QString &) { /* stub */ }
void CategorySidebar::onSearchClearClicked() { /* stub */ }
void CategorySidebar::onUpdateTimer() { /* stub */ }
void CategorySidebar::onAddCustomCategory() { /* stub */ }
void CategorySidebar::onRenameCategory() { /* stub */ }
void CategorySidebar::onRemoveCategory() { /* stub */ }
void CategorySidebar::onSetCategoryIcon() { /* stub */ }
void CategorySidebar::onCategorySeparator() { /* stub */ }
void CategorySidebar::onExpandAll() { /* stub */ }
void CategorySidebar::onCollapseAll() { /* stub */ }
void CategorySidebar::onResetCategories() { /* stub */ }

// Constants
const QString CategorySidebar::CATEGORY_ALL = "all";
const QString CategorySidebar::CATEGORY_DOWNLOADING = "downloading";
const QString CategorySidebar::CATEGORY_COMPLETED = "completed";
const QString CategorySidebar::CATEGORY_UNFINISHED = "unfinished";
const QString CategorySidebar::CATEGORY_VIDEO = "video";
const QString CategorySidebar::CATEGORY_AUDIO = "audio";
const QString CategorySidebar::CATEGORY_DOCUMENTS = "documents";
const QString CategorySidebar::CATEGORY_COMPRESSED = "compressed";
const QString CategorySidebar::CATEGORY_PROGRAMS = "programs";