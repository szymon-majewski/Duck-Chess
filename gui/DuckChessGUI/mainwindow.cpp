#include <QLabel>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Duck Chess Engine");

    // CHESSBOARD
    chessboardPanel = MainWindow::findChild<QGridLayout*>("chessboardPanel");

    for(int i=0; i<8; i++)
    {
        for(int j=0; j<8; j++)
        {
            QFrame *frame = new QFrame();
            if((i+j) % 2 == 0)
            {
                frame->setStyleSheet("background-color: #6db9e8");
            }
            else
            {
                frame->setStyleSheet("background-color: #2052a8");
            }

            frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            frame->setMinimumSize(QSize(70, 70));
            frame->setMaximumSize(QSize(70, 70));

            chessboardPanel->addWidget(frame, i, j);
        }
    }

    for (int i = 0; i < 8; i++)
    {
        chessboardPanel->setRowStretch(i, 1);
        chessboardPanel->setColumnStretch(i, 1);
    }

    //MOVES BUTTONS
    QPixmap forwardsPixmap(":/rsc/img/ui/forward.jpg");
    QPixmap fastForwardsPixmap(":/rsc/img/ui/fast_forward.jpg");
    QPixmap backwardsPixmap(":/rsc/img/ui/backward.jpg");
    QPixmap fastbBackwardsPixmap(":/rsc/img/ui/fast_backward.jpg");

    QPushButton* forwardsBtn = MainWindow::findChild<QPushButton*>("forwardsBtn");
    QPushButton* fastForwardsBtn = MainWindow::findChild<QPushButton*>("fastForwardsBtn");
    QPushButton* backwardsBtn = MainWindow::findChild<QPushButton*>("backwardsBtn");
    QPushButton* fastBackwardsBtn = MainWindow::findChild<QPushButton*>("fastBackwardsBtn");

    forwardsBtn->setIcon(QIcon(forwardsPixmap));
    fastForwardsBtn->setIcon(QIcon(fastForwardsPixmap));
    backwardsBtn->setIcon(QIcon(backwardsPixmap));
    fastBackwardsBtn->setIcon(QIcon(fastbBackwardsPixmap));

    //PIECES
    QPixmap whitePawnPixmap(":/rsc/img/pieces/white_pawn.png");
    QPixmap whiteKnightPixmap(":/rsc/img/pieces/white_knight.png");
    QPixmap whiteBishopPixmap(":/rsc/img/pieces/white_bishop.png");
    QPixmap whiteRookPixmap(":/rsc/img/pieces/white_rook.png");
    QPixmap whiteQueenPixmap(":/rsc/img/pieces/white_queen.png");
    QPixmap whiteKingPixmap(":/rsc/img/pieces/white_king.png");
    QPixmap blackPawnPixmap(":/rsc/img/pieces/black_pawn.png");
    QPixmap blackKnightPixmap(":/rsc/img/pieces/black_knight.png");
    QPixmap blackBishopPixmap(":/rsc/img/pieces/black_bishop.png");
    QPixmap blackRookPixmap(":/rsc/img/pieces/black_rook.png");
    QPixmap blackQueenPixmap(":/rsc/img/pieces/black_queen.png");
    QPixmap blackKingPixmap(":/rsc/img/pieces/black_king.png");
    QPixmap duckPixmap(":/rsc/img/pieces/duck.png");

    QPixmap maps[] = {whitePawnPixmap, whiteKnightPixmap, whiteBishopPixmap, whiteRookPixmap,
                     whiteQueenPixmap, whiteKingPixmap, blackPawnPixmap, blackKnightPixmap,
                     blackBishopPixmap, blackRookPixmap, blackQueenPixmap, blackKingPixmap};

    for (int i = 0; i < 6; ++i)
    {
        QLabel* label = new QLabel(this);
        label->setAlignment(Qt::AlignCenter);
        label->setPixmap(maps[i]);
        label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        label->setScaledContents(true);
        chessboardPanel->addWidget(label, i, 1);
    }

    for (int i = 0; i < 6; ++i)
    {
        QLabel* label = new QLabel(this);
        label->setAlignment(Qt::AlignCenter);
        label->setPixmap(maps[i + 6]);
        label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        label->setScaledContents(true);
        chessboardPanel->addWidget(label, i, 2);
    }

    QLabel* label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter);
    label->setPixmap(duckPixmap);
    label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    label->setScaledContents(true);
    chessboardPanel->addWidget(label, 4, 6);
}

MainWindow::~MainWindow()
{
    delete ui;
}

