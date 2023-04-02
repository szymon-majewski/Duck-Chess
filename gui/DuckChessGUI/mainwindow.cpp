#include <QLabel>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

extern Square BoardIndicesToSquare(const unsigned& y, const unsigned& x);
extern void SquareToBoardIndices(const Square& square, int& y, int& x);

MainWindow::MainWindow(QWidget *parent, Session* session, FenParser* fenParser, MovesGenerator* movesGenerator) :
    session(session),
    fenParser(fenParser),
    movesGenerator(movesGenerator),
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
            squareFrames[x][y].x = x;
            squareFrames[x][y].y = y;
            squareFrames[x][y].mainWindow = this;

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

            chessboardPanel->addWidget(&squareFrames[x][y], y, x);
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
    session->position = fenParser->ParseFen(STARTING_POSITION_FEN);
    session->position.materialDisparity = session->position.CountMaterial();
    unsigned int placeholder;
    currentLegalChessMoves = movesGenerator->GenerateLegalChessMoves(session->position, placeholder);
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
        session->position = fenParser->ParseFen(fenTextEdit->toPlainText().toStdString());
        session->position.materialDisparity = session->position.CountMaterial();
        unsigned int placeholder;
        currentLegalChessMoves = movesGenerator->GenerateLegalChessMoves(session->position, placeholder);

        if (selectedSquare != Square::None)
        {
            DeselectSquare(selectedSquare);
            selectedSquare = Square::None;
        }
        firstPhase = true;

        UpdateChessboard();
    }
    catch (std::exception)
    {
        // Just don't update
    }
}

void MainWindow::UpdateChessboard()
{
    piecesLabels.clear();
    duckOnTheBoard = false;

    BitPiece currentBitPiece;

    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            currentBitPiece = session->position.board.pieces[y][x].GetBitPiece();

            if (currentBitPiece != NO_PIECE)
            {
                auto& insertedLabel = piecesLabels.emplace_back(std::make_unique<PieceLabel>(this, this, x, 7 - y));
                insertedLabel->setAlignment(Qt::AlignCenter);
                insertedLabel->setPixmap(piecesPixmaps.at(currentBitPiece));
                insertedLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
                insertedLabel->setScaledContents(true);
                chessboardPanel->addWidget(insertedLabel.get(), 7 - y, x);

                if (session->position.board.pieces[y][x].PieceType() == Piece::Type::Duck)
                {
                    duckOnTheBoard = true;
                }
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
        int sourceSquareX;
        int sourceSquareY;
        SquareToBoardIndices(selectedSquare, sourceSquareY, sourceSquareX);
        Square targetSquare = BoardIndicesToSquare(7 - y, x);

        // MOVE ADDITIONAL INFO!!! - PROBABLY GO FOR MOVES GENERATOR VALIDATION
        // I GUESS IT ONLY NEDDS CHESS MOVES AND THEN CHECK IF DUCK LANDING SQUARE IS EMPTY (with regards to just moved piece)
        if (session->position.board.pieces[sourceSquareY][sourceSquareX].PieceType() != Piece::Type::Duck)
        {
            bool validMove = false;

            for (const Move& move : *currentLegalChessMoves)
            {
                if (move.sourceSquare == selectedSquare &&  move.targetSquare == targetSquare)
                {
                    firstPhaseMove = move;
                    validMove = true;
                    break;
                }
            }

            if (!validMove)
            {
                return;
            }

            // En passant - removing taken pawn label
            if (firstPhaseMove.additionalInfo == Move::AdditionalInfo::EnPassant)
            {
                unsigned int takenPieceIndex;

                if (session->position.playerToMove == PlayerColor::White)
                {
                    for (int i = 0; i < piecesLabels.size(); ++i)
                    {
                        if (x == piecesLabels[i]->x && y == piecesLabels[i]->y - 1)
                        {
                            takenPieceIndex = i;
                            break;
                        }
                    }
                }
                else
                {
                    for (int i = 0; i < piecesLabels.size(); ++i)
                    {
                        if (x == piecesLabels[i]->x && y == piecesLabels[i]->y + 1)
                        {
                            takenPieceIndex = i;
                            break;
                        }
                    }
                }

                piecesLabels.erase(piecesLabels.begin() + takenPieceIndex);
            }
        }
        else
        {
            session->MakeMove(FullMove(firstPhaseMove, selectedSquare, targetSquare));
            unsigned int placeholder;
            currentLegalChessMoves = movesGenerator->GenerateLegalChessMoves(session->position, placeholder);
        }

        //////////////////////////////////////////////////////////////
        // Remove label from source sqaure
        PieceLabel* movingPieceLabel;

        for (const std::unique_ptr<PieceLabel>& pieceLabel : piecesLabels)
        {
            if (sourceSquareX == pieceLabel->x && 7 - sourceSquareY == pieceLabel->y)
            {
                pieceLabel->x = x;
                pieceLabel->y = y;
                movingPieceLabel = pieceLabel.get();
                break;
            }
        }
        chessboardPanel->removeWidget(movingPieceLabel);

        // Add label to target sqaure
        chessboardPanel->addWidget(movingPieceLabel, y, x);
        //////////////////////////////////////////////////////////////

        DeselectSquare(sourceSquareX, 7 - sourceSquareY);
        selectedSquare = Square::None;
        firstPhase = !firstPhase;
    }
    // It's first move, duck is not on the board, so just place it on the clicked square
    else if (!duckOnTheBoard && !firstPhase)
    {
        //////////////////////////////////////////////////////////////
        auto& duckLabel = piecesLabels.emplace_back(std::make_unique<PieceLabel>(this, this, x, y));
        duckLabel->setAlignment(Qt::AlignCenter);
        duckLabel->setPixmap(piecesPixmaps.at((uint8_t)Piece::Type::Duck | (uint8_t)Piece::Color::Both));
        duckLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        duckLabel->setScaledContents(true);
        chessboardPanel->addWidget(duckLabel.get(), y, x);
        //////////////////////////////////////////////////////////////

        firstPhase = true;
        duckOnTheBoard = true;

        session->MakeMove(FullMove(firstPhaseMove, selectedSquare, BoardIndicesToSquare(7 - y, x)));
        unsigned int placeholder;
        currentLegalChessMoves = movesGenerator->GenerateLegalChessMoves(session->position, placeholder);
    }
}

