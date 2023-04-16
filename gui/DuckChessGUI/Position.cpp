#include "Position.h"
#include "PositionEvaluator.h"

PlayerColor Position::Update(const FullMove& move)
{
    // What piece is moving
    Board::Color movingPieceColor = playerToMove == PlayerColor::White ? Board::Color::White : Board::Color::Black;
    Board::Color opponentsPieceColor = (Board::Color)(1 - movingPieceColor);
    Board::Type movingPieceType = board.FindBoardType(1ULL << move.sourceSquare, movingPieceColor);

    // Check if game is won by either player
    PlayerColor winnerColor = PlayerColor::None;

    // Emptying source sqaure
    ClearBit(board.bitBoards[movingPieceType][movingPieceColor], move.sourceSquare);
    ClearBit(board.bitBoards[Board::Type::All][movingPieceColor], move.sourceSquare);

    int materialMultiplier = movingPieceColor == Board::Color::White ? 1 : -1;
    enPassantTarget = Square::None;

    //Promotions
    Move::AdditionalInfo promotionType;

    if ((promotionType = (Move::AdditionalInfo)((uint16_t)move.additionalInfo & (uint16_t)Move::promotionChecker)) != Move::AdditionalInfo::None)
    {
        switch (promotionType)
        {
            case Move::AdditionalInfo::PromotionToQueen:
            {
                SetBit(board.bitBoards[Board::Type::Queens][movingPieceColor], move.targetSquare);
                materialDisparity += materialMultiplier * (PositionEvaluator::piecesMaterial[Board::Type::Queens] - PositionEvaluator::piecesMaterial[Board::Type::Pawns]);
                break;
            }
            case Move::AdditionalInfo::PromotionToKnight:
            {
                SetBit(board.bitBoards[Board::Type::Knights][movingPieceColor], move.targetSquare);
                materialDisparity += materialMultiplier * (PositionEvaluator::piecesMaterial[Board::Type::Knights] - PositionEvaluator::piecesMaterial[Board::Type::Pawns]);
                break;
            }
            case Move::AdditionalInfo::PromotionToRook:
            {
                SetBit(board.bitBoards[Board::Type::Rooks][movingPieceColor], move.targetSquare);
                materialDisparity += materialMultiplier * (PositionEvaluator::piecesMaterial[Board::Type::Rooks] - PositionEvaluator::piecesMaterial[Board::Type::Pawns]);
                break;
            }
            case Move::AdditionalInfo::PromotionToBishop:
            {
                SetBit(board.bitBoards[Board::Type::Bishops][movingPieceColor], move.targetSquare);
                materialDisparity += materialMultiplier * (PositionEvaluator::piecesMaterial[Board::Type::Bishops] - PositionEvaluator::piecesMaterial[Board::Type::Pawns]);
                break;
            }
        }
    }
    else
    {
        SetBit(board.bitBoards[movingPieceType][movingPieceColor], move.targetSquare);
    }

    SetBit(board.bitBoards[Board::Type::All][movingPieceColor], move.targetSquare);

    uint16_t capturedPiece;

    // Piece was taken
    if ((Move::AdditionalInfo)(capturedPiece = (uint16_t)move.additionalInfo & Move::captureChecker) != Move::AdditionalInfo::None)
    {
        plyClock = 0;

        Board::Type capturedPieceType;

        switch ((Move::AdditionalInfo)capturedPiece)
        {
            case Move::AdditionalInfo::CapturedPawn: { capturedPieceType = Board::Type::Pawns; break; }
            case Move::AdditionalInfo::CapturedBishop: { capturedPieceType = Board::Type::Bishops; break; }
            case Move::AdditionalInfo::CapturedKnight: { capturedPieceType = Board::Type::Knights; break; }
            case Move::AdditionalInfo::CapturedRook: { capturedPieceType = Board::Type::Rooks; break; }
            case Move::AdditionalInfo::CapturedQueen: { capturedPieceType = Board::Type::Queens; break; }
            case Move::AdditionalInfo::CapturedKing: { capturedPieceType = Board::Type::Kings; winnerColor = playerToMove; break; }
        }

        ClearBit(board.bitBoards[capturedPieceType][opponentsPieceColor], move.targetSquare);
        ClearBit(board.bitBoards[Board::Type::All][opponentsPieceColor], move.targetSquare);

        // Material update
        materialDisparity += materialMultiplier * PositionEvaluator::piecesMaterial[capturedPieceType];

        // Losing castling rights if the rook was taken on corner square
        if (capturedPieceType == Board::Type::Rooks)
        {
            if (movingPieceColor == Board::Color::White)
            {
                if (move.targetSquare == Square::H8)
                {
                    castlingRights = (CastlingRights)((uint8_t)castlingRights & ~(uint8_t)CastlingRights::BlackKingside);
                }
                else if (move.targetSquare == Square::A8)
                {
                    castlingRights = (CastlingRights)((uint8_t)castlingRights & ~(uint8_t)CastlingRights::BlackQueenside);
                }
            }
            else
            {
                if (move.targetSquare == Square::H1)
                {
                    castlingRights = (CastlingRights)((uint8_t)castlingRights & ~(uint8_t)CastlingRights::WhiteKingside);
                }
                else if (move.targetSquare == Square::A1)
                {
                    castlingRights = (CastlingRights)((uint8_t)castlingRights & ~(uint8_t)CastlingRights::WhiteQueenside);
                }
            }
        }
    }
    else if (movingPieceType == Board::Type::Pawns)
    {
        plyClock = 0;
    }
    else
    {
        ++plyClock;
    }

    if (move.additionalInfo == Move::AdditionalInfo::EnPassant)
    {
        uint8_t takenPawnSquare;
        // Emptying taken pawn square
        //// White
        if (movingPieceColor == Board::Color::White)
        {
            takenPawnSquare = move.targetSquare - 8;
        }
        //// Black
        else
        {
            takenPawnSquare = move.targetSquare + 8;
        }

        ClearBit(board.bitBoards[Board::Type::Pawns][opponentsPieceColor], takenPawnSquare);
        ClearBit(board.bitBoards[Board::Type::All][opponentsPieceColor], takenPawnSquare);

        // Material update
        materialDisparity += materialMultiplier * PositionEvaluator::piecesMaterial[Board::Type::Pawns];
    }
    else if (move.additionalInfo == Move::AdditionalInfo::PawnDoubleForward)
    {
        enPassantTarget = movingPieceColor == Board::Color::White ? (Square)(move.targetSquare - 8) : (Square)(move.targetSquare + 8);
    }
    else
    {
        UpdateCastling(move.additionalInfo, move.targetSquare, (uint8_t)movingPieceColor);
    }

    // Losing castling rights after king or rook moves
    if (castlingRights != CastlingRights::None)
    {
        if (movingPieceType == Board::Type::Kings)
        {
            if (movingPieceColor == Board::Color::White)
            {
                castlingRights = (CastlingRights)((uint8_t)castlingRights & ~(uint8_t)CastlingRights::White);
            }
            else
            {
                castlingRights = (CastlingRights)((uint8_t)castlingRights & ~(uint8_t)CastlingRights::Black);
            }
        }
        else if (movingPieceType == Board::Type::Rooks)
        {
            // White Kingside
            if (move.sourceSquare == Square::H1)
            {
                castlingRights = (CastlingRights)((uint8_t)castlingRights & ~(uint8_t)CastlingRights::WhiteKingside);
            }
            // White Queenside
            else if (move.sourceSquare == Square::A1)
            {
                castlingRights = (CastlingRights)((uint8_t)castlingRights & ~(uint8_t)CastlingRights::WhiteQueenside);
            }
            // Black Kingside
            else if (move.sourceSquare == Square::H8)
            {
                castlingRights = (CastlingRights)((uint8_t)castlingRights & ~(uint8_t)CastlingRights::BlackKingside);
            }
            // Black Queenside
            else if (move.sourceSquare == Square::A8)
            {
                castlingRights = (CastlingRights)((uint8_t)castlingRights & ~(uint8_t)CastlingRights::BlackQueenside);
            }
        }
    }

    // Move the duck
    board.duck = (1ULL << move.targetDuckSquare);

    // Updating other information

    if (playerToMove == PlayerColor::White)
    {
        playerToMove = PlayerColor::Black;
    }
    else
    {
        playerToMove = PlayerColor::White;
        ++fullMovesCount;
    }

    board.UpdateEmpties();

    return winnerColor;
}

