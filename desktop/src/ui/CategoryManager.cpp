#include "CategoryManager.h"

CategoryManager::CategoryManager(Database *database, QWidget *parent)
    : QDialog(parent), m_database(database)
{
    setupUI();
    loadCategories();
}

CategoryManager::~CategoryManager()
{
}

void CategoryManager::setupUI()
{
    setWindowTitle("Category Manager");
    setModal(true);
    resize(400, 300);

    m_categoryList = new QListWidget(this);
    m_categoryList->setDragEnabled(true);
    m_categoryList->setAcceptDrops(true);
    m_categoryList->setDropIndicatorShown(true);
    m_categoryList->setDragDropMode(QAbstractItemView::InternalMove);
    m_addButton = new QPushButton("Add Category", this);
    m_editButton = new QPushButton("Edit Category", this);
    m_deleteButton = new QPushButton("Delete Category", this);
    m_closeButton = new QPushButton("Close", this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_categoryList);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_editButton);
    buttonLayout->addWidget(m_deleteButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_closeButton);

    mainLayout->addLayout(buttonLayout);

    connect(m_addButton, &QPushButton::clicked, this, &CategoryManager::onAddCategoryClicked);
    connect(m_editButton, &QPushButton::clicked, this, &CategoryManager::onEditCategoryClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &CategoryManager::onDeleteCategoryClicked);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_categoryList, &QListWidget::itemDoubleClicked, this, &CategoryManager::onCategoryDoubleClicked);
}

void CategoryManager::loadCategories()
{
    m_categoryList->clear();
    QVariantList categories = m_database->getCategories();
    for (const QVariant &variant : categories) {
        QVariantMap category = variant.toMap();
        QListWidgetItem *item = new QListWidgetItem(category["name"].toString());
        item->setData(Qt::UserRole, category["id"]);
        m_categoryList->addItem(item);
    }
}

void CategoryManager::refreshCategories()
{
    loadCategories();
}

void CategoryManager::onAddCategoryClicked()
{
    bool ok;
    QString name = QInputDialog::getText(this, "Add Category", "Category name:", QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        if (categoryExists(name)) {
            QMessageBox::warning(this, "Duplicate Category", "A category with this name already exists.");
            return;
        }
        QVariantMap categoryData;
        categoryData["name"] = name;
        if (m_database->insertCategory(categoryData)) {
            refreshCategories();
        } else {
            QMessageBox::critical(this, "Error", "Failed to add category.");
        }
    }
}

void CategoryManager::onEditCategoryClicked()
{
    QListWidgetItem *item = m_categoryList->currentItem();
    if (!item) {
        QMessageBox::information(this, "No Selection", "Please select a category to edit.");
        return;
    }

    int id = item->data(Qt::UserRole).toInt();
    QString currentName = item->text();

    bool ok;
    QString newName = QInputDialog::getText(this, "Edit Category", "Category name:", QLineEdit::Normal, currentName, &ok);
    if (ok && !newName.isEmpty() && newName != currentName) {
        if (categoryExists(newName)) {
            QMessageBox::warning(this, "Duplicate Category", "A category with this name already exists.");
            return;
        }
        QVariantMap categoryData;
        categoryData["name"] = newName;
        if (m_database->updateCategory(id, categoryData)) {
            refreshCategories();
        } else {
            QMessageBox::critical(this, "Error", "Failed to update category.");
        }
    }
}

void CategoryManager::onDeleteCategoryClicked()
{
    QListWidgetItem *item = m_categoryList->currentItem();
    if (!item) {
        QMessageBox::information(this, "No Selection", "Please select a category to delete.");
        return;
    }

    int id = item->data(Qt::UserRole).toInt();
    QString name = item->text();

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Delete Category",
        QString("Are you sure you want to delete the category '%1'?").arg(name),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (m_database->deleteCategory(id)) {
            refreshCategories();
        } else {
            QMessageBox::critical(this, "Error", "Failed to delete category.");
        }
    }
}

void CategoryManager::onCategoryDoubleClicked(QListWidgetItem *item)
{
    onEditCategoryClicked();
}

bool CategoryManager::categoryExists(const QString &name) const
{
    for (int i = 0; i < m_categoryList->count(); ++i) {
        if (m_categoryList->item(i)->text().compare(name, Qt::CaseInsensitive) == 0) {
            return true;
        }
    }
    return false;
}