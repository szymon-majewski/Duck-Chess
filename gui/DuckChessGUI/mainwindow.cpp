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
//    setFixedSize(900, 650);
    setFixedSize(935, 670);

    // INIT
    InitPiecesPixmaps();

    engineWorker = std::make_unique<EngineWorker>(&signalsSentWithoutResponse);
    engineThread = std::make_unique<QThread>();
    engineWorker->moveToThread(engineThread.get());
    connect(engineWorker.get(), SIGNAL(ResultReady(Engine::SearchInfo, long long)), this, SLOT(HandleEngineResult(Engine::SearchInfo, long long)));
    connect(this, SIGNAL(StartEngine(Position)), engineWorker.get(), SLOT(Search(Position)));
    engineThread->start(QThread::TimeCriticalPriority);

    coordsByPerspective = &MainWindow::whitesPerspectiveCoords;

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

    // FILES AND RANKS
    filesLettersLayout = MainWindow::findChild<QGridLayout*>("filesLettersLayout");
    ranksNumbersLayout  = MainWindow::findChild<QGridLayout*>("ranksNumbersLayout");

    QFont font;
    font.setPointSize(10);
    font.setBold(true);

    for (int i = 0; i < 8; i++)
    {
        auto* fileLetter = new QLabel(QChar('A' + i));
        auto* rankNumber = new QLabel(QString::number(8 - i));

        fileLetter->setFont(font);
        fileLetter->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        fileLetter->setMinimumSize(QSize(70, 20));
        fileLetter->setMaximumSize(QSize(70, 20));
        fileLetter->setAlignment(Qt::AlignCenter);

        rankNumber->setFont(font);
        rankNumber->setAlignment(Qt::AlignCenter);

        filesLettersLayout->addWidget(fileLetter, 0, i);
        ranksNumbersLayout->addWidget(rankNumber, i, 0);
    }

    // FLIP BUTTON
    flipBoardButton = MainWindow::findChild<QPushButton*>("flipBoardBtn");
    QPixmap flipBoardPixmap(":/rsc/img/ui/flipBoard.jpg");
    flipBoardButton->setIcon(QIcon(flipBoardPixmap));
    connect(flipBoardButton, SIGNAL(released()), this, SLOT(OnFlipBoardButtonPressed()));

    // ENGINE PANEL
    evaluationLabel = MainWindow::findChild<QLabel*>("evaluationLabel");

    depthSpinBox = MainWindow::findChild<QSpinBox*>("depthSpinBox");
    depthSpinBox->setValue(engineWorker->Depth());
    connect(depthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(OnDepthSpinBoxValueChanged(int)));

    timeLabel = MainWindow::findChild<QLabel*>("timeLabel");
    bestMovesLabel = MainWindow::findChild<QLabel*>("bestMovesLabel");
    moveNumberLabel = MainWindow::findChild<QLabel*>("moveNumberLabel");
    rule50Label = MainWindow::findChild<QLabel*>("rule50Label");
    playerToMoveLabel = MainWindow::findChild<QLabel*>("playerToMoveLabel");
    castlingRightsWhiteLabel = MainWindow::findChild<QLabel*>("castlingRightsWhiteLabel");
    castlingRightsBlackLabel = MainWindow::findChild<QLabel*>("castlingRightsBlackLabel");
    gameModeBtn = MainWindow::findChild<QPushButton*>("gameModeBtn");
    MainWindow::findChild<QHBoxLayout*>("gameModePanel")->setAlignment(Qt::AlignLeft);
    connect(gameModeBtn, SIGNAL(released()), this, SLOT(OnGameModeButtonPressed()));
    playerColorLabel = MainWindow::findChild<QLabel*>("playerColorLabel");
    playerColorLabel->setVisible(false);
    playerColorLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    playerColorLabel->setScaledContents(true);
    playerColorStringLabel = MainWindow::findChild<QLabel*>("playerColorStringLabel");
    playerColorStringLabel->setVisible(false);

    // MOVES PANEL
    movesScrollArea = MainWindow::findChild<QScrollArea*>("movesScrollArea");
    scrollWidget = std::make_unique<QWidget>();
    movesGridLayout = std::make_unique<QGridLayout>();
    movesGridLayout->setAlignment(Qt::AlignTop);
    movesScrollArea->setWidget(scrollWidget.get());
    movesScrollArea->setStyleSheet("QScrollArea {border: 1px solid black; border-radius: 0px; background-color: white; margin: 0px; padding: 0px;}");
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

    forwardsBtn = MainWindow::findChild<QPushButton*>("forwardsBtn");
    fastForwardsBtn = MainWindow::findChild<QPushButton*>("fastForwardsBtn");
    backwardsBtn = MainWindow::findChild<QPushButton*>("backwardsBtn");
    fastBackwardsBtn = MainWindow::findChild<QPushButton*>("fastBackwardsBtn");

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
    fenUpdateBtn = MainWindow::findChild<QPushButton*>("updateFenBtn");
    connect(fenUpdateBtn, SIGNAL(released()), this, SLOT(FenUpdateButtonPressed()));

    //// TEXT EDIT
    fenTextEdit = MainWindow::findChild<QTextEdit*>("fenTextEdit");

    // STARTING POSITION
    session->position = fenParser->ParseFen(STARTING_POSITION_FEN);
    session->position.materialDisparity = session->position.CountMaterial();
    UpdateChessboard();
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
        session->position = fenParser->ParseFen(fenTextEdit->toPlainText().toStdString());
        session->position.materialDisparity = session->position.CountMaterial();
        startingPlayer = session->position.playerToMove;

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
        DeselectPossibleMovesSquares(selectedSquare);
        selectedSquare = Square::None;
    }
    firstPhase = true;

    piecesLabels.clear();

    BitBoard currentBitBoard;
    int currentPieceIndex;

    for (int type = 0; type < 6; ++type)
    {
        for (int color = 0; color < 2; ++color)
        {
            currentBitBoard = session->position.board.bitBoards[type][color];

            while (currentBitBoard)
            {
                currentPieceIndex = IndexOfLSB(currentBitBoard);

                auto [labelX, labelY] = (this->*coordsByPerspective)(currentPieceIndex % 8, currentPieceIndex / 8);
                auto& insertedLabel = piecesLabels.emplace_back(std::make_unique<PieceLabel>(this, this, labelX, labelY));
                insertedLabel->setAlignment(Qt::AlignCenter);
                insertedLabel->setPixmap(piecesPixmaps.at(Board::BOARD_PIECES_TO_BITPIECES_MAP.at({(Board::Type)type, (Board::Color)color})));
                insertedLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
                insertedLabel->setScaledContents(true);
                chessboardPanel->addWidget(insertedLabel.get(), labelY, labelX);

                RemoveLSB(currentBitBoard);
            }
        }
    }

    if (session->position.board.duck)
    {
        duckOnTheBoard = true;

        int duckIndex = IndexOfLSB(session->position.board.duck);
        auto [labelX, labelY] = (this->*coordsByPerspective)(duckIndex % 8, duckIndex / 8);
        auto& insertedLabel = piecesLabels.emplace_back(std::make_unique<PieceLabel>(this, this, labelX, labelY));
        insertedLabel->setAlignment(Qt::AlignCenter);
        insertedLabel->setPixmap(piecesPixmaps.at(Board::BOARD_PIECES_TO_BITPIECES_MAP.at({Board::Type::Duck, Board::Color::Both})));
        insertedLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        insertedLabel->setScaledContents(true);
        chessboardPanel->addWidget(insertedLabel.get(), labelY, labelX);
    }
    else
    {
        duckOnTheBoard = false;
    }
}

