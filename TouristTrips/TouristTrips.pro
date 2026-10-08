QT += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TouristTrips
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h \
    trip.h \
    quicksort.h

DISTFILES += \
    data/berlin.jpg \
    data/london.jpg \
    data/paris.jpg \
    data/trips.txt