#include "mainwindow.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QColorDialog>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_bgColor(Qt::white), m_lineColor(Qt::red)
{
    setupUI();
    connectSignals();
    onApplyAll();
}

void MainWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    QGridLayout *mainLayout = new QGridLayout(central);

    QGroupBox *rangeGroup = new QGroupBox("Диапазон и поле");
    QGridLayout *rangeLayout = new QGridLayout(rangeGroup);
    rangeLayout->addWidget(new QLabel("X min:"), 0, 0);
    m_xMinEdit = new QLineEdit("-5");
    rangeLayout->addWidget(m_xMinEdit, 0, 1);
    rangeLayout->addWidget(new QLabel("X max:"), 0, 2);
    m_xMaxEdit = new QLineEdit("5");
    rangeLayout->addWidget(m_xMaxEdit, 0, 3);
    rangeLayout->addWidget(new QLabel("Y min:"), 1, 0);
    m_yMinEdit = new QLineEdit("-5");
    rangeLayout->addWidget(m_yMinEdit, 1, 1);
    rangeLayout->addWidget(new QLabel("Y max:"), 1, 2);
    m_yMaxEdit = new QLineEdit("5");
    rangeLayout->addWidget(m_yMaxEdit, 1, 3);
    rangeLayout->addWidget(new QLabel("Ширина поля:"), 2, 0);
    m_widthSpin = new QSpinBox; m_widthSpin->setRange(200, 2000); m_widthSpin->setValue(600);
    rangeLayout->addWidget(m_widthSpin, 2, 1);
    rangeLayout->addWidget(new QLabel("Высота поля:"), 2, 2);
    m_heightSpin = new QSpinBox; m_heightSpin->setRange(150, 1500); m_heightSpin->setValue(400);
    rangeLayout->addWidget(m_heightSpin, 2, 3);
    m_bgColorBtn = new QPushButton("Цвет фона");
    m_bgColorBtn->setStyleSheet("background-color: white");
    rangeLayout->addWidget(m_bgColorBtn, 3, 0, 1, 2);
    QPushButton *applyAllBtn = new QPushButton("Применить все настройки");
    rangeLayout->addWidget(applyAllBtn, 3, 2, 1, 2);
    mainLayout->addWidget(rangeGroup, 0, 0);

    QGroupBox *funcGroup = new QGroupBox("Настройка функции");
    QGridLayout *funcLayout = new QGridLayout(funcGroup);
    funcLayout->addWidget(new QLabel("Функция:"), 0, 0);
    m_funcCombo = new QComboBox;
    m_funcCombo->addItems({"sinh", "cosh", "tanh"});
    funcLayout->addWidget(m_funcCombo, 0, 1);
    m_lineColorBtn = new QPushButton("Цвет линии");
    m_lineColorBtn->setStyleSheet("background-color: red");
    funcLayout->addWidget(m_lineColorBtn, 1, 0, 1, 2);
    funcLayout->addWidget(new QLabel("Стиль:"), 2, 0);
    m_styleCombo = new QComboBox;
    m_styleCombo->addItem("Сплошная", QVariant::fromValue<int>(Qt::SolidLine));
    m_styleCombo->addItem("Пунктир", QVariant::fromValue<int>(Qt::DashLine));
    m_styleCombo->addItem("Точки", QVariant::fromValue<int>(Qt::DotLine));
    funcLayout->addWidget(m_styleCombo, 2, 1);
    funcLayout->addWidget(new QLabel("Толщина:"), 3, 0);
    m_thicknessSpin = new QSpinBox; m_thicknessSpin->setRange(1, 10); m_thicknessSpin->setValue(2);
    funcLayout->addWidget(m_thicknessSpin, 3, 1);
    QPushButton *applyFuncBtn = new QPushButton("Применить настройки функции");
    funcLayout->addWidget(applyFuncBtn, 4, 0, 1, 2);
    mainLayout->addWidget(funcGroup, 1, 0);

    QGroupBox *fontGroup = new QGroupBox("Шрифт");
    QGridLayout *fontLayout = new QGridLayout(fontGroup);
    fontLayout->addWidget(new QLabel("Семейство:"), 0, 0);
    m_fontCombo = new QComboBox;
    m_fontCombo->addItems({"Arial", "Times New Roman", "Courier New"});
    fontLayout->addWidget(m_fontCombo, 0, 1);
    fontLayout->addWidget(new QLabel("Размер:"), 1, 0);
    m_fontSizeSpin = new QSpinBox; m_fontSizeSpin->setRange(6, 24); m_fontSizeSpin->setValue(10);
    fontLayout->addWidget(m_fontSizeSpin, 1, 1);
    m_applyFontBtn = new QPushButton("Применить настройки шрифта");
    fontLayout->addWidget(m_applyFontBtn, 2, 0, 1, 2);
    mainLayout->addWidget(fontGroup, 2, 0);

    m_plot = new PlotWidget;
    mainLayout->addWidget(m_plot, 0, 1, 4, 1);

    m_zoomLabel = new QLabel("Зум: 1.0x");
    m_zoomSlider = new QSlider(Qt::Horizontal);
    m_zoomSlider->setRange(20, 200);
    m_zoomSlider->setValue(100);
    mainLayout->addWidget(m_zoomLabel, 4, 0);
    mainLayout->addWidget(m_zoomSlider, 4, 1);

    m_hScroll = new QScrollBar(Qt::Horizontal);
    m_hScroll->setRange(0, 1000);
    mainLayout->addWidget(m_hScroll, 5, 1);

    m_vScroll = new QScrollBar(Qt::Vertical);
    m_vScroll->setRange(0, 1000);
    mainLayout->addWidget(m_vScroll, 0, 2, 5, 1);

    m_statusLabel = new QLabel("Готово");
    statusBar()->addWidget(m_statusLabel);

    setCentralWidget(central);
    setWindowTitle("Графики гиперболических функций");

    connect(applyAllBtn, &QPushButton::clicked, this, &MainWindow::onApplyAll);
    connect(m_bgColorBtn, &QPushButton::clicked, this, &MainWindow::onChooseBgColor);
    connect(m_lineColorBtn, &QPushButton::clicked, this, &MainWindow::onChooseLineColor);
    connect(applyFuncBtn, &QPushButton::clicked, this, &MainWindow::onApplyFunctionSettings);
    connect(m_applyFontBtn, &QPushButton::clicked, this, &MainWindow::onApplyFontSettings);
}

