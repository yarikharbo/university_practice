#include "plotwidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <cmath>

static double sinhFunc(double x) { return std::sinh(x); }
static double coshFunc(double x) { return std::cosh(x); }
static double tanhFunc(double x) { return std::tanh(x); }

PlotWidget::PlotWidget(QWidget *parent)
    : QWidget(parent)
{
    functions.append(FunctionConfig("sinh", sinhFunc, Qt::red,    Qt::SolidLine, 2));
    functions.append(FunctionConfig("cosh", coshFunc, Qt::green,  Qt::SolidLine, 2));
    functions.append(FunctionConfig("tanh", tanhFunc, Qt::blue,   Qt::SolidLine, 2));

    m_xMin = -5.0; m_xMax = 5.0;
    m_yMin = -5.0; m_yMax = 5.0;
    m_plotWidth = 600; m_plotHeight = 400;
    m_marginLeft = 70; m_marginTop = 20; m_marginRight = 30; m_marginBottom = 50;

    m_bgColor = Qt::white;
    m_font = QFont("Arial", 10);
    m_fontSize = 10;
    m_gridVisible = true;

    m_zoomFactor = 1.0;
    m_xPan = m_xMin;
    m_yPan = m_yMin;
    m_panning = false;

    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedSize(m_plotWidth + m_marginLeft + m_marginRight,
                 m_plotHeight + m_marginTop + m_marginBottom);
}

QSize PlotWidget::sizeHint() const
{
    return QSize(m_plotWidth + m_marginLeft + m_marginRight,
                 m_plotHeight + m_marginTop + m_marginBottom);
}

void PlotWidget::setXRange(double min, double max)
{
    if (min < max) {
        m_xMin = min; m_xMax = max;
        if (m_xPan < m_xMin) m_xPan = m_xMin;
        clampPan();
        emit visibleRangeChanged();
        update();
    }
}

void PlotWidget::setYRange(double min, double max)
{
    if (min < max) {
        m_yMin = min; m_yMax = max;
        if (m_yPan < m_yMin) m_yPan = m_yMin;
        clampPan();
        emit visibleRangeChanged();
        update();
    }
}

void PlotWidget::setPlotSize(int w, int h)
{
    m_plotWidth = w; m_plotHeight = h;
    setFixedSize(w + m_marginLeft + m_marginRight,
                 h + m_marginTop + m_marginBottom);
    clampPan();
    emit visibleRangeChanged();
    update();
}

void PlotWidget::setBackgroundColor(const QColor &color)
{
    m_bgColor = color;
    update();
}

void PlotWidget::setFontSettings(const QFont &font, int fontSize)
{
    m_font = font;
    m_fontSize = fontSize;
    m_font.setPointSize(fontSize);
    update();
}

void PlotWidget::setGridVisible(bool vis)
{
    m_gridVisible = vis;
    update();
}

void PlotWidget::setFunctionConfig(int index, const QColor &color, Qt::PenStyle style, int thickness)
{
    if (index >= 0 && index < functions.size()) {
        functions[index].color = color;
        functions[index].style = style;
        functions[index].thickness = thickness;
        update();
    }
}

void PlotWidget::setFunctionEnabled(int index, bool enabled)
{
    if (index >= 0 && index < functions.size()) {
        functions[index].enabled = enabled;
        update();
    }
}

void PlotWidget::setZoomFactor(double factor)
{
    if (factor <= 0.0) return;
    double oldW = visibleXRange();
    double oldH = visibleYRange();
    m_zoomFactor = factor;
    double cx = m_xPan + oldW/2.0;
    double cy = m_yPan + oldH/2.0;
    double newW = visibleXRange();
    double newH = visibleYRange();
    m_xPan = cx - newW/2.0;
    m_yPan = cy - newH/2.0;
    clampPan();
    emit visibleRangeChanged();
    update();
}

void PlotWidget::setPanOffsets(double xMinVisible, double yMinVisible)
{
    m_xPan = xMinVisible;
    m_yPan = yMinVisible;
    clampPan();
    update();
}

double PlotWidget::visibleXRange() const
{
    return (m_xMax - m_xMin) / m_zoomFactor;
}

double PlotWidget::visibleYRange() const
{
    return (m_yMax - m_yMin) / m_zoomFactor;
}

double PlotWidget::xMinVisible() const { return m_xPan; }
double PlotWidget::yMinVisible() const { return m_yPan; }

double PlotWidget::xMinGlobal() const { return m_xMin; }
double PlotWidget::xMaxGlobal() const { return m_xMax; }
double PlotWidget::yMinGlobal() const { return m_yMin; }
double PlotWidget::yMaxGlobal() const { return m_yMax; }

