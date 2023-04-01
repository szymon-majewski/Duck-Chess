#include <QLabel>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

extern Square BoardIndicesToSquare(const unsigned& y, const unsigned& x);
extern void SquareToBoardIndices(const Square& square, int& y, int& x);

MainWindow::MainWindow(QWidget *parent) :
    session(Position()),
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Duck Chess Engine");
    setWindowIcon(QIcon(":/rsc/img/pieces/duck.png"));
    setFixedSize(1000, 650);

    // INIT
    InitPiecesPixmaps();

    // CHESSBOARD
    chessboardPanel = MainWindow::findChild<QGridLayout*>("chessboardPanel");

    for(int y = 0; y < 8; ++y)
    {
        for(int x = 0; x < 8; ++x)
        {
            // Mixed x and y - I don't know why I have to do this
            squareFrames[x][y].x = y;
            squareFrames[x][y].y = x;
            squareFrames[x][y].setParent(this);

            if((x + y) % 2 == 0)
            {
                squareFrames[x][y].setStyleSheet("background-color: " + LIGHT_SQUARE_COLOR.name());
            }
            else
            {
                squareFrames[x][y].setStyleSheet("background-color: " + DARK_SQUARE_COLOR.name());
            }

            squareFrames[x][y].setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            squareFrames[x][y].setMinimumSize(QSize(70, 70));
            squareFrames[x][y].setMaximumSize(QSize(70, 70));

            chessboardPanel->addWidget(&squareFrames[x][y], x, y);
        }
    }

    for (int i = 0; i < 8; i++)
    {
        chessboardPanel->setRowStretch(i, 1);
        chessboardPanel->setColumnStretch(i, 1);
    }

    // MOVES BUTTONS
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

    // FEN
    //// BUTTON
    QPushButton* fenUpdateBtn = MainWindow::findChild<QPushButton*>("updateFenBtn");
    connect(fenUpdateBtn, SIGNAL(released()), this, SLOT(FenUpdateButtonPressed()));

    //// TEXT EDIT
    fenTextEdit = MainWindow::findChild<QTextEdit*>("fenTextEdit");

    // STARTING POSITION
    session.position = fenParser.ParseFen(STARTING_POSITION_FEN);
    UpdateChessboard();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::FenUpdateButtonPressed()
{
    try
    {
        session.position = fenParser.ParseFen(fenTextEdit->toPlainText().toStdString());
    }
    catch (std::exception)
    {
        // Just don't update
    }

    UpdateChessboard();
}

void MainWindow::UpdateChessboard()
{
    piecesLabels.clear();

    BitPiece currentBitPiece;

    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            currentBitPiece = session.position.board.pieces[y][x].GetBitPiece();

            if (currentBitPiece != NO_PIECE)
            {
                auto& insertedLabel = piecesLabels.emplace_back(std::make_unique<PieceLabel>(this, x, 7 - y));
                insertedLabel->setAlignment(Qt::AlignCenter);
                insertedLabel->setPixmap(piecesPixmaps.at(currentBitPiece));
                insertedLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
                insertedLabel->setScaledContents(true);
                chessboardPanel->addWidget(insertedLabel.get(), 7 - y, x);
            }
        }
    }
}

void MainWindow::OnEmptySquareClicked(unsigned int x, unsigned int y)
{
    qDebug() << "Kliknieto puste pole " + QString::number(x) + " " + QString::number(y);

    // Piece was selected earlier and wants to move to an empty square
    if (selectedSquare != Square::None)
    {
        // MAKE A MOVE HERE!!!!
        // ...

        int sourceSqaureX;
        int sourceSqaureY;

        SquareToBoardIndices(selectedSquare, sourceSqaureY, sourceSqaureX);
        DeselectSquare(x, 7 - y);
        selectedSquare = Square::None;
    }

}

