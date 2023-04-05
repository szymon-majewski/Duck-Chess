#include "mainwindow.h"
#include "FenParser.h"
#include "Session.h"
#include "MovesGenerator.h"
#include "Engine.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Session session = Session(Position());
    FenParser fenParser;
    MovesGenerator movesGenerator;
    Engine engine;

    MainWindow w(nullptr, &session, &fenParser, &movesGenerator, &engine);

    w.show();

    return a.exec();
}