void MainWindow::HandleEngineResult(const Engine::SearchInfo& result, long long time)
{
    --signalsSentWithoutResponse;

    if (!gameEnded)
    {
        if (!gameMode)
        {
            UpdateEvaluationLabel(result.evaluation);
            UpdateTimeLabel(time);
            UpdateBestMovesLabel(result.movesPath);
        }
        else if (gameModePlayerColor != session->position.playerToMove)
        {
            auto bestMove = result.movesPath.front();
            if ((Move::AdditionalInfo)((uint16_t)bestMove.additionalInfo & (uint16_t)Move::AdditionalInfo::CapturedKing) != Move::AdditionalInfo::None)
            {
                bestMove.sourceDuckSquare = bestMove.targetDuckSquare = Square::None;
                AddMoveToList(bestMove);
                movesMade.emplace_back(bestMove);
                ++currentMoveIndex;

                PieceLabel* movingPieceLabel;
                unsigned int takenPieceIndex;
                unsigned int breakChecker = 0;
                int sourceSquareX;
                int sourceSquareY;
                int targetSquareX;
                int targetSquareY;

                SquareToBoardIndices(bestMove.sourceSquare, sourceSquareY, sourceSquareX);
                SquareToBoardIndices(bestMove.targetSquare, targetSquareY, targetSquareX);

                auto [guiSourceX, guiSourceY] = (this->*coordsByPerspective)(sourceSquareX, sourceSquareY);
                auto [guiTargetX, guiTargetY] = (this->*coordsByPerspective)(targetSquareX, targetSquareY);

                // Piece which will take the king
                for (int i = 0; i < piecesLabels.size() && breakChecker < 2; ++i)
                {
                    if (guiSourceX == piecesLabels[i]->x && guiSourceY == piecesLabels[i]->y)
                    {
                        piecesLabels[i]->x = guiTargetX;
                        piecesLabels[i]->y = guiTargetY;
                        movingPieceLabel = piecesLabels[i].get();
                        ++breakChecker;
                    }
                    else if (guiTargetX == piecesLabels[i]->x && guiTargetY == piecesLabels[i]->y)
                    {
                        takenPieceIndex = i;
                        ++breakChecker;
                    }
                }

                piecesLabels.erase(piecesLabels.begin() + takenPieceIndex);

                chessboardPanel->removeWidget(movingPieceLabel);
                chessboardPanel->addWidget(movingPieceLabel, guiTargetY, guiTargetX);

                gameEnded = true;
            }
            else
            {
                AddMoveToList(result.movesPath.front());
                session->MakeMove(result.movesPath.front());
                movesMade.emplace_back(result.movesPath.front());
                ++currentMoveIndex;

                UpdateChessboard();
            }

            UpdateTimeLabel(time);
        }
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

        auto [guiSourceX, guiSourceY] = (this->*coordsByPerspective)(sourceSquareX, sourceSquareY);
        auto [guiTargetX, guiTargetY] = (this->*coordsByPerspective)(targetSquareX, targetSquareY);

        // Piece which took the king
        for (const std::unique_ptr<PieceLabel>& pieceLabel : piecesLabels)
        {
            if (guiTargetX == pieceLabel->x && guiTargetY == pieceLabel->y)
            {
                pieceLabel->x = guiSourceX;
                pieceLabel->y = guiSourceY;
                movingPieceLabel = pieceLabel.get();
                break;
            }
        }

        chessboardPanel->removeWidget(movingPieceLabel);
        chessboardPanel->addWidget(movingPieceLabel, guiSourceY, guiSourceX);

        // King itself
        auto& insertedLabel = piecesLabels.emplace_back(std::make_unique<PieceLabel>(this, this, guiTargetX, guiTargetY));
        insertedLabel->setAlignment(Qt::AlignCenter);
        insertedLabel->setPixmap(piecesPixmaps.at((uint8_t)PieceType::King | (uint8_t)opponentsColor));
        insertedLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        insertedLabel->setScaledContents(true);
        chessboardPanel->addWidget(insertedLabel.get(), guiTargetY, guiTargetX);

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

        auto [guiSourceX, guiSourceY] = (this->*coordsByPerspective)(sourceSquareX, sourceSquareY);
        auto [guiTargetX, guiTargetY] = (this->*coordsByPerspective)(targetSquareX, targetSquareY);

        // Piece which will take the king
        for (int i = 0; i < piecesLabels.size() && breakChecker < 2; ++i)
        {
            if (guiSourceX == piecesLabels[i]->x && guiSourceY == piecesLabels[i]->y)
            {
                piecesLabels[i]->x = guiTargetX;
                piecesLabels[i]->y = guiTargetY;
                movingPieceLabel = piecesLabels[i].get();
                ++breakChecker;
            }
            else if (guiTargetX == piecesLabels[i]->x && guiTargetY == piecesLabels[i]->y)
            {
                takenPieceIndex = i;
                ++breakChecker;
            }
        }

        piecesLabels.erase(piecesLabels.begin() + takenPieceIndex);

        chessboardPanel->removeWidget(movingPieceLabel);
        chessboardPanel->addWidget(movingPieceLabel, guiTargetY, guiTargetX);

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

        auto [guiSourceX, guiSourceY] = (this->*coordsByPerspective)(sourceSquareX, sourceSquareY);
        auto [guiTargetX, guiTargetY] = (this->*coordsByPerspective)(targetSquareX, targetSquareY);

        // Piece which will take the king
        for (int i = 0; i < piecesLabels.size() && breakChecker < 2; ++i)
        {
            if (guiSourceX == piecesLabels[i]->x && guiSourceY == piecesLabels[i]->y)
            {
                piecesLabels[i]->x = guiTargetX;
                piecesLabels[i]->y = guiTargetY;
                movingPieceLabel = piecesLabels[i].get();
                ++breakChecker;
            }
            else if (guiTargetX == piecesLabels[i]->x && guiTargetY == piecesLabels[i]->y)
            {
                takenPieceIndex = i;
                ++breakChecker;
            }
        }

        piecesLabels.erase(piecesLabels.begin() + takenPieceIndex);

        chessboardPanel->removeWidget(movingPieceLabel);
        chessboardPanel->addWidget(movingPieceLabel, guiTargetY, guiTargetX);

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

    if (gameEnded || (gameMode && gameModePlayerColor != session->position.playerToMove))
    {
        return;
    }

    auto [engineTargetX, engineTargetY] = (this->*coordsByPerspective)(x, y);

    // Piece was selected earlier and wants to move to an empty square
    if (selectedSquare != Square::None)
    {
        int sourceSquareX;
        int sourceSquareY;
        SquareToBoardIndices(selectedSquare, sourceSquareY, sourceSquareX);

        auto guiSourceCoords = (this->*coordsByPerspective)(sourceSquareX, sourceSquareY);
        auto guiSourceX = guiSourceCoords.first;
        auto guiSourceY = guiSourceCoords.second;

        Square targetSquare = BoardIndicesToSquare(engineTargetY, engineTargetX);
        PlayerColor movingPieceColor = session->position.playerToMove;

        if (firstPhase)
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

                if (movingPieceColor == PlayerColor::White && whitesPerspective ||
                    movingPieceColor == PlayerColor::Black && !whitesPerspective)
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
                    PieceType promotionPiece = promotionPieces[i];
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
                                    if (guiSourceX == pieceLabel->x && guiSourceY == pieceLabel->y)
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
                                    case PieceType::Queen:
                                    {
                                        promotionType = Move::AdditionalInfo::PromotionToQueen;
                                        break;
                                    }
                                    case PieceType::Knight:
                                    {
                                        promotionType = Move::AdditionalInfo::PromotionToKnight;
                                        break;
                                    }
                                    case PieceType::Rook:
                                    {
                                        promotionType = Move::AdditionalInfo::PromotionToRook;
                                        break;
                                    }
                                    case PieceType::Bishop:
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

                auto [guiRookSourceX, guiRookSourceY] = (this->*coordsByPerspective)(rookSourceX, rookSourceY);
                auto [guiRookTargetX, guiRookTargetY] = (this->*coordsByPerspective)(rookTargetX, rookTargetY);

                for (const std::unique_ptr<PieceLabel>& pieceLabel : piecesLabels)
                {
                    if (guiRookSourceX == pieceLabel->x && guiRookSourceY == pieceLabel->y)
                    {
                        pieceLabel->x = guiRookTargetX;
                        pieceLabel->y = guiRookTargetY;
                        rookLabel = pieceLabel.get();
                        break;
                    }
                }

                chessboardPanel->removeWidget(rookLabel);
                chessboardPanel->addWidget(rookLabel, guiRookTargetY, guiRookTargetX);
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
            if (guiSourceX == pieceLabel->x && guiSourceY == pieceLabel->y)
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

        DeselectSquare(guiSourceX, guiSourceY);
        DeselectPossibleMovesSquares(selectedSquare);
        selectedSquare = Square::None;
        firstPhase = !firstPhase;
    }
    // It's first move, duck is not on the board, so just place it on the clicked square
    else if (!duckOnTheBoard && !firstPhase)
    {
        //////////////////////////////////////////////////////////////
        auto& duckLabel = piecesLabels.emplace_back(std::make_unique<PieceLabel>(this, this, x, y));
        duckLabel->setAlignment(Qt::AlignCenter);
        duckLabel->setPixmap(piecesPixmaps.at((uint8_t)PieceType::Duck | (uint8_t)PieceColor::Both));
        duckLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        duckLabel->setScaledContents(true);
        chessboardPanel->addWidget(duckLabel.get(), y, x);
        //////////////////////////////////////////////////////////////

        firstPhase = true;
        duckOnTheBoard = true;

        auto newMove = FullMove(firstPhaseMove, selectedSquare, BoardIndicesToSquare(engineTargetY, engineTargetX));

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

    if (gameEnded || (gameMode && gameModePlayerColor != session->position.playerToMove))
    {
        return;
    }

    auto [engineTargetX, engineTargetY] = (this->*coordsByPerspective)(x, y);

    if (!firstPhase && BoardIndicesToSquare(engineTargetY, engineTargetX) == firstPhaseMove.targetSquare)
    {
        return;
    }

    uint8_t clickedPieceColor;

    if (CheckBit(session->position.board.bitBoards[Board::Type::All][Board::Color::White], engineTargetY, engineTargetX))
    {
        clickedPieceColor = (uint8_t)PieceColor::White;
    }
    else if (CheckBit(session->position.board.bitBoards[Board::Type::All][Board::Color::Black], engineTargetY, engineTargetX))
    {
        clickedPieceColor = (uint8_t)PieceColor::Black;
    }
    else
    {
        clickedPieceColor = (uint8_t)PieceColor::Both;
    }

    //uint8_t clickedPieceColor = (uint8_t)session->position.board.pieces[engineTargetY][engineTargetX].PieceColor();

    // No piece was selected earlier
    if (selectedSquare == Square::None)
    {
        // Color of clicked piece matches player that is to move
        if (firstPhase && clickedPieceColor == (uint8_t)session->position.playerToMove)
        {
            selectedSquare = BoardIndicesToSquare(engineTargetY, engineTargetX);
            SelectSquare(x, y);
            SelectPossibleMovesSquares(selectedSquare);
        }
        // Or it's duck move and duck was clicked
        else if (!firstPhase && clickedPieceColor == (uint8_t)PieceColor::Both)
        {
            selectedSquare = BoardIndicesToSquare(engineTargetY, engineTargetX);
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

            auto guiSourceCoords = (this->*coordsByPerspective)(sourceSquareX, sourceSquareY);
            auto guiSourceX = guiSourceCoords.first;
            auto guiSourceY = guiSourceCoords.second;

            PlayerColor opponentsColor = session->position.playerToMove == PlayerColor::White ? PlayerColor::Black : PlayerColor::White;
            PlayerColor movingPieceColor = session->position.playerToMove;

            // Selected piece wants to take enemy piece
            if (clickedPieceColor == (uint8_t)opponentsColor)
            {
                Square targetSquare = BoardIndicesToSquare(engineTargetY, engineTargetX);
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
                        PieceType promotionPiece = promotionPieces[i];
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
                                        if (guiSourceX == pieceLabel->x && guiSourceY == pieceLabel->y)
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
                                        case PieceType::Queen:
                                        {
                                            promotionType = Move::AdditionalInfo::PromotionToQueen;
                                            break;
                                        }
                                        case PieceType::Knight:
                                        {
                                            promotionType = Move::AdditionalInfo::PromotionToKnight;
                                            break;
                                        }
                                        case PieceType::Rook:
                                        {
                                            promotionType = Move::AdditionalInfo::PromotionToRook;
                                            break;
                                        }
                                        case PieceType::Bishop:
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
                    if (guiSourceX == piecesLabels[i]->x && guiSourceY == piecesLabels[i]->y)
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

                DeselectSquare(guiSourceX, guiSourceY);
                DeselectPossibleMovesSquares(selectedSquare);
                selectedSquare = Square::None;
                firstPhase = false;
            }
            // Change selected piece
            else if (clickedPieceColor == (uint8_t)session->position.playerToMove)
            {
                int sourceSqaureX;
                int sourceSqaureY;
                SquareToBoardIndices(selectedSquare, sourceSqaureY, sourceSqaureX);
                auto [guiSourceX, guiSourceY] = (this->*coordsByPerspective)(sourceSquareX, sourceSquareY);

                DeselectSquare(guiSourceX, guiSourceY);
                DeselectPossibleMovesSquares(selectedSquare);

                selectedSquare = BoardIndicesToSquare(engineTargetY, engineTargetX);
                SelectSquare(x, y);
                SelectPossibleMovesSquares(selectedSquare);
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
    auto [guiSquareX, guiSquareY] = (this->*coordsByPerspective)(x, y);

    SelectSquare(guiSquareX, guiSquareY);
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
    auto [guiSquareX, guiSquareY] = (this->*coordsByPerspective)(x, y);

    DeselectSquare(guiSquareX, guiSquareY);
}

void MainWindow::SelectPossibleMoveSquare(Square square)
{
    int x;
    int y;

    SquareToBoardIndices(square, y, x);
    auto [guiSquareX, guiSquareY] = (this->*coordsByPerspective)(x, y);

    if ((guiSquareX + guiSquareY) % 2 == 0)
    {
        squareFrames[guiSquareX][guiSquareY].setStyleSheet("background-color: " + POSSIBLE_MOVE_LIGHT_SQUARE.name());
    }
    else
    {
        squareFrames[guiSquareX][guiSquareY].setStyleSheet("background-color: " + POSSIBLE_MOVE_DARK_SQUARE.name());
    }

}

void MainWindow::SelectPossibleMovesSquares(Square sourceSquare)
{
    for (const Move& move : *currentLegalChessMoves)
    {
        if (move.sourceSquare == sourceSquare)
        {
            SelectPossibleMoveSquare(move.targetSquare);
        }
    }
}

void MainWindow::DeselectPossibleMovesSquares(Square sourceSquare)
{
    for (const Move& move : *currentLegalChessMoves)
    {
        if (move.sourceSquare == sourceSquare)
        {
            DeselectSquare(move.targetSquare);
        }
    }
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

void MainWindow::UpdateTimeLabel(long long time)
{
    timeLabel->setText("Time: " + QString::number(time) + " ms");
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

void MainWindow::OnGameModeButtonPressed()
{
    if (gameMode)
    {
        gameMode = false;
        gameModePlayerColor = PlayerColor::None;
        gameModeBtn->setText("Game mode: Off");

        if (!gameEnded)
        {
            emitStartEngine(session->position);
            UpdateChessboard();
        }

        forwardsBtn->setEnabled(true);
        fastForwardsBtn->setEnabled(true);
        backwardsBtn->setEnabled(true);
        fastBackwardsBtn->setEnabled(true);
        fenUpdateBtn->setEnabled(true);
        playerColorLabel->setVisible(false);
        playerColorStringLabel->setVisible(false);
    }
    else
    {
        QDialog gameModeDialog;
        gameModeDialog.setWindowFlags(Qt::CustomizeWindowHint /*| Qt::WindowTitleHint*/);
        QGridLayout layout(&gameModeDialog);
        QPushButton colorButtons[2];

        for (int i = 0; i < 2; ++i)
        {
            uint8_t buttonColor = (uint8_t)PlayerColor::White << i;
            colorButtons[i].setIcon(QIcon(piecesPixmaps.at(buttonColor | (uint8_t)PieceType::King)));
            colorButtons[i].setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            colorButtons[i].setMinimumSize(QSize(70, 70));
            colorButtons[i].setMaximumSize(QSize(70, 70));
            colorButtons[i].setIconSize(colorButtons[i].size());
            layout.addWidget(colorButtons + i, 0, i);
            connect(&colorButtons[i], &QPushButton::clicked, this,
                    [&, buttonColor]()
                    {
                        playerColorLabel->setPixmap(piecesPixmaps.at(buttonColor | (uint8_t)PieceType::King));
                        gameModePlayerColor = (PlayerColor)buttonColor;
                        gameModeDialog.accept();
                    });
        }

        gameModeDialog.exec();

        gameMode = true;
        playerColorLabel->setVisible(true);
        playerColorStringLabel->setVisible(true);
        gameModeBtn->setText("Game mode: On");
        evaluationLabel ->setText("Evaluation: -----");
        bestMovesLabel ->setText("-----");

        if (gameModePlayerColor != session->position.playerToMove)
        {
            emitStartEngine(session->position);
        }

        UpdateChessboard();

        forwardsBtn->setEnabled(false);
        fastForwardsBtn->setEnabled(false);
        backwardsBtn->setEnabled(false);
        fastBackwardsBtn->setEnabled(false);
        fenUpdateBtn->setEnabled(false);

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
    }
}

void MainWindow::OnFlipBoardButtonPressed()
{
    // Flipping ranks and files symbols
    for (int i = 0; i < 8; ++i)
    {
        QLabel* fileLetter = qobject_cast<QLabel*>(filesLettersLayout->itemAtPosition(0, i)->widget());
        QLabel* rankNumber = qobject_cast<QLabel*>(ranksNumbersLayout->itemAtPosition(i, 0)->widget());
        char newFileLetter = 137 - fileLetter->text()[0].toLatin1();
        char newRankNumber = 105 - rankNumber->text()[0].toLatin1();

        fileLetter->setText(QString::fromLatin1(&newFileLetter, 1));
        rankNumber->setText(QString::fromLatin1(&newRankNumber, 1));
    }

    if (selectedSquare != Square::None)
    {
        DeselectSquare(selectedSquare);
        DeselectPossibleMovesSquares(selectedSquare);
        selectedSquare = Square::None;
    }

    if (whitesPerspective)
    {
        coordsByPerspective = &MainWindow::blacksPerspectiveCoords;
        whitesPerspective = false;
    }
    else
    {
        coordsByPerspective = &MainWindow::whitesPerspectiveCoords;
        whitesPerspective = true;
    }

    firstPhase = true;
    piecesLabels.clear();

    BitBoard currentBitBoard;
    int currentPieceIndex;

    for (int type = 0; type < 6; ++type)
    {
        for (int color = 0; color < 2; ++color)
        {
            currentBitBoard = session->position.board.bitBoards[type][color];

            while (currentBitBoard)
            {
                currentPieceIndex = IndexOfLSB(currentBitBoard);

                auto [labelX, labelY] = (this->*coordsByPerspective)(currentPieceIndex % 8, currentPieceIndex / 8);
                auto& insertedLabel = piecesLabels.emplace_back(std::make_unique<PieceLabel>(this, this, labelX, labelY));
                insertedLabel->setAlignment(Qt::AlignCenter);
                insertedLabel->setPixmap(piecesPixmaps.at(Board::BOARD_PIECES_TO_BITPIECES_MAP.at({(Board::Type)type, (Board::Color)color})));
                insertedLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
                insertedLabel->setScaledContents(true);
                chessboardPanel->addWidget(insertedLabel.get(), labelY, labelX);

                RemoveLSB(currentBitBoard);
            }
        }
    }

    if (session->position.board.duck)
    {
        duckOnTheBoard = true;

        int duckIndex = IndexOfLSB(session->position.board.duck);
        auto [labelX, labelY] = (this->*coordsByPerspective)(duckIndex % 8, duckIndex / 8);
        auto& insertedLabel = piecesLabels.emplace_back(std::make_unique<PieceLabel>(this, this, labelX, labelY));
        insertedLabel->setAlignment(Qt::AlignCenter);
        insertedLabel->setPixmap(piecesPixmaps.at(Board::BOARD_PIECES_TO_BITPIECES_MAP.at({Board::Type::Duck, Board::Color::Both})));
        insertedLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        insertedLabel->setScaledContents(true);
        chessboardPanel->addWidget(insertedLabel.get(), labelY, labelX);
    }
    else
    {
        duckOnTheBoard = false;
    }

    if (gameEnded)
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

        auto [guiSourceX, guiSourceY] = (this->*coordsByPerspective)(sourceSquareX, sourceSquareY);
        auto [guiTargetX, guiTargetY] = (this->*coordsByPerspective)(targetSquareX, targetSquareY);

        // Piece which will take the king
        for (int i = 0; i < piecesLabels.size() && breakChecker < 2; ++i)
        {
            if (guiSourceX == piecesLabels[i]->x && guiSourceY == piecesLabels[i]->y)
            {
                piecesLabels[i]->x = guiTargetX;
                piecesLabels[i]->y = guiTargetY;
                movingPieceLabel = piecesLabels[i].get();
                ++breakChecker;
            }
            else if (guiTargetX == piecesLabels[i]->x && guiTargetY == piecesLabels[i]->y)
            {
                takenPieceIndex = i;
                ++breakChecker;
            }
        }

        piecesLabels.erase(piecesLabels.begin() + takenPieceIndex);

        chessboardPanel->removeWidget(movingPieceLabel);
        chessboardPanel->addWidget(movingPieceLabel, guiTargetY, guiTargetX);
    }
}

std::pair<int, int> MainWindow::whitesPerspectiveCoords(const int x, const int y)
{
    return std::make_pair(x, 7 - y);
}

std::pair<int, int> MainWindow::blacksPerspectiveCoords(const int x, const int y)
{
    return std::make_pair(7 - x, y);
}

void MainWindow::emitStartEngine(const Position& position)
{
    ++signalsSentWithoutResponse;
    emit StartEngine(session->position);
}

void MainWindow::OnDepthSpinBoxValueChanged(int newDepth)
{
    engineWorker->SetDepth(newDepth);

    if (!gameEnded)
    {
        emitStartEngine(session->position);
    }
}

void MainWindow::InitPiecesPixmaps()
{
    piecesPixmaps =
    {
        { (uint8_t)PieceType::Pawn | (uint8_t)PieceColor::Black, QPixmap(":/rsc/img/pieces/black_pawn.png") },
        { (uint8_t)PieceType::Pawn | (uint8_t)PieceColor::White, QPixmap(":/rsc/img/pieces/white_pawn.png") },
        { (uint8_t)PieceType::Knight | (uint8_t)PieceColor::Black, QPixmap(":/rsc/img/pieces/black_knight.png") },
        { (uint8_t)PieceType::Knight | (uint8_t)PieceColor::White, QPixmap(":/rsc/img/pieces/white_knight.png") },
        { (uint8_t)PieceType::Bishop | (uint8_t)PieceColor::Black, QPixmap(":/rsc/img/pieces/black_bishop.png") },
        { (uint8_t)PieceType::Bishop | (uint8_t)PieceColor::White, QPixmap(":/rsc/img/pieces/white_bishop.png") },
        { (uint8_t)PieceType::Rook | (uint8_t)PieceColor::Black, QPixmap(":/rsc/img/pieces/black_rook.png") },
        { (uint8_t)PieceType::Rook | (uint8_t)PieceColor::White, QPixmap(":/rsc/img/pieces/white_rook.png") },
        { (uint8_t)PieceType::Queen | (uint8_t)PieceColor::Black, QPixmap(":/rsc/img/pieces/black_queen.png") },
        { (uint8_t)PieceType::Queen | (uint8_t)PieceColor::White, QPixmap(":/rsc/img/pieces/white_queen.png") },
        { (uint8_t)PieceType::King | (uint8_t)PieceColor::Black, QPixmap(":/rsc/img/pieces/black_king.png") },
        { (uint8_t)PieceType::King | (uint8_t)PieceColor::White, QPixmap(":/rsc/img/pieces/white_king.png") },
        { (uint8_t)PieceType::Duck | (uint8_t)PieceColor::Both, QPixmap(":/rsc/img/pieces/duck.png") }
    };
}
