#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.setWindowTitle("Туристические поездки");
    window.resize(900, 600);
    window.show();
    return app.exec();
}