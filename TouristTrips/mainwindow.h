#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include "trip.h"

class QTableWidget;
class QListWidget;
class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void loadFromFile();
    void addTrip();
    void deleteSelectedTrip();
    void onTableSelectionChanged();
    void onTableContextMenu(const QPoint &pos);
    void onListContextMenu(const QPoint &pos);
    void addListItem();
    void deleteSelectedListItem();
    void sortListStandardAsc();
    void sortListStandardDesc();
    void sortListQuickAsc();
    void sortListQuickDesc();
    void sortTableQuick();

private:
    void setupUI();
    void refreshTable();
    void refreshList();
    void updateImageLabel(const Trip &trip);

    QVector<Trip> m_trips;

    QLineEdit   *m_sizeEdit;
    QTableWidget *m_table;
    QListWidget  *m_list;
    QLabel       *m_imageLabel;
    QComboBox    *m_sortColumnCombo;
    QPushButton  *m_loadBtn, *m_addBtn, *m_delBtn;
    QPushButton  *m_quickSortTableBtn;
    QPushButton  *m_addListBtn, *m_delListBtn;
    QPushButton  *m_sortListStdAsc, *m_sortListStdDesc;
    QPushButton  *m_sortListQuickAsc, *m_sortListQuickDesc;
};

#endif