#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QTextEdit>
#include <QLabel>
#include <QThread>

#include <memory>

#include "FenParser.h"
#include "Session.h"
#include "MovesGenerator.h"
#include "EngineWorker.h"
#include "PieceLabel.h"
#include "SquareFrame.h"
#include "Stack"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, Session* session = nullptr, FenParser* fenParser = nullptr, MovesGenerator* movesGenerator = nullptr, EngineWorker* engineWorker = nullptr);
    ~MainWindow();

    void OnEmptySquareClicked(unsigned int x, unsigned int y);
    void OnPieceClicked(unsigned int x, unsigned int y);

    std::unordered_map<BitPiece, QPixmap> piecesPixmaps;

private:
    Ui::MainWindow *ui;

    // UI stuff
    QGridLayout* chessboardPanel;
    SquareFrame squareFrames[8][8];
    QTextEdit* fenTextEdit;
    QLabel* evaluationLabel;
    QLabel* depthLabel;
    QLabel* bestMovesLabel;
    QLabel* moveNumberLabel;
    QLabel* rule50Label;
    QLabel* playerToMoveLabel;
    QLabel* castlingRightsWhiteLabel;
    QLabel* castlingRightsBlackLabel;
    std::vector<std::unique_ptr<PieceLabel>> piecesLabels;
    const Piece::Type promotionPieces[4] = { Piece::Type::Queen, Piece::Type::Rook, Piece::Type::Knight, Piece::Type::Bishop };

    Square selectedSquare = Square::None;
    Move firstPhaseMove;
    bool firstPhase = true; // Or duck move
    bool duckOnTheBoard;
    std::unique_ptr<std::list<Move>> currentLegalChessMoves;

    const QColor LIGHT_SQUARE_COLOR = QColor(109, 185, 232);
    const QColor DARK_SQUARE_COLOR = QColor(32, 82, 168);
    const QColor SELECTED_LIGHT_SQUARE_COLOR = QColor(79, 147, 190);
    const QColor SELECTED_DARK_SQUARE_COLOR = QColor(32, 61, 109);

    // Engine stuff
    EngineWorker* engineWorker;
    std::unique_ptr<QThread> engineThread;
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
    void UpdateBestMovesLabel(const std::list<FullMove>& bestMovesList);
    void UpdatePositionLabels();
    void SetCastlingRightsLabel(QLabel* label, QString textToSet, const uint8_t kingside, const uint8_t queenside);

private slots:
    void FenUpdateButtonPressed();
    void HandleEngineResult(const Engine::SearchInfo& result);
    void OnBackwardsButtonPressed();

signals:
    void StartEngine(const Position& position);
};
#endif // MAINWINDOW_H