void MainWindow::OnPieceClicked(unsigned int x, unsigned int y)
{
    // THINK IF YOU WANT TO CLICK DUCK OR NOT EVERY TIME TO MOVE IT (IF NOT YOU NEED TO TAKE INTO ACCOUNT THAT IT'S NOT ALWAYS THERE)
    qDebug() << "Kliknieto figurke " + QString::number(x) + " " + QString::number(y);

    uint8_t clickedPieceColor = (uint8_t)session.position.board.pieces[7 - y][x].PieceColor();

    // No piece was selected earlier
    if (selectedSquare == Square::None)
    {
        // Color of clicked piece matches player that is to move
        if (firstPhaseMove && clickedPieceColor == (uint8_t)session.position.playerToMove)
        {
            selectedSquare = BoardIndicesToSquare(7 - y, x);
            SelectSquare(x, y);
        }
        // Or it's duck move and duck was clicked
        else if (!firstPhaseMove && clickedPieceColor == (uint8_t)Piece::Color::Both)
        {
            SelectSquare(x, y);
        }
    }
    // Piece was selected earlier and wants to land on a square that is occupied by some other piece
    else
    {
        if (firstPhaseMove && clickedPieceColor != (uint8_t)session.position.playerToMove)
        {
            // MAKE A MOVE HERE!!!!
            // ...

            int sourceSqaureX;
            int sourceSqaureY;

            SquareToBoardIndices(selectedSquare, sourceSqaureY, sourceSqaureX);
            DeselectSquare(x, 7 - y);
            selectedSquare = Square::None;
        }
    }
}

void MainWindow::SelectSquare(unsigned int x, unsigned int y)
{
    if ((x + y) % 2 == 0)
    {
        qDebug() << "To jest jasne";
        squareFrames[x][y].setStyleSheet("background-color: " + SELECTED_LIGHT_SQUARE_COLOR.name());
    }
    else
    {
        squareFrames[x][y].setStyleSheet("background-color: " + SELECTED_DARK_SQUARE_COLOR.name());
    }
}

void MainWindow::DeselectSquare(unsigned int x, unsigned int y)
{
    if ((x + y) % 2 == 0)
    {
        squareFrames[x][y].setStyleSheet("background-color: " + LIGHT_SQUARE_COLOR.name());
    }
    else
    {
        squareFrames[x][y].setStyleSheet("background-color: " + DARK_SQUARE_COLOR.name());
    }
}

void MainWindow::InitPiecesPixmaps()
{
    piecesPixmaps =
    {
        { (uint8_t)Piece::Type::Pawn | (uint8_t)Piece::Color::Black, QPixmap(":/rsc/img/pieces/black_pawn.png") },
        { (uint8_t)Piece::Type::Pawn | (uint8_t)Piece::Color::White, QPixmap(":/rsc/img/pieces/white_pawn.png") },
        { (uint8_t)Piece::Type::Knight | (uint8_t)Piece::Color::Black, QPixmap(":/rsc/img/pieces/black_knight.png") },
        { (uint8_t)Piece::Type::Knight | (uint8_t)Piece::Color::White, QPixmap(":/rsc/img/pieces/white_knight.png") },
        { (uint8_t)Piece::Type::Bishop | (uint8_t)Piece::Color::Black, QPixmap(":/rsc/img/pieces/black_bishop.png") },
        { (uint8_t)Piece::Type::Bishop | (uint8_t)Piece::Color::White, QPixmap(":/rsc/img/pieces/white_bishop.png") },
        { (uint8_t)Piece::Type::Rook | (uint8_t)Piece::Color::Black, QPixmap(":/rsc/img/pieces/black_rook.png") },
        { (uint8_t)Piece::Type::Rook | (uint8_t)Piece::Color::White, QPixmap(":/rsc/img/pieces/white_rook.png") },
        { (uint8_t)Piece::Type::Queen | (uint8_t)Piece::Color::Black, QPixmap(":/rsc/img/pieces/black_queen.png") },
        { (uint8_t)Piece::Type::Queen | (uint8_t)Piece::Color::White, QPixmap(":/rsc/img/pieces/white_queen.png") },
        { (uint8_t)Piece::Type::King | (uint8_t)Piece::Color::Black, QPixmap(":/rsc/img/pieces/black_king.png") },
        { (uint8_t)Piece::Type::King | (uint8_t)Piece::Color::White, QPixmap(":/rsc/img/pieces/white_king.png") },
        { (uint8_t)Piece::Type::Duck | (uint8_t)Piece::Color::Both, QPixmap(":/rsc/img/pieces/duck.png") }
    };
}