void MainWindow::connectSignals()
{
    connect(m_zoomSlider, &QSlider::valueChanged, this, &MainWindow::onZoomSliderChanged);
    connect(m_hScroll, &QScrollBar::valueChanged, this, &MainWindow::onHPanChanged);
    connect(m_vScroll, &QScrollBar::valueChanged, this, &MainWindow::onVPanChanged);
    connect(m_plot, &PlotWidget::visibleRangeChanged, this, &MainWindow::updateScrollBars);
    connect(m_plot, &PlotWidget::statusMessage, m_statusLabel, &QLabel::setText);
}

void MainWindow::onApplyAll()
{
    bool okXMin, okXMax, okYMin, okYMax;
    double xMin = m_xMinEdit->text().toDouble(&okXMin);
    double xMax = m_xMaxEdit->text().toDouble(&okXMax);
    double yMin = m_yMinEdit->text().toDouble(&okYMin);
    double yMax = m_yMaxEdit->text().toDouble(&okYMax);
    if (okXMin && okXMax && okYMin && okYMax) {
        m_plot->setXRange(xMin, xMax);
        m_plot->setYRange(yMin, yMax);
    }
    m_plot->setPlotSize(m_widthSpin->value(), m_heightSpin->value());
    m_plot->setBackgroundColor(m_bgColor);
    onApplyFunctionSettings();
    onApplyFontSettings();
}

void MainWindow::onChooseBgColor()
{
    QColor col = QColorDialog::getColor(m_bgColor, this, "Цвет фона");
    if (col.isValid()) {
        m_bgColor = col;
        m_bgColorBtn->setStyleSheet(QString("background-color: %1").arg(col.name()));
        m_plot->setBackgroundColor(col);
    }
}

void MainWindow::onChooseLineColor()
{
    QColor col = QColorDialog::getColor(m_lineColor, this, "Цвет линии");
    if (col.isValid()) {
        m_lineColor = col;
        m_lineColorBtn->setStyleSheet(QString("background-color: %1").arg(col.name()));
    }
}

void MainWindow::onApplyFunctionSettings()
{
    int idx = m_funcCombo->currentIndex();
    if (idx < 0) return;
    Qt::PenStyle style = static_cast<Qt::PenStyle>(m_styleCombo->currentData().toInt());
    int thick = m_thicknessSpin->value();
    m_plot->setFunctionConfig(idx, m_lineColor, style, thick);
}

void MainWindow::onApplyFontSettings()
{
    QString family = m_fontCombo->currentText();
    int size = m_fontSizeSpin->value();
    QFont font(family, size);
    m_plot->setFontSettings(font, size);
}

void MainWindow::onZoomSliderChanged(int value)
{
    double factor = value / 100.0;
    m_plot->setZoomFactor(factor);
    m_zoomLabel->setText(QString("Зум: %1x").arg(factor, 0, 'f', 1));
}

void MainWindow::onHPanChanged(int value)
{
    double range = m_plot->visibleXRange();
    double totalSpan = m_plot->xMaxGlobal() - m_plot->xMinGlobal() - range;
    if (totalSpan > 0) {
        double offset = m_plot->xMinGlobal() + (value / 1000.0) * totalSpan;
        m_plot->setPanOffsets(offset, m_plot->yMinVisible());
    }
}

void MainWindow::onVPanChanged(int value)
{
    double range = m_plot->visibleYRange();
    double totalSpan = m_plot->yMaxGlobal() - m_plot->yMinGlobal() - range;
    if (totalSpan > 0) {
        double offset = m_plot->yMinGlobal() + (value / 1000.0) * totalSpan;
        m_plot->setPanOffsets(m_plot->xMinVisible(), offset);
    }
}

void MainWindow::updateScrollBars()
{
    double xMinV = m_plot->xMinVisible();
    double xRange = m_plot->visibleXRange();
    double xTotal = m_plot->xMaxGlobal() - m_plot->xMinGlobal() - xRange;
    if (xTotal > 0) {
        int hVal = static_cast<int>((xMinV - m_plot->xMinGlobal()) / xTotal * 1000);
        m_hScroll->blockSignals(true);
        m_hScroll->setValue(hVal);
        m_hScroll->blockSignals(false);
    } else {
        m_hScroll->blockSignals(true);
        m_hScroll->setValue(500);
        m_hScroll->blockSignals(false);
    }

    double yMinV = m_plot->yMinVisible();
    double yRange = m_plot->visibleYRange();
    double yTotal = m_plot->yMaxGlobal() - m_plot->yMinGlobal() - yRange;
    if (yTotal > 0) {
        int vVal = static_cast<int>((yMinV - m_plot->yMinGlobal()) / yTotal * 1000);
        m_vScroll->blockSignals(true);
        m_vScroll->setValue(vVal);
        m_vScroll->blockSignals(false);
    } else {
        m_vScroll->blockSignals(true);
        m_vScroll->setValue(500);
        m_vScroll->blockSignals(false);
    }
}