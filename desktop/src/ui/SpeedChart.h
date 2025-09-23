#ifndef SPEEDCHART_H
#define SPEEDCHART_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTimer>
#include <QDateTime>
#include <QList>
#include <QVector>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QFontMetrics>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QRect>
#include <QPoint>
#include <QSize>
#include <QToolTip>
#include <QSettings>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QGraphicsDropShadowEffect>
#include <QPainterPath>
#include <QPolygonF>
#include <QApplication>
#include <QStyle>
#include <QPalette>
#include <QStyleOption>

/**
 * Real-time speed chart widget that reproduces IDM's speed graph exactly
 * Features:
 * - IDM-style green speed line with smooth curves
 * - Grid background with time and speed markings
 * - Real-time data updates with smooth animations
 * - Hover tooltips showing exact speed at time points
 * - Auto-scaling Y-axis based on peak speeds
 * - Time axis showing last N minutes of data
 * - Peak speed indicator and average speed line
 * - Visual effects: glow, shadows, anti-aliasing
 * - Zooming and panning capabilities
 * - Export functionality for charts
 */
class SpeedChart : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor lineColor READ lineColor WRITE setLineColor)
    Q_PROPERTY(QColor gridColor READ gridColor WRITE setGridColor)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
    Q_PROPERTY(bool showGrid READ showGrid WRITE setShowGrid)
    Q_PROPERTY(bool showLabels READ showLabels WRITE setShowLabels)

public:
    explicit SpeedChart(QWidget *parent = nullptr);
    ~SpeedChart();

    // === PUBLIC INTERFACE ===
    
    // Data management
    void addDataPoint(int speedBytesPerSec, const QDateTime &timestamp = QDateTime::currentDateTime());
    void addDataPoints(const QList<QPair<int, QDateTime>> &dataPoints);
    void clearData();
    void removeOldData(int maxAgeMinutes = 10);
    void setMaxDataPoints(int maxPoints);
    int maxDataPoints() const { return m_maxDataPoints; }
    
    // Time range and scaling
    void setTimeRange(int minutes);
    int timeRange() const { return m_timeRangeMinutes; }
    void setAutoScale(bool enabled);
    bool autoScale() const { return m_autoScale; }
    void setMaxSpeed(int maxSpeedBytesPerSec);
    int maxSpeed() const { return m_maxSpeed; }
    void setMinSpeed(int minSpeedBytesPerSec);
    int minSpeed() const { return m_minSpeed; }
    
    // Visual customization
    void setLineColor(const QColor &color);
    QColor lineColor() const { return m_lineColor; }
    void setGridColor(const QColor &color);
    QColor gridColor() const { return m_gridColor; }
    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const { return m_backgroundColor; }
    void setTextColor(const QColor &color);
    QColor textColor() const { return m_textColor; }
    
    // Grid and labels
    void setShowGrid(bool show);
    bool showGrid() const { return m_showGrid; }
    void setShowLabels(bool show);
    bool showLabels() const { return m_showLabels; }
    void setShowPeakIndicator(bool show);
    bool showPeakIndicator() const { return m_showPeakIndicator; }
    void setShowAverageLine(bool show);
    bool showAverageLine() const { return m_showAverageLine; }
    
    // Animation and effects
    void setAnimationEnabled(bool enabled);
    bool animationEnabled() const { return m_animationEnabled; }
    void setGlowEffect(bool enabled);
    bool glowEffect() const { return m_glowEffect; }
    void setSmoothCurves(bool enabled);
    bool smoothCurves() const { return m_smoothCurves; }
    
    // Statistics
    int getCurrentSpeed() const;
    int getAverageSpeed() const;
    int getPeakSpeed() const;
    QDateTime getLastUpdateTime() const;
    int getDataPointCount() const;
    
    // Utility
    void saveSettings();
    void loadSettings();
    void resetToDefaults();
    QPixmap exportChart(const QSize &size = QSize()) const;

    // === CHART STYLES ===
    enum ChartStyle {
        StyleIDM,           // Classic IDM green line style
        StyleModern,        // Modern gradient style
        StyleMinimal,       // Minimal clean style
        StyleClassic        // Classic line chart style
    };

    void setChartStyle(ChartStyle style);
    ChartStyle chartStyle() const { return m_chartStyle; }

    // === SCALING MODES ===
    enum ScalingMode {
        ScaleAutomatic,     // Auto-scale based on data
        ScaleFixed,         // Fixed min/max values
        ScaleAdaptive       // Adaptive scaling with smoothing
    };

    void setScalingMode(ScalingMode mode);
    ScalingMode scalingMode() const { return m_scalingMode; }

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void leaveEvent(QEvent *event) override;

