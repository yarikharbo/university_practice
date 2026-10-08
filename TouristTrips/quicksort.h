#ifndef QUICKSORT_H
#define QUICKSORT_H

#include <iterator>
#include <QString>
#include <QStringList>

template <typename RandomIt, typename Compare>
void quickSort(RandomIt first, RandomIt last, Compare comp) {
    auto size = std::distance(first, last);
    if (size <= 1) return;
    auto pivot = *(first + size / 2);
    RandomIt i = first;
    RandomIt j = last - 1;
    while (i <= j) {
        while (comp(*i, pivot)) ++i;
        while (comp(pivot, *j)) --j;
        if (i <= j) {
            std::swap(*i, *j);
            ++i;
            --j;
        }
    }
    if (first < j + 1) quickSort(first, j + 1, comp);
    if (i < last)      quickSort(i, last, comp);
}

inline void quickSortStringList(QStringList& list, bool ascending) {
    auto comp = ascending
                    ? [](const QString& a, const QString& b) { return a < b; }
                    : [](const QString& a, const QString& b) { return b < a; };
    quickSort(list.begin(), list.end(), comp);
}

#endif