void Position::UndoMove(const MoveMemento::PositionData& revertedMove)
{
    // What piece is moving
    Board::Color movingPieceColor = playerToMove == PlayerColor::White ? Board::Color::Black : Board::Color::White;
    Board::Color opponentsPieceColor = (Board::Color)(1 - movingPieceColor);
    Board::Type movingPieceType = board.FindBoardType(1ULL << revertedMove.move.targetSquare, movingPieceColor);

    int materialMultiplier = movingPieceColor == Board::Color::White ? 1 : -1;

    // Clearing target square
    ClearBit(board.bitBoards[movingPieceType][movingPieceColor], revertedMove.move.targetSquare);
    ClearBit(board.bitBoards[Board::Type::All][movingPieceColor], revertedMove.move.targetSquare);

    // Promotions
    if ((Move::AdditionalInfo)((uint16_t)revertedMove.move.additionalInfo & Move::promotionChecker) != Move::AdditionalInfo::None)
    {
        // Material update
        materialDisparity -= materialMultiplier * (PositionEvaluator::piecesMaterial[movingPieceType] - PositionEvaluator::piecesMaterial[Board::Type::Pawns]);
        movingPieceType = Board::Type::Pawns;
    }

    SetBit(board.bitBoards[movingPieceType][movingPieceColor], revertedMove.move.sourceSquare);
    SetBit(board.bitBoards[Board::Type::All][movingPieceColor], revertedMove.move.sourceSquare);

    // Move the duck
    if (revertedMove.move.sourceDuckSquare != Square::None)
    {
        board.duck = (1ULL << revertedMove.move.sourceDuckSquare);
    }
    else
    {
        board.duck = 0ULL;
    }

    uint16_t capturedPiece;
    Move::AdditionalInfo castlingInfo;

    // Piece was taken
    if ((Move::AdditionalInfo)(capturedPiece = (uint16_t)revertedMove.move.additionalInfo & Move::captureChecker) != Move::AdditionalInfo::None)
    {
        Board::Type capturedPieceType;

        switch ((Move::AdditionalInfo)capturedPiece)
        {
        case Move::AdditionalInfo::CapturedPawn: { capturedPieceType = Board::Type::Pawns; break; }
        case Move::AdditionalInfo::CapturedBishop: { capturedPieceType = Board::Type::Bishops; break; }
        case Move::AdditionalInfo::CapturedKnight: { capturedPieceType = Board::Type::Knights; break; }
        case Move::AdditionalInfo::CapturedRook: { capturedPieceType = Board::Type::Rooks; break; }
        case Move::AdditionalInfo::CapturedQueen: { capturedPieceType = Board::Type::Queens; break; }
        case Move::AdditionalInfo::CapturedKing: { capturedPieceType = Board::Type::Kings; break; }
        }

        //board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)capturedPieceType | (uint8_t)opponentPiecesColor));
        SetBit(board.bitBoards[capturedPieceType][opponentsPieceColor], revertedMove.move.targetSquare);
        SetBit(board.bitBoards[Board::Type::All][opponentsPieceColor], revertedMove.move.targetSquare);

        // Material update
        materialDisparity -= materialMultiplier * PositionEvaluator::piecesMaterial[capturedPieceType];
    }
    else if ((Move::AdditionalInfo)((uint16_t)revertedMove.move.additionalInfo & (uint16_t)Move::AdditionalInfo::EnPassant) != Move::AdditionalInfo::None)
    {
        uint8_t takenPawnSquare;

        // Current player is white, so the move was done by black
        if (playerToMove == PlayerColor::White)
        {
            takenPawnSquare = revertedMove.move.targetSquare + 8;
        }
        else
        {
            takenPawnSquare = revertedMove.move.targetSquare - 8;
        }

        SetBit(board.bitBoards[Board::Type::Pawns][opponentsPieceColor], takenPawnSquare);
        SetBit(board.bitBoards[Board::Type::All][opponentsPieceColor], takenPawnSquare);

        materialDisparity -= materialMultiplier * PositionEvaluator::piecesMaterial[Board::Type::Pawns];
    }
    else if ((castlingInfo = (Move::AdditionalInfo)((uint16_t)revertedMove.move.additionalInfo & Move::castlingChecker)) != Move::AdditionalInfo::None)
    {
        // Moving rook to starting position
        auto& [rookSourceSquare, rookTargetSquare] = Move::ROOK_CASTLING_SQUARES.at(castlingInfo);

        // Emptying rook square
        ClearBit(board.bitBoards[Board::Type::Rooks][movingPieceColor], rookTargetSquare);
        ClearBit(board.bitBoards[Board::Type::All][movingPieceColor], rookTargetSquare);

        // Moving rook
        SetBit(board.bitBoards[Board::Type::Rooks][movingPieceColor], rookSourceSquare);
        SetBit(board.bitBoards[Board::Type::All][movingPieceColor], rookSourceSquare);
    }

    enPassantTarget = revertedMove.enPassantTarget;
    plyClock = revertedMove.plyClock;
    castlingRights = revertedMove.castlingRights;

    if (playerToMove == PlayerColor::White)
    {
        --fullMovesCount;
        playerToMove = PlayerColor::Black;
    }
    else
    {
        playerToMove = PlayerColor::White;
    }

    board.UpdateEmpties();
}

