#include "mainwindow.h"
#include "FenParser.h"
#include "Session.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Session session = Session(Position());
    FenParser fenParser;

    MainWindow w(nullptr, &session, &fenParser);

    w.show();

    return a.exec();
}
