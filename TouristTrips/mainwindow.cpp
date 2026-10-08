#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTableWidget>
#include <QListWidget>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <QKeyEvent>
#include <QFile>
#include <QTextStream>
#include <QPixmap>
#include <QIcon>
#include <QDate>
#include <QApplication>

#include "quicksort.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();

    connect(m_loadBtn, &QPushButton::clicked, this, &MainWindow::loadFromFile);
    connect(m_addBtn, &QPushButton::clicked, this, &MainWindow::addTrip);
    connect(m_delBtn, &QPushButton::clicked, this, &MainWindow::deleteSelectedTrip);
    connect(m_quickSortTableBtn, &QPushButton::clicked, this, &MainWindow::sortTableQuick);

    connect(m_table, &QTableWidget::itemSelectionChanged, this, &MainWindow::onTableSelectionChanged);
    m_table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_table, &QTableWidget::customContextMenuRequested, this, &MainWindow::onTableContextMenu);

    m_list->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_list, &QListWidget::customContextMenuRequested, this, &MainWindow::onListContextMenu);

    connect(m_addListBtn, &QPushButton::clicked, this, &MainWindow::addListItem);
    connect(m_delListBtn, &QPushButton::clicked, this, &MainWindow::deleteSelectedListItem);
    connect(m_sortListStdAsc, &QPushButton::clicked, this, &MainWindow::sortListStandardAsc);
    connect(m_sortListStdDesc, &QPushButton::clicked, this, &MainWindow::sortListStandardDesc);
    connect(m_sortListQuickAsc, &QPushButton::clicked, this, &MainWindow::sortListQuickAsc);
    connect(m_sortListQuickDesc, &QPushButton::clicked, this, &MainWindow::sortListQuickDesc);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(new QLabel("Размер:"));
    m_sizeEdit = new QLineEdit("10");
    m_sizeEdit->setMaximumWidth(50);
    topLayout->addWidget(m_sizeEdit);

    m_loadBtn = new QPushButton("Загрузить файл");
    m_addBtn = new QPushButton("Добавить");
    m_delBtn = new QPushButton("Удалить");
    topLayout->addWidget(m_loadBtn);
    topLayout->addWidget(m_addBtn);
    topLayout->addWidget(m_delBtn);

    topLayout->addStretch();
    topLayout->addWidget(new QLabel("Сорт. по:"));
    m_sortColumnCombo = new QComboBox();
    m_sortColumnCombo->addItems({"Стоимость", "Название", "Дата"});
    topLayout->addWidget(m_sortColumnCombo);
    m_quickSortTableBtn = new QPushButton("Хоар-сортировка таблицы");
    topLayout->addWidget(m_quickSortTableBtn);

    mainLayout->addLayout(topLayout);

    // Таблица
    m_table = new QTableWidget(0, 4);
    m_table->setHorizontalHeaderLabels({"Стоимость", "Название", "Дата", "Картинка"});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->setVisible(false);
    m_table->setIconSize(QSize(64, 48));
    mainLayout->addWidget(m_table, 1);

    // картинка + список
    QHBoxLayout *bottomLayout = new QHBoxLayout();

    m_imageLabel = new QLabel();
    m_imageLabel->setFixedSize(200, 150);
    m_imageLabel->setFrameStyle(QFrame::Box);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setText("Фото поездки");
    bottomLayout->addWidget(m_imageLabel);

    // Блок списка
    QVBoxLayout *listLayout = new QVBoxLayout();
    QLabel *listLabel = new QLabel("Список поездок:");
    m_list = new QListWidget();
    m_list->setIconSize(QSize(32, 24));

    QHBoxLayout *listBtnLayout = new QHBoxLayout();
    m_addListBtn = new QPushButton("+");
    m_delListBtn = new QPushButton("-");
    listBtnLayout->addWidget(m_addListBtn);
    listBtnLayout->addWidget(m_delListBtn);
    listBtnLayout->addStretch();

    QGridLayout *listSortLayout = new QGridLayout();
    m_sortListStdAsc = new QPushButton("Станд ▲");
    m_sortListStdDesc = new QPushButton("Станд ▼");
    m_sortListQuickAsc = new QPushButton("Хоар ▲");
    m_sortListQuickDesc = new QPushButton("Хоар ▼");
    listSortLayout->addWidget(m_sortListStdAsc, 0, 0);
    listSortLayout->addWidget(m_sortListStdDesc, 0, 1);
    listSortLayout->addWidget(m_sortListQuickAsc, 1, 0);
    listSortLayout->addWidget(m_sortListQuickDesc, 1, 1);

    listLayout->addWidget(listLabel);
    listLayout->addWidget(m_list);
    listLayout->addLayout(listBtnLayout);
    listLayout->addLayout(listSortLayout);

    bottomLayout->addLayout(listLayout, 1);
    mainLayout->addLayout(bottomLayout);
}

