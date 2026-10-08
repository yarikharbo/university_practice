#include <QtTest>
#include <QVector>
#include <QString>
#include "trip.h"
#include "quicksort.h"

class TestQuickSort : public QObject {
    Q_OBJECT

private slots:
    void testSortTripsByCost();
    void testSortTripsByName();
    void testSortStringListAscending();
    void testSortStringListDescending();
    void testEmptyContainer();
};

void TestQuickSort::testSortTripsByCost() {
    QVector<Trip> trips = {
        {300.0, "Париж", "2025-04-01", ""},
        {100.0, "Лондон", "2025-03-15", ""},
        {200.0, "Берлин", "2025-05-10", ""}
    };
    quickSort(trips.begin(), trips.end(),
              [](const Trip& a, const Trip& b) { return a.cost < b.cost; });

    QCOMPARE(trips[0].cost, 100.0);
    QCOMPARE(trips[1].cost, 200.0);
    QCOMPARE(trips[2].cost, 300.0);
    QCOMPARE(trips[0].name, QString("Лондон"));
    QCOMPARE(trips[2].name, QString("Париж"));
}

void TestQuickSort::testSortTripsByName() {
    QVector<Trip> trips = {
        {10, "Амстердам", "2025-01-01", ""},
        {20, "Вена", "2025-02-02", ""},
        {30, "Барселона", "2025-03-03", ""}
    };
    quickSort(trips.begin(), trips.end(),
              [](const Trip& a, const Trip& b) { return b.name < a.name; });

    QCOMPARE(trips[0].name, QString("Вена"));
    QCOMPARE(trips[1].name, QString("Барселона"));
    QCOMPARE(trips[2].name, QString("Амстердам"));
}

void TestQuickSort::testSortStringListAscending() {
    QStringList list = {"banana", "apple", "cherry"};
    quickSortStringList(list, true);
    QCOMPARE(list[0], QString("apple"));
    QCOMPARE(list[1], QString("banana"));
    QCOMPARE(list[2], QString("cherry"));
}

void TestQuickSort::testSortStringListDescending() {
    QStringList list = {"banana", "apple", "cherry"};
    quickSortStringList(list, false);
    QCOMPARE(list[0], QString("cherry"));
    QCOMPARE(list[1], QString("banana"));
    QCOMPARE(list[2], QString("apple"));
}

void TestQuickSort::testEmptyContainer() {
    QVector<int> empty;
    quickSort(empty.begin(), empty.end(), std::less<int>());
    QVERIFY(empty.isEmpty());

    QStringList emptyList;
    quickSortStringList(emptyList, true);
    QVERIFY(emptyList.isEmpty());
}

QTEST_MAIN(TestQuickSort)
#include "test_quicksort.moc"