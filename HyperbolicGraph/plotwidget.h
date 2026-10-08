#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>
#include <QPen>
#include <QFont>
#include <QList>
#include <QColor>

struct FunctionConfig {
    QString name;
    bool enabled;
    QColor color;
    Qt::PenStyle style;
    int thickness;
    double (*func)(double);
    FunctionConfig(const QString &n, double (*f)(double),
                   QColor c = Qt::black, Qt::PenStyle s = Qt::SolidLine, int t = 2)
        : name(n), enabled(true), color(c), style(s), thickness(t), func(f) {}
};

class PlotWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlotWidget(QWidget *parent = nullptr);

    void setXRange(double min, double max);
    void setYRange(double min, double max);
    void setPlotSize(int width, int height);
    void setBackgroundColor(const QColor &color);
    void setFontSettings(const QFont &font, int fontSize);
    void setGridVisible(bool vis);
    void setFunctionConfig(int index, const QColor &color, Qt::PenStyle style, int thickness);
    void setFunctionEnabled(int index, bool enabled);
    void setZoomFactor(double factor);
    void setPanOffsets(double xMinVisible, double yMinVisible);

    double visibleXRange() const;
    double visibleYRange() const;
    double xMinVisible() const;
    double yMinVisible() const;
    double xMinGlobal() const;
    double xMaxGlobal() const;
    double yMinGlobal() const;
    double yMaxGlobal() const;

signals:
    void statusMessage(const QString &msg);
    void visibleRangeChanged();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    QSize sizeHint() const override;

private:
    void clampPan();
    QPointF worldToPixel(double x, double y) const;
    double pixelToWorldX(int px) const;
    double pixelToWorldY(int py) const;
    void drawGrid(QPainter &painter);
    void drawAxes(QPainter &painter);
    void drawFunction(QPainter &painter, const FunctionConfig &cfg);
    void applyFont(QPainter &painter);

    QList<FunctionConfig> functions;
    double m_xMin, m_xMax, m_yMin, m_yMax;
    int m_plotWidth, m_plotHeight;
    int m_marginLeft, m_marginTop, m_marginRight, m_marginBottom;
    QColor m_bgColor;
    QFont m_font;
    int m_fontSize;
    bool m_gridVisible;

    double m_zoomFactor;
    double m_xPan, m_yPan;
    bool m_panning;
    QPoint m_lastPanPos;
};

#endif