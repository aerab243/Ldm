#ifndef CATEGORYSIDEBAR_H
#define CATEGORYSIDEBAR_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>
#include <QHeaderView>
#include <QScrollBar>
#include <QSettings>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QFrame>
#include <QGroupBox>
#include <QPixmap>
#include <QIcon>
#include <QFont>
#include <QFontMetrics>
#include <QApplication>
#include <QStyle>

/**
 * Category sidebar widget that reproduces IDM's category panel exactly
 * Features:
 * - IDM-style category tree with icons
 * - Download counters in parentheses (e.g., "Downloading (3)")
 * - Search/filter box at top
 * - Custom categories support
 * - Drag & drop for organizing downloads
 * - Context menu for category management
 * - Highlighting for selected category
 * - Auto-updating counters
 * - IDM visual styling
 */
class CategorySidebar : public QWidget
{
    Q_OBJECT

public:
    explicit CategorySidebar(QWidget *parent = nullptr);
    ~CategorySidebar();

    // === PUBLIC INTERFACE ===
    
    // Category management
    void addCustomCategory(const QString &name, const QIcon &icon = QIcon());
    void removeCustomCategory(const QString &name);
    void renameCategory(const QString &oldName, const QString &newName);
    QStringList getCustomCategories() const;
    
    // Selection and filtering
    QString getCurrentCategory() const;
    void setCurrentCategory(const QString &category);
    void refreshCategoryCounters();
    void updateCategoryCount(const QString &category, int count);
    
    // Search functionality
    void setSearchText(const QString &text);
    QString getSearchText() const;
    void clearSearch();
    
    // UI state
    void saveSettings();
    void loadSettings();
    void resetToDefaults();
    
    // === IDM CATEGORY CONSTANTS ===
    static const QString CATEGORY_ALL;
    static const QString CATEGORY_DOWNLOADING;
    static const QString CATEGORY_COMPLETED;
    static const QString CATEGORY_UNFINISHED;
    static const QString CATEGORY_VIDEO;
    static const QString CATEGORY_AUDIO;
    static const QString CATEGORY_DOCUMENTS;
    static const QString CATEGORY_COMPRESSED;
    static const QString CATEGORY_PROGRAMS;

    enum CategoryType {
        TypeBuiltIn,
        TypeCustom,
        TypeSeparator
    };

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

signals:
    // Category selection
    void categorySelected(const QString &category);
    void categoryChanged(const QString &oldCategory, const QString &newCategory);
    
    // Search events
    void searchTextChanged(const QString &text);
    void searchCleared();
    
    // Category management
    void customCategoryAdded(const QString &name);
    void customCategoryRemoved(const QString &name);
    void customCategoryRenamed(const QString &oldName, const QString &newName);
    
    // Drag & drop events
    void downloadsDroppedOnCategory(const QStringList &downloadIds, const QString &category);

private slots:
    void onCategorySelectionChanged();
    void onCategoryDoubleClicked(QTreeWidgetItem *item, int column);
    void onSearchTextChanged(const QString &text);
    void onSearchClearClicked();
    void onUpdateTimer();
    
    // Context menu actions
    void onAddCustomCategory();
    void onRenameCategory();
    void onRemoveCategory();
    void onSetCategoryIcon();
    void onCategorySeparator();
    void onExpandAll();
    void onCollapseAll();
    void onResetCategories();

private:
    // === DATA STRUCTURES ===
    
    struct CategoryItem {
        QString name;
        QString displayName;
        QIcon icon;
        CategoryType type;
        int count;
        bool expanded;
        QTreeWidgetItem *treeItem;
        
        CategoryItem() : type(TypeBuiltIn), count(0), expanded(true), treeItem(nullptr) {}
        CategoryItem(const QString &n, const QIcon &i, CategoryType t) 
            : name(n), displayName(n), icon(i), type(t), count(0), expanded(true), treeItem(nullptr) {}
    };

    // === MEMBER VARIABLES ===
    
    // Main layout
    QVBoxLayout *m_mainLayout;
    QFrame *m_searchFrame;
    QHBoxLayout *m_searchLayout;
    QLineEdit *m_searchEdit;
    QPushButton *m_searchClearButton;
    QTreeWidget *m_categoryTree;
    
    // Data
    QList<CategoryItem> m_categories;
    QMap<QString, int> m_categoryCounts;
    QString m_currentCategory;
    QString m_searchText;
    
    // UI components
    QMenu *m_contextMenu;
    QSettings *m_settings;
    QTimer *m_updateTimer;
    
    // Context menu actions
    QAction *m_addCategoryAction;
    QAction *m_renameCategoryAction;
    QAction *m_removeCategoryAction;
    QAction *m_setCategoryIconAction;
    QAction *m_separatorAction;
    QAction *m_expandAllAction;
    QAction *m_collapseAllAction;
    QAction *m_resetCategoriesAction;
    
    // Visual state
    QFont m_categoryFont;
    QFont m_countFont;
    QColor m_selectedColor;
    QColor m_hoverColor;
    QColor m_textColor;
    QColor m_countColor;
    
    // Animation
    QPropertyAnimation *m_selectionAnimation;
    QGraphicsDropShadowEffect *m_shadowEffect;
    
    // === SETUP METHODS ===
    void setupUI();
    void setupSearchBox();
    void setupCategoryTree();
    void setupContextMenu();
    void setupAnimations();
    void applyIDMStyle();
    void loadIDMIcons();
    