void PlotWidget::clampPan()
{
    double w = visibleXRange();
    double h = visibleYRange();
    double totalW = m_xMax - m_xMin;
    double totalH = m_yMax - m_yMin;

    if (w > totalW) {
        m_xPan = m_xMin - (w - totalW) / 2.0;
    } else {
        if (m_xPan < m_xMin) m_xPan = m_xMin;
        if (m_xPan + w > m_xMax) m_xPan = m_xMax - w;
    }

    if (h > totalH) {
        m_yPan = m_yMin - (h - totalH) / 2.0;
    } else {
        if (m_yPan < m_yMin) m_yPan = m_yMin;
        if (m_yPan + h > m_yMax) m_yPan = m_yMax - h;
    }
}

QPointF PlotWidget::worldToPixel(double x, double y) const
{
    double w = visibleXRange();
    double h = visibleYRange();
    double px = m_marginLeft + (x - m_xPan) / w * m_plotWidth;
    double py = m_marginTop + (1.0 - (y - m_yPan) / h) * m_plotHeight;
    return QPointF(px, py);
}

double PlotWidget::pixelToWorldX(int px) const
{
    double w = visibleXRange();
    return m_xPan + (px - m_marginLeft) / (double)m_plotWidth * w;
}

double PlotWidget::pixelToWorldY(int py) const
{
    double h = visibleYRange();
    return m_yPan + (1.0 - (py - m_marginTop) / (double)m_plotHeight) * h;
}

void PlotWidget::applyFont(QPainter &painter)
{
    QFont f = m_font;
    f.setPointSize(m_fontSize);
    painter.setFont(f);
}

void PlotWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QRect plotRect(m_marginLeft, m_marginTop, m_plotWidth, m_plotHeight);
    painter.fillRect(plotRect, m_bgColor);
    painter.setPen(Qt::black);
    painter.drawRect(plotRect);

    if (m_gridVisible)
        drawGrid(painter);

    drawAxes(painter);

    for (const auto &cfg : functions) {
        if (cfg.enabled)
            drawFunction(painter, cfg);
    }
}

void PlotWidget::drawGrid(QPainter &painter)
{
    painter.save();
    QPen pen(Qt::lightGray, 1, Qt::DotLine);
    painter.setPen(pen);

    double stepX = visibleXRange() / 10.0;
    double startX = std::ceil(m_xPan / stepX) * stepX;
    for (double x = startX; x <= m_xPan + visibleXRange(); x += stepX) {
        QPointF p = worldToPixel(x, 0);
        painter.drawLine(QPointF(p.x(), m_marginTop), QPointF(p.x(), m_marginTop + m_plotHeight));
    }
    double stepY = visibleYRange() / 10.0;
    double startY = std::ceil(m_yPan / stepY) * stepY;
    for (double y = startY; y <= m_yPan + visibleYRange(); y += stepY) {
        QPointF p = worldToPixel(0, y);
        painter.drawLine(QPointF(m_marginLeft, p.y()), QPointF(m_marginLeft + m_plotWidth, p.y()));
    }
    painter.restore();
}

void PlotWidget::drawAxes(QPainter &painter)
{
    painter.save();
    applyFont(painter);
    QPen axisPen(Qt::black, 1);
    painter.setPen(axisPen);

    bool drawXAxis = false, drawYAxis = false;
    QPointF origin = worldToPixel(0, 0);
    if (m_yPan <= 0 && m_yPan + visibleYRange() >= 0) {
        if (origin.y() >= m_marginTop && origin.y() <= m_marginTop + m_plotHeight) {
            painter.drawLine(QPointF(m_marginLeft, origin.y()), QPointF(m_marginLeft + m_plotWidth, origin.y()));
            drawXAxis = true;
        }
    }
    if (m_xPan <= 0 && m_xPan + visibleXRange() >= 0) {
        if (origin.x() >= m_marginLeft && origin.x() <= m_marginLeft + m_plotWidth) {
            painter.drawLine(QPointF(origin.x(), m_marginTop), QPointF(origin.x(), m_marginTop + m_plotHeight));
            drawYAxis = true;
        }
    }

    if (drawXAxis) {
        QPointF arrowEnd(m_marginLeft + m_plotWidth, origin.y());
        QPointF arrowP1 = arrowEnd + QPointF(-8, -4);
        QPointF arrowP2 = arrowEnd + QPointF(-8,  4);
        QPolygonF arrowHead;
        arrowHead << arrowEnd << arrowP1 << arrowP2;
        painter.setBrush(Qt::black);
        painter.drawPolygon(arrowHead);
    }
    if (drawYAxis) {
        QPointF arrowEnd(origin.x(), m_marginTop);
        QPointF arrowP1 = arrowEnd + QPointF(-4, 8);
        QPointF arrowP2 = arrowEnd + QPointF( 4, 8);
        QPolygonF arrowHead;
        arrowHead << arrowEnd << arrowP1 << arrowP2;
        painter.setBrush(Qt::black);
        painter.drawPolygon(arrowHead);
    }

    double stepX = visibleXRange() / 10.0;
    double startX = std::ceil(m_xPan / stepX) * stepX;
    for (double x = startX; x <= m_xPan + visibleXRange(); x += stepX) {
        QPointF p = worldToPixel(x, 0);
        double tickY = drawXAxis ? origin.y() : m_marginTop + m_plotHeight;
        painter.drawLine(QPointF(p.x(), tickY - 3), QPointF(p.x(), tickY + 3));
        painter.drawText(QRectF(p.x() - 25, tickY + 5, 50, 20),
                         Qt::AlignHCenter | Qt::AlignTop, QString::number(x, 'f', 1));
    }

    double stepY = visibleYRange() / 10.0;
    double startY = std::ceil(m_yPan / stepY) * stepY;
    for (double y = startY; y <= m_yPan + visibleYRange(); y += stepY) {
        QPointF p = worldToPixel(0, y);
        double tickX = drawYAxis ? origin.x() : m_marginLeft;
        painter.drawLine(QPointF(tickX - 3, p.y()), QPointF(tickX + 3, p.y()));
        painter.drawText(QRectF(tickX - 65, p.y() - 10, 60, 20),
                         Qt::AlignRight | Qt::AlignVCenter, QString::number(y, 'f', 1));
    }

    if (drawXAxis) {
        painter.drawText(QRectF(m_marginLeft + m_plotWidth - 15, origin.y() - 25, 30, 20),
                         Qt::AlignCenter, "X");
    }
    if (drawYAxis) {
        painter.drawText(QRectF(origin.x() - 25, m_marginTop - 5, 30, 20),
                         Qt::AlignCenter, "Y");
    }

    painter.restore();
}