void MainWindow::OnPieceClicked(unsigned int x, unsigned int y)
{
    qDebug() << "Kliknieto figurke " + QString::number(x) + " " + QString::number(y);

    uint8_t clickedPieceColor = (uint8_t)session->position.board.pieces[7 - y][x].PieceColor();

    // No piece was selected earlier
    if (selectedSquare == Square::None)
    {
        // Color of clicked piece matches player that is to move
        if (firstPhase && clickedPieceColor == (uint8_t)session->position.playerToMove)
        {
            selectedSquare = BoardIndicesToSquare(7 - y, x);
            SelectSquare(x, y);
        }
        // Or it's duck move and duck was clicked
        else if (!firstPhase && clickedPieceColor == (uint8_t)Piece::Color::Both)
        {
            selectedSquare = BoardIndicesToSquare(7 - y, x);
            SelectSquare(x, y);
        }
    }
    // Piece was selected earlier and wants to land on a square that is occupied by some other piece
    else
    {
        if (firstPhase)
        {
            PlayerColor opponentsColor = session->position.playerToMove == PlayerColor::White ? PlayerColor::Black : PlayerColor::White;

            // Selected piece wants to take enemy piece
            if (clickedPieceColor == (uint8_t)opponentsColor)
            {
                Square targetSquare = BoardIndicesToSquare(7 - y, x);
                bool validMove = false;

                for (const Move& move : *currentLegalChessMoves)
                {
                    if (move.sourceSquare == selectedSquare &&  move.targetSquare == targetSquare)
                    {
                        firstPhaseMove = move;
                        validMove = true;
                        break;
                    }
                }

                if (!validMove)
                {
                    return;
                }

                int sourceSquareX;
                int sourceSquareY;
                SquareToBoardIndices(selectedSquare, sourceSquareY, sourceSquareX);

                //////////////////////////////////////////////////////////////
                // Remove label from source sqaure and target square
                PieceLabel* movingPieceLabel;
                unsigned int takenPieceIndex;
                unsigned int breakChecker = 0;

                for (int i = 0; i < piecesLabels.size() && breakChecker < 2; ++i)
                {
                    if (sourceSquareX == piecesLabels[i]->x && 7 - sourceSquareY == piecesLabels[i]->y)
                    {
                        piecesLabels[i]->x = x;
                        piecesLabels[i]->y = y;
                        movingPieceLabel = piecesLabels[i].get();
                        ++breakChecker;
                    }
                    else if (x == piecesLabels[i]->x && y == piecesLabels[i]->y)
                    {
                        takenPieceIndex = i;
                        ++breakChecker;
                    }
                }

                piecesLabels.erase(piecesLabels.begin() + takenPieceIndex);

                // Add label to target sqaure
                chessboardPanel->addWidget(movingPieceLabel, y, x);
                //////////////////////////////////////////////////////////////

                DeselectSquare(sourceSquareX, 7 - sourceSquareY);
                selectedSquare = Square::None;
                firstPhase = false;
            }
            // Change selected piece
            else if (clickedPieceColor == (uint8_t)session->position.playerToMove)
            {
                int sourceSqaureX;
                int sourceSqaureY;
                SquareToBoardIndices(selectedSquare, sourceSqaureY, sourceSqaureX);
                DeselectSquare(sourceSqaureX, 7 - sourceSqaureY);

                selectedSquare = BoardIndicesToSquare(7 - y, x);
                SelectSquare(x, y);
            }
        }
    }
}

void MainWindow::SelectSquare(unsigned int x, unsigned int y)
{
    if ((x + y) % 2 == 0)
    {
        squareFrames[x][y].setStyleSheet("background-color: " + SELECTED_LIGHT_SQUARE_COLOR.name());
    }
    else
    {
        squareFrames[x][y].setStyleSheet("background-color: " + SELECTED_DARK_SQUARE_COLOR.name());
    }
}

void MainWindow::SelectSquare(Square square)
{
    int x;
    int y;

    SquareToBoardIndices(square, y, x);

    SelectSquare(x, 7 - y);
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

void MainWindow::DeselectSquare(Square square)
{
    int x;
    int y;

    SquareToBoardIndices(square, y, x);

    DeselectSquare(x, 7 - y);
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