    // === CATEGORY MANAGEMENT ===
    void initializeDefaultCategories();
    void populateCategoryTree();
    void addCategoryToTree(const CategoryItem &category);
    void updateCategoryInTree(const CategoryItem &category);
    void removeCategoryFromTree(const QString &categoryName);
    void refreshCategoryTree();
    
    // === TREE WIDGET MANAGEMENT ===
    QTreeWidgetItem* createCategoryItem(const CategoryItem &category);
    void updateCategoryItemText(QTreeWidgetItem *item, const CategoryItem &category);
    void updateCategoryItemIcon(QTreeWidgetItem *item, const QIcon &icon);
    void updateCategoryItemCount(QTreeWidgetItem *item, int count);
    
    // === SEARCH FUNCTIONALITY ===
    void filterCategories(const QString &searchText);
    bool categoryMatchesSearch(const CategoryItem &category, const QString &searchText) const;
    void highlightSearchResults(const QString &searchText);
    
    // === SELECTION MANAGEMENT ===
    void selectCategory(const QString &categoryName);
    void selectCategoryItem(QTreeWidgetItem *item);
    CategoryItem* getCategoryByName(const QString &name);
    CategoryItem* getCategoryByTreeItem(QTreeWidgetItem *item);
    QString getCategoryNameFromItem(QTreeWidgetItem *item) const;
    
    // === VISUAL STYLING ===
    void updateCategoryItemStyle(QTreeWidgetItem *item, bool selected = false);
    void paintCategoryBackground(QPainter &painter, const QRect &rect, bool selected, bool hovered);
    void paintCategoryText(QPainter &painter, const QRect &rect, const QString &text, const QFont &font, const QColor &color);
    void paintCategoryCount(QPainter &painter, const QRect &rect, int count);
    void paintCategoryIcon(QPainter &painter, const QRect &rect, const QIcon &icon);
    
    // === DRAG & DROP ===
    void setupDragDrop();
    bool isValidDropTarget(QTreeWidgetItem *item) const;
    void handleDownloadDrop(QTreeWidgetItem *targetItem, const QStringList &downloadIds);
    
    // === CONTEXT MENU MANAGEMENT ===
    void showContextMenu(const QPoint &pos);
    void updateContextMenuActions();
    bool canRenameCategory(const QString &categoryName) const;
    bool canRemoveCategory(const QString &categoryName) const;
    
    // === ICON MANAGEMENT ===
    QIcon getDefaultCategoryIcon(const QString &categoryName) const;
    QIcon loadCategoryIcon(const QString &iconName) const;
    void setCategoryIcon(const QString &categoryName, const QIcon &icon);
    
    // === UTILITY METHODS ===
    QString formatCategoryDisplayName(const QString &name, int count) const;
    QString getCategoryDisplayName(const QString &name) const;
    int getCategoryCount(const QString &name) const;
    bool isCategoryExpanded(const QString &name) const;
    void setCategoryExpanded(const QString &name, bool expanded);
    
    // === ANIMATION UTILITIES ===
    void animateSelection(QTreeWidgetItem *item);
    void animateCountChange(QTreeWidgetItem *item);
    void highlightNewCategory(QTreeWidgetItem *item);
    
    // === CONSTANTS ===
    static const int UPDATE_INTERVAL = 5000;           // Update counters every 5 seconds
    static const int ANIMATION_DURATION = 200;         // Animation duration in ms
    static const int CATEGORY_ICON_SIZE = 16;          // Category icon size
    static const int CATEGORY_INDENT = 20;             // Category indentation
    static const int CATEGORY_HEIGHT = 22;             // Category item height
    static const int SEARCH_HEIGHT = 25;               // Search box height
    static const int SIDEBAR_MIN_WIDTH = 150;          // Minimum sidebar width
    static const int SIDEBAR_MAX_WIDTH = 300;          // Maximum sidebar width
    static const int COUNT_MARGIN = 5;                 // Margin around count text
};

/**
 * Custom tree widget item for categories with IDM styling
 */
class CategoryTreeWidgetItem : public QTreeWidgetItem
{
public:
    explicit CategoryTreeWidgetItem(QTreeWidget *parent, const QString &categoryName, int count = 0);
    explicit CategoryTreeWidgetItem(QTreeWidgetItem *parent, const QString &categoryName, int count = 0);
    
    void updateCount(int count);
    void updateIcon(const QIcon &icon);
    void setCategory(const QString &categoryName);
    QString getCategory() const;
    
private:
    QString m_categoryName;
    int m_count;
    
    void updateDisplayText();
};

/**
 * Custom tree widget with IDM-style rendering and drag & drop
 */
class CategoryTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit CategoryTreeWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void startDrag(Qt::DropActions supportedActions) override;

signals:
    void categoryContextMenuRequested(const QPoint &pos, QTreeWidgetItem *item);
    void downloadsDropped(QTreeWidgetItem *targetItem, const QStringList &downloadIds);

private:
    void paintCategoryItem(QPainter &painter, QTreeWidgetItem *item, const QRect &rect);
    void paintSelectionBackground(QPainter &painter, const QRect &rect);
    void paintHoverBackground(QPainter &painter, const QRect &rect);
    
    QTreeWidgetItem *m_hoveredItem;
    QPoint m_dragStartPosition;
    bool m_dragging;
};

#endif // CATEGORYSIDEBAR_H