void PlotWidget::drawFunction(QPainter &painter, const FunctionConfig &cfg)
{
    painter.save();
    QPen pen(cfg.color, cfg.thickness, cfg.style);
    painter.setPen(pen);

    double step = visibleXRange() / (m_plotWidth * 2);
    bool first = true;
    QPointF prevPt;
    for (double x = m_xPan; x <= m_xPan + visibleXRange(); x += step) {
        double y = cfg.func(x);
        if (std::isnan(y) || std::isinf(y)) { first = true; continue; }
        QPointF pt = worldToPixel(x, y);
        if (first) {
            prevPt = pt;
            first = false;
        } else {
            painter.drawLine(prevPt, pt);
            prevPt = pt;
        }
    }
    painter.restore();
}

void PlotWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_panning = true;
        m_lastPanPos = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
    QWidget::mousePressEvent(event);
}

void PlotWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_panning) {
        QPoint delta = event->pos() - m_lastPanPos;
        m_lastPanPos = event->pos();
        double dx = -delta.x() * visibleXRange() / m_plotWidth;
        double dy =  delta.y() * visibleYRange() / m_plotHeight;
        m_xPan += dx;
        m_yPan += dy;
        clampPan();
        emit visibleRangeChanged();
        update();
    }
    double wx = pixelToWorldX(static_cast<int>(event->position().x()));
    double wy = pixelToWorldY(static_cast<int>(event->position().y()));
    emit statusMessage(QString("x: %1, y: %2").arg(wx, 0, 'f', 2).arg(wy, 0, 'f', 2));
    QWidget::mouseMoveEvent(event);
}

void PlotWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_panning = false;
        setCursor(Qt::ArrowCursor);
    }
    QWidget::mouseReleaseEvent(event);
}

void PlotWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    QAction *resetAct = menu.addAction("Сброс вида");
    QAction *gridAct = menu.addAction(m_gridVisible ? "Скрыть сетку" : "Показать сетку");
    QAction *selected = menu.exec(event->globalPos());
    if (selected == resetAct) {
        m_zoomFactor = 1.0;
        m_xPan = m_xMin;
        m_yPan = m_yMin;
        clampPan();
        emit visibleRangeChanged();
        update();
    } else if (selected == gridAct) {
        m_gridVisible = !m_gridVisible;
        update();
    }
}

void PlotWidget::keyPressEvent(QKeyEvent *event)
{
    double stepX = visibleXRange() * 0.1;
    double stepY = visibleYRange() * 0.1;
    switch (event->key()) {
    case Qt::Key_Left:  m_xPan -= stepX; break;
    case Qt::Key_Right: m_xPan += stepX; break;
    case Qt::Key_Up:    m_yPan += stepY; break;
    case Qt::Key_Down:  m_yPan -= stepY; break;
    case Qt::Key_Plus:
    case Qt::Key_Equal:
        setZoomFactor(m_zoomFactor * 1.2);
        return;
    case Qt::Key_Minus:
        setZoomFactor(m_zoomFactor / 1.2);
        return;
    case Qt::Key_R:
        m_zoomFactor = 1.0;
        m_xPan = m_xMin;
        m_yPan = m_yMin;
        clampPan();
        emit visibleRangeChanged();
        update();
        return;
    default:
        QWidget::keyPressEvent(event);
        return;
    }
    clampPan();
    emit visibleRangeChanged();
    update();
}