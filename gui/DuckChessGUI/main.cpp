#include "mainwindow.h"
#include "FenParser.h"
#include "Session.h"
#include "MovesGenerator.h"
#include "EngineWorker.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Session session = Session(Position());
    FenParser fenParser;
    MovesGenerator movesGenerator;
    EngineWorker engineWorker;

    MainWindow w(nullptr, &session, &fenParser, &movesGenerator, &engineWorker);

    w.show();

    return a.exec();
}
