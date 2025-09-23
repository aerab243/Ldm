#ifndef CUSTOMPROGRESSBAR_H
#define CUSTOMPROGRESSBAR_H

#include <QProgressBar>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QStyleOption>
#include <QFont>
#include <QFontMetrics>
#include <QColor>
#include <QLinearGradient>
#include <QBrush>
#include <QPen>
#include <QRect>
#include <QSize>
#include <QTimer>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QGraphicsDropShadowEffect>
#include <QApplication>
#include <QStyle>
#include <QPalette>

// Enums for progress bar status
enum class ProgressStatus {
    Normal,
    Paused,
    Error,
    Completed,
    Connecting,
    Queued
};

enum class SegmentStatus {
    Empty,
    Downloading,
    Completed,
    Error,
    Paused
};

/**
 * Custom progress bar that reproduces IDM's progress bar style exactly
 * Features:
 * - IDM-style gradient background and fill
 * - Embedded percentage text in center
 * - Smooth animations for progress changes
 * - Color coding based on status (downloading, paused, completed, error)
 * - Segmented progress visualization
 * - Hover effects and visual feedback
 * - High DPI support with crisp rendering
 * - Customizable appearance and behavior
 */
class CustomProgressBar : public QProgressBar
{
    Q_OBJECT
    Q_PROPERTY(int animatedValue READ animatedValue WRITE setAnimatedValue)
    Q_PROPERTY(QColor fillColor READ fillColor WRITE setFillColor)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
    Q_PROPERTY(bool showPercentage READ showPercentage WRITE setShowPercentage)

public:
    explicit CustomProgressBar(QWidget *parent = nullptr);
    ~CustomProgressBar();

    // === PUBLIC INTERFACE ===
    
    // Progress control
    void setValue(int value);
    void setValueAnimated(int value, int duration = 300);
    void setRange(int minimum, int maximum);
    
    // Visual customization
    void setFillColor(const QColor &color);
    QColor fillColor() const { return m_fillColor; }
    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const { return m_backgroundColor; }
    void setBorderColor(const QColor &color);
    QColor borderColor() const { return m_borderColor; }
    void setTextColor(const QColor &color);
    QColor textColor() const { return m_textColor; }
    
    // Text and formatting
    void setShowPercentage(bool show);
    bool showPercentage() const { return m_showPercentage; }
    void setCustomText(const QString &text);
    QString customText() const { return m_customText; }
    void setTextFormat(const QString &format);
    QString textFormat() const { return m_textFormat; }
    
    // Status and appearance
    void setStatus(::ProgressStatus status);
    ::ProgressStatus status() const { return m_status; }
    void setSegmented(bool segmented, int segmentCount = 8);
    bool isSegmented() const { return m_segmented; }
    void setGlowEffect(bool enabled);
    bool hasGlowEffect() const { return m_glowEffect; }
    
    // Animation control
    void setAnimationEnabled(bool enabled);
    bool isAnimationEnabled() const { return m_animationEnabled; }
    void setAnimationDuration(int duration);
    int animationDuration() const { return m_animationDuration; }
    
    // Size and layout
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    void setFixedHeight(int height);
    void setProgressHeight(int height);

    // === STATUS ENUM ===
    // Using global ProgressStatus enum class defined above

    // === ANIMATION STATE ===
    enum AnimationState {
        AnimationIdle,
        AnimationRunning,
        AnimationFinished
    };

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void clicked();
    void statusChanged(ProgressStatus status);
    void animationFinished();

private slots:
    void onAnimationValueChanged(const QVariant &value);
    void onAnimationFinished();
    void onGlowTimer();

private:
    // === MEMBER VARIABLES ===
    
    // Progress values
    int m_animatedValue;
    int m_targetValue;
    
    // Colors and styling
    QColor m_fillColor;
    QColor m_backgroundColor;
    QColor m_borderColor;
    QColor m_textColor;
    QColor m_hoverColor;
    QColor m_shadowColor;
    
    // Text and formatting
    bool m_showPercentage;
    QString m_customText;
    QString m_textFormat;
    QFont m_textFont;
    
    // Status and appearance
    ::ProgressStatus m_status;
    bool m_segmented;
    int m_segmentCount;
    bool m_glowEffect;
    bool m_hovered;
    bool m_pressed;
    
    // Animation
    QPropertyAnimation *m_progressAnimation;
    bool m_animationEnabled;
    int m_animationDuration;
    AnimationState m_animationState;
    
    // Glow effect
    QTimer *m_glowTimer;
    int m_glowPhase;
    QGraphicsDropShadowEffect *m_shadowEffect;
    
