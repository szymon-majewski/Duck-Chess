#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QTextEdit>
#include <QLabel>
#include <QThread>
#include <QScrollArea>
#include <QAtomicInt>
#include <QPushButton>
#include <QSpinBox>

#include <memory>

#include "FenParser.h"
#include "Session.h"
#include "MovesGenerator.h"
#include "EngineWorker.h"
#include "PieceLabel.h"
#include "SquareFrame.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, Session* session = nullptr, FenParser* fenParser = nullptr, MovesGenerator* movesGenerator = nullptr);
    ~MainWindow();

    void OnEmptySquareClicked(unsigned int x, unsigned int y);
    void OnPieceClicked(unsigned int x, unsigned int y);

    std::unordered_map<BitPiece, QPixmap> piecesPixmaps;

private:
    Ui::MainWindow *ui;

    // UI stuff
    QGridLayout* chessboardPanel;
    SquareFrame squareFrames[8][8];
    QGridLayout* filesLettersLayout;
    QGridLayout* ranksNumbersLayout;
    QPushButton* flipBoardButton;
    QTextEdit* fenTextEdit;
    QLabel* evaluationLabel;
    QSpinBox* depthSpinBox;
    QLabel* timeLabel;
    QLabel* bestMovesLabel;
    QLabel* moveNumberLabel;
    QPushButton* fenUpdateBtn;
    QLabel* rule50Label;
    QLabel* playerToMoveLabel;
    QLabel* castlingRightsWhiteLabel;
    QLabel* castlingRightsBlackLabel;
    QPushButton* gameModeBtn;
    QLabel* playerColorLabel;
    QLabel* playerColorStringLabel;
    QPushButton* forwardsBtn;
    QPushButton* fastForwardsBtn ;
    QPushButton* backwardsBtn;
    QPushButton* fastBackwardsBtn;
    QScrollArea* movesScrollArea;
    std::unique_ptr<QWidget> scrollWidget;
    std::unique_ptr<QGridLayout> movesGridLayout;
    QLabel* lastMovePlayedScrollAreaLabel = nullptr;
    PlayerColor startingPlayer = PlayerColor::White;
    std::vector<std::unique_ptr<PieceLabel>> piecesLabels;
    const Piece::Type promotionPieces[4] = { Piece::Type::Queen, Piece::Type::Rook, Piece::Type::Knight, Piece::Type::Bishop };
    std::pair<int, int> (MainWindow::*coordsByPerspective)(int, int);

    Square selectedSquare = Square::None;
    Move firstPhaseMove;
    bool firstPhase = true; // Or duck move
    bool duckOnTheBoard;
    std::unique_ptr<std::list<Move>> currentLegalChessMoves;
    bool gameMode = false;
    PlayerColor gameModePlayerColor = PlayerColor::None;
    bool whitesPerspective = true;

    const QColor LIGHT_SQUARE_COLOR = QColor(109, 185, 232);
    const QColor DARK_SQUARE_COLOR = QColor(32, 82, 168);
    const QColor SELECTED_LIGHT_SQUARE_COLOR = QColor(79, 147, 190);
    const QColor SELECTED_DARK_SQUARE_COLOR = QColor(32, 61, 109);

    // Engine stuff
    std::unique_ptr<EngineWorker> engineWorker;
    std::unique_ptr<QThread> engineThread;
    int signalsSentWithoutResponse = 0;
    FenParser* fenParser;
    Session* session;
    MovesGenerator* movesGenerator;
    bool gameEnded = false;
    std::vector<FullMove> movesMade;
    int currentMoveIndex = -1;

    // Window methods
    void InitPiecesPixmaps();
    void UpdateChessboard();
    void SelectSquare(unsigned int x, unsigned int y);
    void SelectSquare(Square square);
    void DeselectSquare(unsigned int x, unsigned int y);
    void DeselectSquare(Square square);
    void UpdateEvaluationLabel(const Evaluation evaluation);
    void UpdateTimeLabel(long long time);
    void UpdateBestMovesLabel(const std::list<FullMove>& bestMovesList);
    void UpdatePositionLabels();
    void SetCastlingRightsLabel(QLabel* label, QString textToSet, const uint8_t kingside, const uint8_t queenside);
    void AddMoveToList(const FullMove& move);
    void emitStartEngine(const Position& position);
    std::pair<int, int> whitesPerspectiveCoords(const int x, const int y);
    std::pair<int, int> blacksPerspectiveCoords(const int x, const int y);

private slots:
    void FenUpdateButtonPressed();
    void HandleEngineResult(const Engine::SearchInfo& result, long long time);
    void OnBackwardsButtonPressed();
    void OnForwardsButtonPressed();
    void OnFastBackwardsButtonPressed();
    void OnFastForwardsButtonPressed();
    void OnGameModeButtonPressed();
    void OnFlipBoardButtonPressed();
    void OnDepthSpinBoxValueChanged(int newDepth);

signals:
    void StartEngine(Position position);
};
#endif // MAINWINDOW_H
