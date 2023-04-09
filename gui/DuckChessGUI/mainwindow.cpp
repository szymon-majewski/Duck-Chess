#include <QLabel>
#include <QScrollBar>

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDialog>

extern Square BoardIndicesToSquare(const unsigned& y, const unsigned& x);
extern void SquareToBoardIndices(const Square& square, int& y, int& x);
extern std::string MoveStringFormat(const FullMove& move, const Board& board);

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
    setFixedSize(900, 650);

    // INIT
    InitPiecesPixmaps();

    engineWorker = std::make_unique<EngineWorker>(&signalsSentWithoutResponse);
    engineThread = std::make_unique<QThread>();
    engineWorker->moveToThread(engineThread.get());
    connect(engineWorker.get(), SIGNAL(ResultReady(Engine::SearchInfo)), this, SLOT(HandleEngineResult(Engine::SearchInfo)));
    connect(this, SIGNAL(StartEngine(Position)), engineWorker.get(), SLOT(Search(Position)));
    engineThread->start(QThread::TimeCriticalPriority);

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

    // ENGINE PANEL
    evaluationLabel = MainWindow::findChild<QLabel*>("evaluationLabel");
    depthLabel = MainWindow::findChild<QLabel*>("depthLabel");
    depthLabel->setText("Depth: " + QString::number(engineWorker->Depth()));
    bestMovesLabel = MainWindow::findChild<QLabel*>("bestMovesLabel");
    moveNumberLabel = MainWindow::findChild<QLabel*>("moveNumberLabel");
    rule50Label = MainWindow::findChild<QLabel*>("rule50Label");
    playerToMoveLabel = MainWindow::findChild<QLabel*>("playerToMoveLabel");
    castlingRightsWhiteLabel = MainWindow::findChild<QLabel*>("castlingRightsWhiteLabel");
    castlingRightsBlackLabel = MainWindow::findChild<QLabel*>("castlingRightsBlackLabel");

    // MOVES PANEL
    movesScrollArea = MainWindow::findChild<QScrollArea*>("movesScrollArea");
    scrollWidget = std::make_unique<QWidget>();
    movesGridLayout = std::make_unique<QGridLayout>();
    movesGridLayout->setAlignment(Qt::AlignTop);
    movesScrollArea->setWidget(scrollWidget.get());
    scrollWidget->setLayout(movesGridLayout.get());
    movesScrollArea->setWidgetResizable(true);
    movesScrollArea->setWidget(scrollWidget.get());

    movesGridLayout->setColumnStretch(0, 1);
    movesGridLayout->setColumnStretch(1, 1);
    movesGridLayout->setColumnStretch(2, 1);

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

    connect(backwardsBtn, SIGNAL(released()), this, SLOT(OnBackwardsButtonPressed()));
    connect(forwardsBtn, SIGNAL(released()), this, SLOT(OnForwardsButtonPressed()));
    connect(fastForwardsBtn, SIGNAL(released()), this, SLOT(OnFastForwardsButtonPressed()));
    connect(fastBackwardsBtn, SIGNAL(released()), this, SLOT(OnFastBackwardsButtonPressed()));

    // FEN
    //// BUTTON
    QPushButton* fenUpdateBtn = MainWindow::findChild<QPushButton*>("updateFenBtn");
    connect(fenUpdateBtn, SIGNAL(released()), this, SLOT(FenUpdateButtonPressed()));

    //// TEXT EDIT
    fenTextEdit = MainWindow::findChild<QTextEdit*>("fenTextEdit");

    // STARTING POSITION
    session->position = fenParser->ParseFen(STARTING_POSITION_FEN);
    session->position.materialDisparity = session->position.CountMaterial();
    UpdateChessboard();

    unsigned int placeholder;
    currentLegalChessMoves = movesGenerator->GenerateLegalChessMoves(session->position, placeholder);
}

MainWindow::~MainWindow()
{
    engineThread->exit();

    for (int row = (movesMade.size() - 1) / 2; row >= 0; --row)
    {
        for (int col = 0; col < 3; ++col)
        {
            QLayoutItem* layoutItem = movesGridLayout->itemAtPosition(row, col);
            if (layoutItem)
            {
                QLabel* label = qobject_cast<QLabel*>(layoutItem->widget());
                movesGridLayout->removeWidget(label);
                delete label;
            }
        }
    }

    delete ui;
}

