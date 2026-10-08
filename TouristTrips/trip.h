#ifndef TRIP_H
#define TRIP_H

#include <QString>

struct Trip {
    qreal cost = 0.0;
    QString name;
    QString date;
    QString imagePath;
};

#endif