    // Geometry
    int m_progressHeight;
    int m_borderWidth;
    int m_cornerRadius;
    
    // === SETUP METHODS ===
    void setupUI();
    void setupAnimation();
    void setupColors();
    void setupFonts();
    void setupEffects();
    
    // === PAINTING METHODS ===
    void paintBackground(QPainter &painter, const QRect &rect);
    void paintBorder(QPainter &painter, const QRect &rect);
    void paintProgress(QPainter &painter, const QRect &rect);
    void paintSegments(QPainter &painter, const QRect &rect);
    void paintText(QPainter &painter, const QRect &rect);
    void paintGlow(QPainter &painter, const QRect &rect);
    void paintHighlight(QPainter &painter, const QRect &rect);
    
    // === GRADIENT CREATION ===
    QLinearGradient createBackgroundGradient(const QRect &rect) const;
    QLinearGradient createProgressGradient(const QRect &rect) const;
    QLinearGradient createHoverGradient(const QRect &rect) const;
    QLinearGradient createGlowGradient(const QRect &rect) const;
    
    // === COLOR UTILITIES ===
    QColor getStatusColor(ProgressStatus status) const;
    QColor lightenColor(const QColor &color, int factor = 150) const;
    QColor darkenColor(const QColor &color, int factor = 150) const;
    QColor adjustColorAlpha(const QColor &color, int alpha) const;
    
    // === TEXT UTILITIES ===
    QString getProgressText() const;
    QString formatPercentage(int value) const;
    QRect getTextRect(const QRect &progressRect) const;
    void calculateOptimalFontSize(const QRect &rect);
    
    // === GEOMETRY UTILITIES ===
    QRect getProgressRect() const;
    QRect getFilledRect(const QRect &progressRect) const;
    QRect getBorderRect() const;
    QList<QRect> getSegmentRects(const QRect &progressRect) const;
    
    // === ANIMATION UTILITIES ===
    int animatedValue() const { return m_animatedValue; }
    void setAnimatedValue(int value);
    void startProgressAnimation(int targetValue);
    void stopProgressAnimation();
    
    // === VISUAL EFFECTS ===
    void updateGlowEffect();
    void updateHoverEffect();
    void updateShadowEffect();
    void applyIDMStyling();
    
    // === CONSTANTS ===
    static const int DEFAULT_HEIGHT = 16;
    static const int MIN_HEIGHT = 8;
    static const int MAX_HEIGHT = 32;
    static const int DEFAULT_BORDER_WIDTH = 1;
    static const int DEFAULT_CORNER_RADIUS = 2;
    static const int DEFAULT_ANIMATION_DURATION = 300;
    static const int GLOW_TIMER_INTERVAL = 50;
    static const int GLOW_CYCLE_DURATION = 2000;
    static const int SEGMENT_SPACING = 2;
    static const int TEXT_MARGIN = 4;
    static const int HOVER_ALPHA = 200;
    static const int GLOW_ALPHA = 100;
    static const int SHADOW_BLUR_RADIUS = 3;
    static const int SHADOW_OFFSET = 1;
};



/**
 * Circular progress indicator (for speeds, completion percentage)
 */
class CircularProgressBar : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue)
    Q_PROPERTY(QColor progressColor READ progressColor WRITE setProgressColor)

public:
    explicit CircularProgressBar(QWidget *parent = nullptr);
    
    // Progress control
    void setValue(int value);
    int value() const { return m_value; }
    void setRange(int minimum, int maximum);
    int minimum() const { return m_minimum; }
    int maximum() const { return m_maximum; }
    
    // Appearance
    void setProgressColor(const QColor &color);
    QColor progressColor() const { return m_progressColor; }
    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const { return m_backgroundColor; }
    void setLineWidth(int width);
    int lineWidth() const { return m_lineWidth; }
    
    // Text display
    void setShowText(bool show);
    bool showText() const { return m_showText; }
    void setTextFormat(const QString &format);
    QString textFormat() const { return m_textFormat; }
    
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

signals:
    void valueChanged(int value);

private:
    int m_value;
    int m_minimum;
    int m_maximum;
    QColor m_progressColor;
    QColor m_backgroundColor;
    int m_lineWidth;
    bool m_showText;
    QString m_textFormat;
    
    void paintCircularProgress(QPainter &painter);
    void paintProgressText(QPainter &painter);
    QRect getProgressRect() const;
    int getProgressAngle() const;
};

#endif // CUSTOMPROGRESSBAR_H