void MainWindow::FenUpdateButtonPressed()
{
    try
    {
        for (int row = (movesMade.size() - 1) / 2; row >= 0; --row)
        {
            for (int col = 0; col < 3; ++col)
            {
                QLayoutItem* layoutItem = movesGridLayout->itemAtPosition(row, col);
                if (layoutItem)
                {
                    QLabel* label = qobject_cast<QLabel*>(layoutItem->widget());
                    movesGridLayout->removeWidget(label);
                    delete label;
                }
            }
        }

        lastMovePlayedScrollAreaLabel = nullptr;

        session->Clear();
        movesMade.clear();
        currentMoveIndex = -1;

        gameEnded = false;
        session->position = fenParser->ParseFen(fenTextEdit->toPlainText().toStdString());
        session->position.materialDisparity = session->position.CountMaterial();
        startingPlayer = session->position.playerToMove;

        UpdateChessboard();
    }
    catch (std::exception)
    {
        // Just don't update
    }
}

void MainWindow::UpdateChessboard()
{
    emitStartEngine(session->position);
    UpdatePositionLabels();
    unsigned int placeholder;
    currentLegalChessMoves = movesGenerator->GenerateLegalChessMoves(session->position, placeholder);

    if (selectedSquare != Square::None)
    {
        DeselectSquare(selectedSquare);
        selectedSquare = Square::None;
    }
    firstPhase = true;

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

void MainWindow::HandleEngineResult(const Engine::SearchInfo& result)
{
    --signalsSentWithoutResponse;

    if (!gameEnded)
    {
        UpdateEvaluationLabel(result.evaluation);
        UpdateBestMovesLabel(result.movesPath);
    }
}

void MainWindow::OnBackwardsButtonPressed()
{
    if (currentMoveIndex < 0)
    {
        return;
    }

    if (gameEnded)
    {
        PlayerColor opponentsColor = session->position.playerToMove == PlayerColor::White ? PlayerColor::Black : PlayerColor::White;
        PieceLabel* movingPieceLabel;
        int sourceSquareX;
        int sourceSquareY;
        int targetSquareX;
        int targetSquareY;

        SquareToBoardIndices(movesMade[currentMoveIndex].sourceSquare, sourceSquareY, sourceSquareX);
        SquareToBoardIndices(movesMade[currentMoveIndex].targetSquare, targetSquareY, targetSquareX);

        // Piece which took the king
        for (const std::unique_ptr<PieceLabel>& pieceLabel : piecesLabels)
        {
            if (targetSquareX == pieceLabel->x && 7 - targetSquareY == pieceLabel->y)
            {
                pieceLabel->x = sourceSquareX;
                pieceLabel->y = 7 - sourceSquareY;
                movingPieceLabel = pieceLabel.get();
                break;
            }
        }

        chessboardPanel->removeWidget(movingPieceLabel);
        chessboardPanel->addWidget(movingPieceLabel, 7 - sourceSquareY, sourceSquareX);

        // King itself
        auto& insertedLabel = piecesLabels.emplace_back(std::make_unique<PieceLabel>(this, this, targetSquareX, 7 - targetSquareY));
        insertedLabel->setAlignment(Qt::AlignCenter);
        insertedLabel->setPixmap(piecesPixmaps.at((uint8_t)Piece::Type::King | (uint8_t)opponentsColor));
        insertedLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        insertedLabel->setScaledContents(true);
        chessboardPanel->addWidget(insertedLabel.get(), 7 - targetSquareY, targetSquareX);

        emitStartEngine(session->position);

        gameEnded = false;
    }
    else
    {
        session->UndoMove();
        UpdateChessboard();
    }

    --currentMoveIndex;
    firstPhase = true;

    // Changing highlited move
    lastMovePlayedScrollAreaLabel->setStyleSheet("");

    if (currentMoveIndex == -1)
    {
        lastMovePlayedScrollAreaLabel = nullptr;
    }
    else
    {
        int newHighlightedMoveColumn = startingPlayer == PlayerColor::White ? (currentMoveIndex % 2) + 1 : 2 - (currentMoveIndex % 2);
        int newHighlightedMoveRow = startingPlayer == PlayerColor::White ? currentMoveIndex / 2 : (currentMoveIndex + 1) / 2;
        QLabel* currentMove = qobject_cast<QLabel*>(movesGridLayout->itemAtPosition(newHighlightedMoveRow, newHighlightedMoveColumn)->widget());
        currentMove->setStyleSheet("background-color: #cccccc");
        lastMovePlayedScrollAreaLabel = currentMove;
    }
}

void MainWindow::OnForwardsButtonPressed()
{
    if (currentMoveIndex >= movesMade.size() - 1 && !(currentMoveIndex == -1 && movesMade.size() > 0))
    {
        return;
    }

    ++currentMoveIndex;
    firstPhase = true;

    // It's a game ending move
    if (movesMade[currentMoveIndex].targetDuckSquare == Square::None)
    {
        PieceLabel* movingPieceLabel;
        unsigned int takenPieceIndex;
        unsigned int breakChecker = 0;
        int sourceSquareX;
        int sourceSquareY;
        int targetSquareX;
        int targetSquareY;

        SquareToBoardIndices(movesMade[currentMoveIndex].sourceSquare, sourceSquareY, sourceSquareX);
        SquareToBoardIndices(movesMade[currentMoveIndex].targetSquare, targetSquareY, targetSquareX);

        // Piece which will take the king
        for (int i = 0; i < piecesLabels.size() && breakChecker < 2; ++i)
        {
            if (sourceSquareX == piecesLabels[i]->x && 7 - sourceSquareY == piecesLabels[i]->y)
            {
                piecesLabels[i]->x = targetSquareX;
                piecesLabels[i]->y = 7 - targetSquareY;
                movingPieceLabel = piecesLabels[i].get();
                ++breakChecker;
            }
            else if (targetSquareX == piecesLabels[i]->x && 7 - targetSquareY == piecesLabels[i]->y)
            {
                takenPieceIndex = i;
                ++breakChecker;
            }
        }

        piecesLabels.erase(piecesLabels.begin() + takenPieceIndex);

        chessboardPanel->removeWidget(movingPieceLabel);
        chessboardPanel->addWidget(movingPieceLabel, 7 - targetSquareY, targetSquareX);

        bestMovesLabel->setText("-----");
        gameEnded = true;
    }
    else
    {
        session->MakeMove(movesMade[currentMoveIndex]);
        UpdateChessboard();
    }

    // Changing highlited move
    if (lastMovePlayedScrollAreaLabel)
    {
        lastMovePlayedScrollAreaLabel->setStyleSheet("");
    }

    int newHighlightedMoveColumn = startingPlayer == PlayerColor::White ? (currentMoveIndex % 2) + 1 : 2 - (currentMoveIndex % 2);
    int newHighlightedMoveRow = startingPlayer == PlayerColor::White ? currentMoveIndex / 2 : (currentMoveIndex + 1) / 2;
    QLabel* currentMove = qobject_cast<QLabel*>(movesGridLayout->itemAtPosition(newHighlightedMoveRow, newHighlightedMoveColumn)->widget());
    currentMove->setStyleSheet("background-color: #cccccc");
    lastMovePlayedScrollAreaLabel = currentMove;
}

void MainWindow::OnFastBackwardsButtonPressed()
{
    if (currentMoveIndex < 0)
    {
        return;
    }

    if (gameEnded)
    {
        --currentMoveIndex;
        gameEnded = false;
    }

    for (int i = currentMoveIndex; i >= 0; --i)
    {
        session->UndoMove();
    }

    currentMoveIndex = -1;
    firstPhase = true;
    lastMovePlayedScrollAreaLabel->setStyleSheet("");
    lastMovePlayedScrollAreaLabel = nullptr;

    UpdateChessboard();
}

void MainWindow::OnFastForwardsButtonPressed()
{
    if (currentMoveIndex >= movesMade.size() - 1 && !(currentMoveIndex == -1 && movesMade.size() > 0))
    {
        return;
    }

    for (int i = currentMoveIndex + 1; i < movesMade.size() - 1; ++i)
    {
        session->MakeMove(movesMade[i]);
    }

    int lastMoveIndex = movesMade.size() - 1;

    // It's a game ending move
    if (movesMade[lastMoveIndex].targetDuckSquare == Square::None)
    {
        UpdateChessboard();

        PieceLabel* movingPieceLabel;
        unsigned int takenPieceIndex;
        unsigned int breakChecker = 0;
        int sourceSquareX;
        int sourceSquareY;
        int targetSquareX;
        int targetSquareY;

        SquareToBoardIndices(movesMade[lastMoveIndex].sourceSquare, sourceSquareY, sourceSquareX);
        SquareToBoardIndices(movesMade[lastMoveIndex].targetSquare, targetSquareY, targetSquareX);

        // Piece which will take the king
        for (int i = 0; i < piecesLabels.size() && breakChecker < 2; ++i)
        {
            if (sourceSquareX == piecesLabels[i]->x && 7 - sourceSquareY == piecesLabels[i]->y)
            {
                piecesLabels[i]->x = targetSquareX;
                piecesLabels[i]->y = 7 - targetSquareY;
                movingPieceLabel = piecesLabels[i].get();
                ++breakChecker;
            }
            else if (targetSquareX == piecesLabels[i]->x && 7 - targetSquareY == piecesLabels[i]->y)
            {
                takenPieceIndex = i;
                ++breakChecker;
            }
        }

        piecesLabels.erase(piecesLabels.begin() + takenPieceIndex);

        chessboardPanel->removeWidget(movingPieceLabel);
        chessboardPanel->addWidget(movingPieceLabel, 7 - targetSquareY, targetSquareX);

        bestMovesLabel->setText("-----");
        gameEnded = true;
    }
    else
    {
        session->MakeMove(movesMade[lastMoveIndex]);
        UpdateChessboard();
    }

    currentMoveIndex = lastMoveIndex;

    // Changing highlited move
    if (lastMovePlayedScrollAreaLabel)
    {
        lastMovePlayedScrollAreaLabel->setStyleSheet("");
    }

    int newHighlightedMoveColumn = startingPlayer == PlayerColor::White ? (currentMoveIndex % 2) + 1 : 2 - (currentMoveIndex % 2);
    int newHighlightedMoveRow = startingPlayer == PlayerColor::White ? currentMoveIndex / 2 : (currentMoveIndex + 1) / 2;
    QLabel* currentMove = qobject_cast<QLabel*>(movesGridLayout->itemAtPosition(newHighlightedMoveRow, newHighlightedMoveColumn)->widget());
    currentMove->setStyleSheet("background-color: #cccccc");
    lastMovePlayedScrollAreaLabel = currentMove;
}

void MainWindow::OnEmptySquareClicked(unsigned int x, unsigned int y)
{
    qDebug() << "Kliknieto puste pole " + QString::number(x) + " " + QString::number(y);

    if (gameEnded)
    {
        return;
    }

    // Piece was selected earlier and wants to move to an empty square
    if (selectedSquare != Square::None)
    {
        int sourceSquareX;
        int sourceSquareY;
        SquareToBoardIndices(selectedSquare, sourceSquareY, sourceSquareX);
        Square targetSquare = BoardIndicesToSquare(7 - y, x);
        PlayerColor movingPieceColor = session->position.playerToMove;

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

                if (movingPieceColor == PlayerColor::White)
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

            // Promotions
            else if ((Move::AdditionalInfo)((uint16_t)firstPhaseMove.additionalInfo & (uint16_t)Move::promotionChecker) != Move::AdditionalInfo::None)
            {
                QDialog promotionDialog;
                promotionDialog.setWindowFlags(Qt::CustomizeWindowHint /*| Qt::WindowTitleHint*/);
                QGridLayout layout(&promotionDialog);
                QPushButton promotionButtons[4];

                for (int i = 0; i < 4; ++i)
                {
                    Piece::Type promotionPiece = promotionPieces[i];
                    promotionButtons[i].setIcon(QIcon(piecesPixmaps.at((uint8_t)movingPieceColor | (uint8_t)promotionPiece)));
                    promotionButtons[i].setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
                    promotionButtons[i].setMinimumSize(QSize(70, 70));
                    promotionButtons[i].setMaximumSize(QSize(70, 70));
                    promotionButtons[i].setIconSize(promotionButtons[i].size());
                    layout.addWidget(promotionButtons + i, 0, i);
                    connect(&promotionButtons[i], &QPushButton::clicked, this,
                            [&, promotionPiece]()
                            {
                                PieceLabel* movingPieceLabel;

                                for (const std::unique_ptr<PieceLabel>& pieceLabel : piecesLabels)
                                {
                                    if (sourceSquareX == pieceLabel->x && 7 - sourceSquareY == pieceLabel->y)
                                    {
                                        movingPieceLabel = pieceLabel.get();
                                        break;
                                    }
                                }

                                movingPieceLabel->setPixmap(piecesPixmaps.at((uint8_t)movingPieceColor | (uint8_t)promotionPiece));

                                firstPhaseMove.additionalInfo = (Move::AdditionalInfo)((uint16_t)firstPhaseMove.additionalInfo & ~Move::promotionChecker);

                                Move::AdditionalInfo promotionType;
                                switch (promotionPiece)
                                {
                                    case Piece::Type::Queen:
                                    {
                                        promotionType = Move::AdditionalInfo::PromotionToQueen;
                                        break;
                                    }
                                    case Piece::Type::Knight:
                                    {
                                        promotionType = Move::AdditionalInfo::PromotionToKnight;
                                        break;
                                    }
                                    case Piece::Type::Rook:
                                    {
                                        promotionType = Move::AdditionalInfo::PromotionToRook;
                                        break;
                                    }
                                    case Piece::Type::Bishop:
                                    {
                                        promotionType = Move::AdditionalInfo::PromotionToBishop;
                                        break;
                                    }
                                }

                                firstPhaseMove.additionalInfo = (Move::AdditionalInfo)((uint16_t)firstPhaseMove.additionalInfo | (uint16_t)promotionType);
                                promotionDialog.accept();
                            });
                }

                promotionDialog.exec();
            }

            // Castling
            else if ((Move::AdditionalInfo)((uint16_t)firstPhaseMove.additionalInfo & (uint16_t)Move::castlingChecker) != Move::AdditionalInfo::None)
            {
                PieceLabel* rookLabel;
                const auto& [rookSourceSquare, rookTargetSquare] = Move::ROOK_CASTLING_SQUARES.at(firstPhaseMove.additionalInfo);
                int rookSourceY;
                int rookSourceX;
                int rookTargetY;
                int rookTargetX;
                SquareToBoardIndices(rookSourceSquare, rookSourceY, rookSourceX);
                SquareToBoardIndices(rookTargetSquare, rookTargetY, rookTargetX);

                for (const std::unique_ptr<PieceLabel>& pieceLabel : piecesLabels)
                {
                    if (rookSourceX == pieceLabel->x && 7 - rookSourceY == pieceLabel->y)
                    {
                        pieceLabel->x = rookTargetX;
                        pieceLabel->y = 7 - rookTargetY;
                        rookLabel = pieceLabel.get();
                        break;
                    }
                }

                chessboardPanel->removeWidget(rookLabel);
                chessboardPanel->addWidget(rookLabel, 7 - rookTargetY, rookTargetX);
            }

        }
        else
        {
            auto newMove = FullMove(firstPhaseMove, selectedSquare, targetSquare);
            AddMoveToList(newMove);
            session->MakeMove(newMove);
            emitStartEngine(session->position);

            // Delete the moves made after current time in game if there are any
            if (currentMoveIndex != movesMade.size() - 1)
            {
                int startRow = startingPlayer == PlayerColor::White ? (movesMade.size() - 1) / 2 : movesMade.size() / 2;
                int endRow;

                if (currentMoveIndex != -1)
                {
                    endRow = startingPlayer == PlayerColor::White ? currentMoveIndex / 2 : (currentMoveIndex + 1) / 2;
                }
                else
                {
                    endRow = -1;
                }

                for (int row = startRow; row > endRow; --row)
                {
                    for (int col = 0; col < 3; ++col)
                    {
                        QLayoutItem* layoutItem = movesGridLayout->itemAtPosition(row, col);
                        if (layoutItem)
                        {
                            QLabel* label = qobject_cast<QLabel*>(layoutItem->widget());
                            movesGridLayout->removeWidget(label);
                            delete label;
                        }
                    }
                }

                // Before making a move it's black turn, so we want to delete blacks' move from the list
                if (session->position.playerToMove == PlayerColor::White)
                {
                    int rowToDeleteIndex = startingPlayer == PlayerColor::White ? currentMoveIndex / 2 : currentMoveIndex / 2 + 1;
                    QLayoutItem* layoutItem = movesGridLayout->itemAtPosition(rowToDeleteIndex, 2);
                    if (layoutItem)
                    {
                        movesGridLayout->removeWidget(layoutItem->widget());
                        delete layoutItem->widget();
                    }
                }

                movesMade.erase(movesMade.begin() + currentMoveIndex + 1, movesMade.end());
            }

            movesMade.emplace_back(newMove);
            ++currentMoveIndex;

            UpdatePositionLabels();
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

        auto newMove = FullMove(firstPhaseMove, selectedSquare, BoardIndicesToSquare(7 - y, x));

        // Delete the moves made after current time in game if there are any
        if (currentMoveIndex != movesMade.size() - 1)
        {
            int startRow = startingPlayer == PlayerColor::White ? (movesMade.size() - 1) / 2 : movesMade.size() / 2;
            for (int row = startRow; row >= 0; --row)
            {
                for (int col = 0; col < 3; ++col)
                {
                    QLayoutItem* layoutItem = movesGridLayout->itemAtPosition(row, col);
                    if (layoutItem)
                    {
                        QLabel* label = qobject_cast<QLabel*>(layoutItem->widget());
                        movesGridLayout->removeWidget(label);
                        delete label;
                    }
                }
            }

            movesMade.erase(movesMade.begin() + currentMoveIndex + 1, movesMade.end());
        }

        AddMoveToList(newMove);
        session->MakeMove(newMove);
        emitStartEngine(session->position);

        movesMade.emplace_back(newMove);
        ++currentMoveIndex;

        UpdatePositionLabels();
        unsigned int placeholder;
        currentLegalChessMoves = movesGenerator->GenerateLegalChessMoves(session->position, placeholder);
    }
}

void MainWindow::OnPieceClicked(unsigned int x, unsigned int y)
{
    qDebug() << "Kliknieto figurke " + QString::number(x) + " " + QString::number(y);

    if (gameEnded)
    {
        return;
    }

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
            int sourceSquareX;
            int sourceSquareY;
            SquareToBoardIndices(selectedSquare, sourceSquareY, sourceSquareX);
            PlayerColor opponentsColor = session->position.playerToMove == PlayerColor::White ? PlayerColor::Black : PlayerColor::White;
            PlayerColor movingPieceColor = session->position.playerToMove;

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

                // Promotions
                if ((Move::AdditionalInfo)((uint16_t)firstPhaseMove.additionalInfo & (uint16_t)Move::promotionChecker) != Move::AdditionalInfo::None)
                {
                    QDialog promotionDialog;
                    promotionDialog.setWindowFlags(Qt::CustomizeWindowHint /*| Qt::WindowTitleHint*/);
                    QGridLayout layout(&promotionDialog);
                    QPushButton promotionButtons[4];

                    for (int i = 0; i < 4; ++i)
                    {
                        Piece::Type promotionPiece = promotionPieces[i];
                        promotionButtons[i].setIcon(QIcon(piecesPixmaps.at((uint8_t)movingPieceColor | (uint8_t)promotionPiece)));
                        promotionButtons[i].setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
                        promotionButtons[i].setMinimumSize(QSize(70, 70));
                        promotionButtons[i].setMaximumSize(QSize(70, 70));
                        promotionButtons[i].setIconSize(promotionButtons[i].size());
                        layout.addWidget(promotionButtons + i, 0, i);
                        connect(&promotionButtons[i], &QPushButton::clicked, this,
                                [&, promotionPiece]()
                                {
                                    PieceLabel* movingPieceLabel;

                                    for (const std::unique_ptr<PieceLabel>& pieceLabel : piecesLabels)
                                    {
                                        if (sourceSquareX == pieceLabel->x && 7 - sourceSquareY == pieceLabel->y)
                                        {
                                            movingPieceLabel = pieceLabel.get();
                                            break;
                                        }
                                    }

                                    movingPieceLabel->setPixmap(piecesPixmaps.at((uint8_t)movingPieceColor | (uint8_t)promotionPiece));

                                    firstPhaseMove.additionalInfo = (Move::AdditionalInfo)((uint16_t)firstPhaseMove.additionalInfo & ~Move::promotionChecker);

                                    Move::AdditionalInfo promotionType;
                                    switch (promotionPiece)
                                    {
                                        case Piece::Type::Queen:
                                        {
                                            promotionType = Move::AdditionalInfo::PromotionToQueen;
                                            break;
                                        }
                                        case Piece::Type::Knight:
                                        {
                                            promotionType = Move::AdditionalInfo::PromotionToKnight;
                                            break;
                                        }
                                        case Piece::Type::Rook:
                                        {
                                            promotionType = Move::AdditionalInfo::PromotionToRook;
                                            break;
                                        }
                                        case Piece::Type::Bishop:
                                        {
                                            promotionType = Move::AdditionalInfo::PromotionToBishop;
                                            break;
                                        }
                                    }

                                    firstPhaseMove.additionalInfo = (Move::AdditionalInfo)((uint16_t)firstPhaseMove.additionalInfo | (uint16_t)promotionType);
                                    promotionDialog.accept();
                                });
                    }

                    promotionDialog.exec();
                }

                if ((Move::AdditionalInfo)((uint16_t)firstPhaseMove.additionalInfo & (uint16_t)Move::AdditionalInfo::CapturedKing) != Move::AdditionalInfo::None)
                {
                    // Delete the moves made after current time in game if there are any
                    if (currentMoveIndex != movesMade.size() - 1)
                    {
                        int startRow = startingPlayer == PlayerColor::White ? (movesMade.size() - 1) / 2 : movesMade.size() / 2;
                        int endRow;

                        if (currentMoveIndex != -1)
                        {
                            endRow = startingPlayer == PlayerColor::White ? currentMoveIndex / 2 : (currentMoveIndex + 1) / 2;
                        }
                        else
                        {
                            endRow = -1;
                        }

                        for (int row = startRow; row > endRow; --row)
                        {
                            for (int col = 0; col < 3; ++col)
                            {
                                QLayoutItem* layoutItem = movesGridLayout->itemAtPosition(row, col);
                                if (layoutItem)
                                {
                                    QLabel* label = qobject_cast<QLabel*>(layoutItem->widget());
                                    movesGridLayout->removeWidget(label);
                                    delete label;
                                }
                            }
                        }

                        // Before making a move it's black turn, so we want to delete blacks' move from the list
                        if (session->position.playerToMove == PlayerColor::White)
                        {
                            int rowToDeleteIndex = startingPlayer == PlayerColor::White ? currentMoveIndex / 2 : currentMoveIndex / 2 + 1;
                            QLayoutItem* layoutItem = movesGridLayout->itemAtPosition(rowToDeleteIndex, 2);
                            if (layoutItem)
                            {
                                movesGridLayout->removeWidget(layoutItem->widget());
                                delete layoutItem->widget();
                            }
                        }

                        movesMade.erase(movesMade.begin() + currentMoveIndex + 1, movesMade.end());
                    }

                    auto newMove = FullMove(firstPhaseMove, Square::None, Square::None);
                    AddMoveToList(newMove);
                    movesMade.emplace_back(newMove);
                    ++currentMoveIndex;
                    gameEnded = true;
                    bestMovesLabel->setText("-----");
                }

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

void MainWindow::AddMoveToList(const FullMove& move)
{
    QLabel* moveLabel = new QLabel();
    moveLabel->setText(QString::fromStdString(MoveStringFormat(move, session->position.board)));
    moveLabel->setAlignment(Qt::AlignLeft);
    unsigned int rowIndex = ((startingPlayer == PlayerColor::White) ?
                                 (currentMoveIndex + 1) / 2 :
                                 (currentMoveIndex + 2) / 2);

    if (session->position.playerToMove == PlayerColor::White)
    {
        QLabel* moveNumberScrollAreaLabel = new QLabel();
        moveNumberScrollAreaLabel->setText(QString::number(session->position.fullMovesCount) + '.');
        moveNumberScrollAreaLabel->setAlignment(Qt::AlignLeft);
        movesGridLayout->addWidget(moveNumberScrollAreaLabel, rowIndex, 0);

        movesGridLayout->addWidget(moveLabel, rowIndex, 1);
    }
    else
    {
        movesGridLayout->addWidget(moveLabel, rowIndex, 2);

        if (currentMoveIndex == -1)
        {
            QLabel* moveNumberScrollAreaLabel = new QLabel();
            moveNumberScrollAreaLabel->setText(QString::number(session->position.fullMovesCount) + '.');
            moveNumberScrollAreaLabel->setAlignment(Qt::AlignLeft);
            movesGridLayout->addWidget(moveNumberScrollAreaLabel, rowIndex, 0);

            QLabel* whiteNonExistantMoveLabel = new QLabel();
            whiteNonExistantMoveLabel->setText("-----");
            whiteNonExistantMoveLabel->setAlignment(Qt::AlignLeft);
            movesGridLayout->addWidget(whiteNonExistantMoveLabel, rowIndex, 1);
        }
    }

    QScrollBar* verticalScrollBar = movesScrollArea->verticalScrollBar();
    verticalScrollBar->setValue(verticalScrollBar->maximum());

    if (lastMovePlayedScrollAreaLabel)
    {
        lastMovePlayedScrollAreaLabel->setStyleSheet("");
    }

    moveLabel->setStyleSheet("background-color: #cccccc");
    lastMovePlayedScrollAreaLabel = moveLabel;
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

void MainWindow::UpdateEvaluationLabel(const Evaluation evaluation)
{
    QString newText = "Evaluation: ";

    if (evaluation == POSITIVE_INFINITY_EVALUATION)
    {
        newText.append("1 - 0");
    }
    else if (evaluation == NEGATIVE_INFINITY_EVALUATION)
    {
        newText.append("0 - 1");
    }
    else
    {
        if (evaluation > 0)
        {
            newText.append('+');
        }
        newText.append(QString::number(evaluation / 100.0, 'f', 2));
    }

    evaluationLabel->setText(newText);
}

void MainWindow::UpdateBestMovesLabel(const std::list<FullMove>& bestMovesList)
{
    if (!bestMovesList.size())
    {
        return;
    }

    std::vector<FullMove> bestMovesVector(std::begin(bestMovesList), std::end(bestMovesList));
    QString newText = QString::number(session->position.fullMovesCount) + '.';
    unsigned int movesWritten = 0;
    unsigned int additionalMoveNumber = 0;

    if (session->position.playerToMove == PlayerColor::White)
    {
        newText.append(' ' + QString::fromStdString(MoveStringFormat(bestMovesVector[0], session->position.board)));
        session->MakeMove(bestMovesVector[0]);
        ++movesWritten;

        if (bestMovesVector.size() > 1)
        {
            newText.append(' ' + QString::fromStdString(MoveStringFormat(bestMovesVector[1], session->position.board)));
            session->MakeMove(bestMovesVector[1]);
            ++movesWritten;
        }

        for (; movesWritten < bestMovesVector.size(); ++movesWritten)
        {
            if (movesWritten % 2 == 0)
            {
                newText.append("  " + QString::number(session->position.fullMovesCount + additionalMoveNumber) + '.');
                ++additionalMoveNumber;
            }

            newText.append(' ' + QString::fromStdString(MoveStringFormat(bestMovesVector[movesWritten], session->position.board)));
            session->MakeMove(bestMovesVector[movesWritten]);
        }
    }
    else
    {
        newText.append(" ... " + QString::fromStdString(MoveStringFormat(bestMovesVector[0], session->position.board)));
        session->MakeMove(bestMovesVector[0]);
        ++movesWritten;
        ++additionalMoveNumber;

        for (; movesWritten < bestMovesVector.size(); ++movesWritten)
        {
            if (movesWritten % 2 == 1)
            {
                newText.append(' ' + QString::number(session->position.fullMovesCount + movesWritten / 2) + '.');
                ++additionalMoveNumber;
            }

            newText.append("  " + QString::fromStdString(MoveStringFormat(bestMovesVector[movesWritten], session->position.board)));
            session->MakeMove(bestMovesVector[movesWritten]);
        }
    }

    for (int i = 0; i < bestMovesVector.size(); ++i)
    {
        session->UndoMove();
    }

    bestMovesLabel->setText(newText);
}

void MainWindow::UpdatePositionLabels()
{
    moveNumberLabel->setText("Move: " + QString::number(session->position.fullMovesCount));
    rule50Label->setText("50 move rule status: " + QString::number(session->position.plyClock));
    playerToMoveLabel->setText(QString(session->position.playerToMove == PlayerColor::White ? "White" : "Black") + " to move");

    uint8_t whiteKingside = (uint8_t) session->position.castlingRights & (uint8_t)CastlingRights::WhiteKingside;
    uint8_t whiteQueenside = (uint8_t)session->position.castlingRights & (uint8_t)CastlingRights::WhiteQueenside;
    uint8_t blackKingside = (uint8_t) session->position.castlingRights & (uint8_t)CastlingRights::BlackKingside;
    uint8_t blackQueenside = (uint8_t)session->position.castlingRights & (uint8_t)CastlingRights::BlackQueenside;

    SetCastlingRightsLabel(castlingRightsWhiteLabel, "White", whiteKingside, whiteQueenside);
    SetCastlingRightsLabel(castlingRightsBlackLabel, "Black", blackKingside, blackQueenside);
}

void MainWindow::SetCastlingRightsLabel(QLabel* label, QString textToSet, const uint8_t kingside, const uint8_t queenside)
{
    textToSet.append(": ");

    if (!(kingside || queenside))
    {
        textToSet.append("-----");
    }

    if (kingside) { textToSet.append("Kingside"); }
    if (queenside)
    {
        if (kingside) { textToSet.append(", "); }

        textToSet.append("Queenside");
    }

    label->setText(textToSet);
}

void MainWindow::emitStartEngine(const Position& position)
{
    ++signalsSentWithoutResponse;
    emit StartEngine(session->position);
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
