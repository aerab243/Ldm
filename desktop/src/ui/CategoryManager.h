#ifndef CATEGORYMANAGER_H
#define CATEGORYMANAGER_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include "core/Category.h"
#include "core/Database.h"

class CategoryManager : public QDialog
{
    Q_OBJECT

public:
    explicit CategoryManager(Database *database, QWidget *parent = nullptr);
    ~CategoryManager();

private slots:
    void onAddCategoryClicked();
    void onEditCategoryClicked();
    void onDeleteCategoryClicked();
    void onCategoryDoubleClicked(QListWidgetItem *item);

private:
    Database *m_database;
    QListWidget *m_categoryList;
    QPushButton *m_addButton;
    QPushButton *m_editButton;
    QPushButton *m_deleteButton;
    QPushButton *m_closeButton;

    void setupUI();
    void loadCategories();
    void refreshCategories();
    bool categoryExists(const QString &name) const;
};

#endif // CATEGORYMANAGER_H