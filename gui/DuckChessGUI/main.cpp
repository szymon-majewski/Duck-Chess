#include "mainwindow.h"
#include "FenParser.h"
#include "Session.h"
#include "MovesGenerator.h"

#include <QApplication>

/* TODO:
 * Some kind of bug with long ranged pieces - invisible walls
 * moves buttons
 * moves list
*/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Session session;
    FenParser fenParser;
    MovesGenerator movesGenerator;

    MainWindow w(nullptr, &session, &fenParser, &movesGenerator);

    w.show();

    return a.exec();
}