void MainWindow::refreshTable() {
    m_table->setRowCount(0);
    for (const Trip &trip : m_trips) {
        int row = m_table->rowCount();
        m_table->insertRow(row);

        QTableWidgetItem *costItem = new QTableWidgetItem(QString::number(trip.cost, 'f', 2));
        costItem->setData(Qt::UserRole, trip.cost);
        m_table->setItem(row, 0, costItem);

        m_table->setItem(row, 1, new QTableWidgetItem(trip.name));
        m_table->setItem(row, 2, new QTableWidgetItem(trip.date));

        QTableWidgetItem *imgItem = new QTableWidgetItem();
        if (!trip.imagePath.isEmpty()) {
            QPixmap pix(trip.imagePath);
            if (!pix.isNull())
                imgItem->setIcon(QIcon(pix.scaled(64, 48, Qt::KeepAspectRatio)));
            else
                imgItem->setText("нет");
        } else {
            imgItem->setText("нет");
        }
        m_table->setItem(row, 3, imgItem);
    }
    m_table->resizeColumnsToContents();
}

void MainWindow::refreshList() {
    m_list->clear();
    for (const Trip &trip : m_trips) {
        QListWidgetItem *item = new QListWidgetItem(trip.name);
        if (!trip.imagePath.isEmpty()) {
            QPixmap pix(trip.imagePath);
            if (!pix.isNull())
                item->setIcon(QIcon(pix.scaled(32, 24, Qt::KeepAspectRatio)));
        }
        m_list->addItem(item);
    }
}

void MainWindow::updateImageLabel(const Trip &trip) {
    qDebug() << "updateImageLabel called for:" << trip.name;
    if (trip.imagePath.isEmpty()) {
        m_imageLabel->setText("Нет изображения");
        qDebug() << "Image path is empty";
        return;
    }
    qDebug() << "Attempting to load:" << trip.imagePath;
    bool exists = QFile::exists(trip.imagePath);
    qDebug() << "File exists?" << exists;
    QPixmap pix(trip.imagePath);
    if (pix.isNull()) {
        m_imageLabel->setText("Ошибка загрузки");
        qDebug() << "Pixmap is null (format not supported or corrupt?)";
        return;
    }
    m_imageLabel->setPixmap(pix.scaled(m_imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::loadFromFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Открыть trips.txt", "", "Текстовые файлы (*.txt)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл");
        return;
    }

    bool ok;
    int maxLines = m_sizeEdit->text().toInt(&ok);
    if (!ok || maxLines <= 0) maxLines = INT_MAX;

    m_trips.clear();
    QTextStream in(&file);
    int count = 0;
    while (!in.atEnd() && count < maxLines) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;
        QStringList parts = line.split(';');
        if (parts.size() < 3) continue;
        Trip trip;
        trip.cost = parts[0].toDouble();
        trip.name = parts[1];
        trip.date = parts[2];
        trip.imagePath = (parts.size() > 3) ? parts[3] : "";
        m_trips.append(trip);
        ++count;
    }
    file.close();
    refreshTable();
    refreshList();
}

void MainWindow::addTrip() {
    Trip trip;
    trip.cost = 0.0;
    trip.name = "Новая поездка";
    trip.date = QDate::currentDate().toString("yyyy-MM-dd");
    trip.imagePath = "";
    m_trips.append(trip);
    refreshTable();
    refreshList();
}

void MainWindow::deleteSelectedTrip() {
    int row = m_table->currentRow();
    if (row >= 0 && row < m_trips.size()) {
        m_trips.removeAt(row);
        refreshTable();
        refreshList();
    }
}