Evaluation Position::CountMaterial()
{
    Evaluation result = 0;
    int multiplier = 1;

    for (int color = 0; color < 2; ++color)
    {
        for (int type = 0; type < 5; ++type) // Counting to 5 because king is 6th
        {
            result += PositionEvaluator::piecesMaterial[(Board::Type)type] *
                      CountSetBits(board.bitBoards[type][color]) *
                      multiplier;
        }

        multiplier *= -1;
    }

    return result;
}

void Position::UpdateCastling(const Move::AdditionalInfo& moveInfo, const Square targetSquare, const uint8_t& playerColor)
{
    Move::AdditionalInfo castlingInfo;

    if ((castlingInfo = (Move::AdditionalInfo)((uint16_t)moveInfo & Move::castlingChecker)) != Move::AdditionalInfo::None)
    {
        auto& [rookSourceSquare, rookTargetSquare] = Move::ROOK_CASTLING_SQUARES.at(castlingInfo);

        // Emptying rook square
        ClearBit(board.bitBoards[Board::Type::Rooks][playerColor], rookSourceSquare);
        ClearBit(board.bitBoards[Board::Type::All][playerColor], rookSourceSquare);

        // Moving rook
        SetBit(board.bitBoards[Board::Type::Rooks][playerColor], rookTargetSquare);
        SetBit(board.bitBoards[Board::Type::All][playerColor], rookTargetSquare);
    }
}