signals:
    void dataPointAdded(int speed, const QDateTime &timestamp);
    void peakSpeedChanged(int peakSpeed);
    void averageSpeedChanged(int averageSpeed);
    void chartClicked(const QPoint &position);
    void timeRangeChanged(int minutes);
    void scaleChanged(int minSpeed, int maxSpeed);

private slots:
    void onUpdateTimer();
    void onAnimationTimer();
    void onDataCleanupTimer();

private:
    // === DATA STRUCTURES ===
    
    struct DataPoint {
        QDateTime timestamp;
        int speed;              // bytes per second
        bool isValid;
        
        DataPoint() : speed(0), isValid(false) {}
        DataPoint(int s, const QDateTime &t) : timestamp(t), speed(s), isValid(true) {}
    };

    struct ChartGeometry {
        QRect chartRect;        // Chart drawing area
        QRect gridRect;         // Grid area
        QRect labelRect;        // Label area
        int marginLeft;
        int marginRight;
        int marginTop;
        int marginBottom;
        
        ChartGeometry() : marginLeft(50), marginRight(20), marginTop(20), marginBottom(30) {}
    };

    struct VisualSettings {
        QColor lineColor;
        QColor fillColor;
        QColor gridColor;
        QColor backgroundColor;
        QColor textColor;
        QColor peakColor;
        QColor averageColor;
        QFont labelFont;
        QFont valueFont;
        int lineWidth;
        int gridLineWidth;
        bool antiAliasing;
        
        VisualSettings() : lineWidth(2), gridLineWidth(1), antiAliasing(true) {}
    };

    // === MEMBER VARIABLES ===
    
    // Data storage
    QList<DataPoint> m_dataPoints;
    int m_maxDataPoints;
    QDateTime m_lastCleanup;
    
    // Time and scaling
    int m_timeRangeMinutes;
    int m_maxSpeed;
    int m_minSpeed;
    bool m_autoScale;
    ScalingMode m_scalingMode;
    
    // Visual settings
    VisualSettings m_visual;
    ChartStyle m_chartStyle;
    bool m_showGrid;
    bool m_showLabels;
    bool m_showPeakIndicator;
    bool m_showAverageLine;
    
    // Effects and animation
    bool m_animationEnabled;
    bool m_glowEffect;
    bool m_smoothCurves;
    QTimer *m_updateTimer;
    QTimer *m_animationTimer;
    QTimer *m_dataCleanupTimer;
    
    // Interaction state
    QPoint m_mousePos;
    bool m_mouseTracking;
    bool m_dragging;
    QPoint m_dragStart;
    int m_zoomLevel;
    
    // Geometry and layout
    ChartGeometry m_geometry;
    bool m_geometryDirty;
    
    // Cached calculations
    mutable int m_cachedAverageSpeed;
    mutable int m_cachedPeakSpeed;
    mutable bool m_statsDirty;
    
    // Visual effects
    QGraphicsDropShadowEffect *m_shadowEffect;
    QPropertyAnimation *m_glowAnimation;
    int m_glowPhase;
    
    // === SETUP METHODS ===
    void setupUI();
    void setupTimers();
    void setupVisualSettings();
    void setupAnimations();
    void applyIDMStyle();
    void setupGeometry();
    
    // === PAINTING METHODS ===
    void paintChart(QPainter &painter);
    void paintBackground(QPainter &painter, const QRect &rect);
    void paintGrid(QPainter &painter, const QRect &rect);
    void paintSpeedLine(QPainter &painter, const QRect &rect);
    void paintFillArea(QPainter &painter, const QRect &rect);
    void paintLabels(QPainter &painter, const QRect &rect);
    void paintPeakIndicator(QPainter &painter, const QRect &rect);
    void paintAverageLine(QPainter &painter, const QRect &rect);
    void paintTooltip(QPainter &painter);
    void paintGlowEffect(QPainter &painter, const QRect &rect);
    
    // === COORDINATE CONVERSION ===
    QPoint mapSpeedToPoint(int speed, const QDateTime &timestamp, const QRect &rect) const;
    QPair<int, QDateTime> mapPointToSpeed(const QPoint &point, const QRect &rect) const;
    QList<QPointF> getSpeedLinePoints(const QRect &rect) const;
    QPolygonF createSmoothCurve(const QList<QPointF> &points) const;
    
    // === SCALING AND RANGE ===
    void updateScaling();
    void calculateOptimalScale();
    QPair<int, int> getSpeedRange() const;
    QPair<QDateTime, QDateTime> getTimeRange() const;
    void adjustTimeRange();
    void adjustSpeedRange();
    
    // === GRID AND LABELS ===
    void drawTimeLabels(QPainter &painter, const QRect &rect);
    void drawSpeedLabels(QPainter &painter, const QRect &rect);
    void drawGridLines(QPainter &painter, const QRect &rect);
    QList<QDateTime> getTimeGridPoints(const QRect &rect) const;
    QList<int> getSpeedGridPoints(const QRect &rect) const;
    QString formatTimeLabel(const QDateTime &time) const;
    QString formatSpeedLabel(int speed) const;
    
    // === DATA UTILITIES ===
    void addDataPointInternal(const DataPoint &point);
    void removeOldDataPoints();
    void updateStatistics() const;
    DataPoint* findDataPointAt(const QDateTime &time);
    const DataPoint* findNearestDataPoint(const QDateTime &time) const;
    QList<DataPoint> getDataInTimeRange(const QDateTime &start, const QDateTime &end) const;
    
    // === VISUAL EFFECTS ===
    void updateGlowEffect();
    void animateNewDataPoint();
    void createShadowEffect();
    void updateVisualSettings();
    void applyChartStyle();
    
    // === INTERACTION UTILITIES ===
    void updateMouseTracking(const QPoint &pos);
    void showSpeedTooltip(const QPoint &pos);
    void handleChartClick(const QPoint &pos);
    void handleChartDrag(const QPoint &pos);
    void handleZoom(int delta);
    
    // === UTILITY METHODS ===
    QString formatSpeed(int bytesPerSecond) const;
    QString formatSpeedShort(int bytesPerSecond) const;
    QString formatTimeShort(const QDateTime &time) const;
    QString formatDuration(int seconds) const;
    QColor interpolateColor(const QColor &start, const QColor &end, qreal factor) const;
    
    // === CONSTANTS ===
    static const int DEFAULT_MAX_DATA_POINTS = 600;    // 10 minutes at 1 second intervals
    static const int DEFAULT_TIME_RANGE = 10;          // 10 minutes
    static const int DEFAULT_UPDATE_INTERVAL = 1000;   // 1 second
    static const int DEFAULT_ANIMATION_INTERVAL = 16;  // ~60 FPS
    static const int CLEANUP_INTERVAL = 60000;         // 1 minute
    static const int MIN_CHART_WIDTH = 200;
    static const int MIN_CHART_HEIGHT = 100;
    static const int DEFAULT_LINE_WIDTH = 2;
    static const int GLOW_LINE_WIDTH = 4;
    static const int PEAK_INDICATOR_SIZE = 8;
    static const int TOOLTIP_MARGIN = 10;
    static const int GRID_SPACING = 50;
    static const int LABEL_SPACING = 20;
    static const int TIME_LABEL_HEIGHT = 20;
    static const int SPEED_LABEL_WIDTH = 50;
    static const qreal SMOOTH_CURVE_TENSION = 0.3;
    static const int MAX_ZOOM_LEVEL = 5;
    static const int MIN_ZOOM_LEVEL = -3;
    
    // IDM Colors
    static const QColor IDM_GREEN;
    static const QColor IDM_GRID_COLOR;
    static const QColor IDM_BACKGROUND;
    static const QColor IDM_TEXT_COLOR;
    static const QColor IDM_PEAK_COLOR;
    static const QColor IDM_AVERAGE_COLOR;
};

#endif // SPEEDCHART_H