void MainWindow::onTableSelectionChanged() {
    int row = m_table->currentRow();
    if (row >= 0 && row < m_trips.size())
        updateImageLabel(m_trips[row]);
    else
        m_imageLabel->clear();
}

void MainWindow::onTableContextMenu(const QPoint &pos) {
    QMenu menu(this);
    QAction *addAct = menu.addAction("Добавить поездку");
    QAction *delAct = menu.addAction("Удалить поездку");
    connect(addAct, &QAction::triggered, this, &MainWindow::addTrip);
    connect(delAct, &QAction::triggered, this, &MainWindow::deleteSelectedTrip);
    menu.exec(m_table->viewport()->mapToGlobal(pos));
}

void MainWindow::onListContextMenu(const QPoint &pos) {
    QMenu menu(this);
    QAction *addAct = menu.addAction("Добавить элемент");
    QAction *delAct = menu.addAction("Удалить элемент");
    connect(addAct, &QAction::triggered, this, &MainWindow::addListItem);
    connect(delAct, &QAction::triggered, this, &MainWindow::deleteSelectedListItem);
    menu.exec(m_list->viewport()->mapToGlobal(pos));
}

void MainWindow::addListItem() {
    m_list->addItem("Новый пункт");
}

void MainWindow::deleteSelectedListItem() {
    QListWidgetItem *item = m_list->currentItem();
    if (item)
        delete m_list->takeItem(m_list->row(item));
}

void MainWindow::sortListStandardAsc() {
    m_list->sortItems(Qt::AscendingOrder);
}

void MainWindow::sortListStandardDesc() {
    m_list->sortItems(Qt::DescendingOrder);
}

void MainWindow::sortListQuickAsc() {
    QVector<QPair<QString, QIcon>> items;
    items.reserve(m_list->count());
    for (int i = 0; i < m_list->count(); ++i) {
        QListWidgetItem *item = m_list->item(i);
        items.append(qMakePair(item->text(), item->icon()));
    }

    quickSort(items.begin(), items.end(),
              [](const QPair<QString, QIcon>& a, const QPair<QString, QIcon>& b) {
                  return a.first < b.first;
              });

    m_list->clear();
    for (const auto &pair : items) {
        QListWidgetItem *newItem = new QListWidgetItem(pair.second, pair.first);
        m_list->addItem(newItem);
    }
}

void MainWindow::sortListQuickDesc() {
    QVector<QPair<QString, QIcon>> items;
    items.reserve(m_list->count());
    for (int i = 0; i < m_list->count(); ++i) {
        QListWidgetItem *item = m_list->item(i);
        items.append(qMakePair(item->text(), item->icon()));
    }

    quickSort(items.begin(), items.end(),
              [](const QPair<QString, QIcon>& a, const QPair<QString, QIcon>& b) {
                  return b.first < a.first;
              });

    m_list->clear();
    for (const auto &pair : items) {
        QListWidgetItem *newItem = new QListWidgetItem(pair.second, pair.first);
        m_list->addItem(newItem);
    }
}

// Быстрая сортировка таблицы по выбранному полю
void MainWindow::sortTableQuick() {
    if (m_trips.empty()) return;

    int column = m_sortColumnCombo->currentIndex(); // 0 - стоимость, 1 - название, 2 - дата
    // Сортировка по возрастанию
    if (column == 0) {
        quickSort(m_trips.begin(), m_trips.end(),
                  [](const Trip& a, const Trip& b) { return a.cost < b.cost; });
    } else if (column == 1) {
        quickSort(m_trips.begin(), m_trips.end(),
                  [](const Trip& a, const Trip& b) { return a.name < b.name; });
    } else {
        quickSort(m_trips.begin(), m_trips.end(),
                  [](const Trip& a, const Trip& b) { return a.date < b.date; });
    }
    refreshTable();
    refreshList();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Delete) {
        if (m_table->hasFocus() && m_table->currentRow() >= 0)
            deleteSelectedTrip();
        else if (m_list->hasFocus() && m_list->currentItem())
            deleteSelectedListItem();
    } else if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_O) {
        loadFromFile();
    } else {
        QMainWindow::keyPressEvent(event);
    }
}