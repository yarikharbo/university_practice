#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QScrollBar>
#include <QSlider>
#include <QLabel>
#include "plotwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onApplyAll();
    void onChooseBgColor();
    void onChooseLineColor();
    void onApplyFunctionSettings();
    void onApplyFontSettings();
    void onZoomSliderChanged(int value);
    void onHPanChanged(int value);
    void onVPanChanged(int value);
    void updateScrollBars();

private:
    void setupUI();
    void connectSignals();

    PlotWidget *m_plot;

    QLineEdit *m_xMinEdit, *m_xMaxEdit, *m_yMinEdit, *m_yMaxEdit;
    QSpinBox  *m_widthSpin, *m_heightSpin;
    QPushButton *m_bgColorBtn;
    QColor m_bgColor;

    QComboBox *m_funcCombo;
    QPushButton *m_lineColorBtn;
    QColor m_lineColor;
    QComboBox *m_styleCombo;
    QSpinBox  *m_thicknessSpin;

    QComboBox *m_fontCombo;
    QSpinBox *m_fontSizeSpin;
    QPushButton *m_applyFontBtn;

    QScrollBar *m_hScroll, *m_vScroll;
    QSlider *m_zoomSlider;
    QLabel *m_zoomLabel;
    QLabel *m_statusLabel;
};

#endif