#include "Position.h"
#include "PositionEvaluator.h"

extern void SquareToBoardIndices(const Square& square, int& y, int& x);
Square BoardIndicesToSquare(const unsigned& y, const unsigned& x);

PlayerColor Position::Update(const FullMove& move)
{
	int sourceY;
	int sourceX;
	int targetY;
	int targetX;

	SquareToBoardIndices(move.sourceSquare, sourceY, sourceX);
	SquareToBoardIndices(move.targetSquare, targetY, targetX);

	// What piece is moving
	Piece::Type movingPieceType = board.pieces[sourceY][sourceX].PieceType();
	Piece::Color movingPieceColor = board.pieces[sourceY][sourceX].PieceColor();

	// Check if game is won by either player
	PlayerColor winnerColor = PlayerColor::None;

	if (board.pieces[targetY][targetX].PieceType() == Piece::Type::King)
	{
		winnerColor = (PlayerColor)movingPieceColor;
	}

	// Update some position data
	enPassantTarget = Square::None;
	if (movingPieceType == Piece::Type::Pawn || board.pieces[targetY][targetX].PieceType() != Piece::Type::None)
	{
		plyClock = 0;
	}
	else
	{
		++plyClock;
	}

	// Emptying source sqaure
	board.pieces[sourceY][sourceX].SetBitPiece(NO_PIECE);

	int materialMultiplier = movingPieceColor == Piece::Color::White ? 1 : -1;
	uint16_t capturedPiece;

	if (move.additionalInfo == Move::AdditionalInfo::None)
	{
		board.pieces[targetY][targetX].SetBitPiece((uint8_t)movingPieceType | (uint8_t)movingPieceColor);
	}

	// Piece was taken
	if ((Move::AdditionalInfo)(capturedPiece = (uint16_t)move.additionalInfo & Move::captureChecker) != Move::AdditionalInfo::None)
	{
		Piece::Type capturedPieceType;

		switch ((Move::AdditionalInfo)capturedPiece)
		{
			case Move::AdditionalInfo::CapturedPawn: { capturedPieceType = Piece::Type::Pawn; break; }
			case Move::AdditionalInfo::CapturedBishop: { capturedPieceType = Piece::Type::Bishop; break; }
			case Move::AdditionalInfo::CapturedKnight: { capturedPieceType = Piece::Type::Knight; break; }
			case Move::AdditionalInfo::CapturedRook: { capturedPieceType = Piece::Type::Rook; break; }
			case Move::AdditionalInfo::CapturedQueen: { capturedPieceType = Piece::Type::Queen; break; }
			case Move::AdditionalInfo::CapturedKing: { capturedPieceType = Piece::Type::King; break; }
			default: { capturedPieceType = Piece::Type::None; }
		}

		board.pieces[targetY][targetX].SetBitPiece((uint8_t)movingPieceType | (uint8_t)movingPieceColor);

		// Material update 
		materialDisparity += materialMultiplier * PositionEvaluator::piecesMaterial[capturedPieceType];

		// Losing castling rights if the rook was taken on corner square
		if (capturedPieceType == Piece::Type::Rook)
		{
			if (movingPieceColor == Piece::Color::White)
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

	//Promotions
	Move::AdditionalInfo promotionType;

	if ((promotionType = (Move::AdditionalInfo)((uint16_t)move.additionalInfo & (uint16_t)Move::promotionChecker)) != Move::AdditionalInfo::None)
	{
		switch (promotionType)
		{
			case Move::AdditionalInfo::PromotionToQueen:
			{
				board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::Queen | (uint8_t)movingPieceColor));
				materialDisparity += materialMultiplier * (PositionEvaluator::piecesMaterial[Piece::Type::Queen] - PositionEvaluator::piecesMaterial[Piece::Type::Pawn]);
				break;
			}
			case Move::AdditionalInfo::PromotionToRook:
			{
				board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::Rook | (uint8_t)movingPieceColor));
				materialDisparity += materialMultiplier * (PositionEvaluator::piecesMaterial[Piece::Type::Rook] - PositionEvaluator::piecesMaterial[Piece::Type::Pawn]);
				break;
			}
			case Move::AdditionalInfo::PromotionToKnight:
			{
				board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::Knight | (uint8_t)movingPieceColor));
				materialDisparity += materialMultiplier * (PositionEvaluator::piecesMaterial[Piece::Type::Knight] - PositionEvaluator::piecesMaterial[Piece::Type::Pawn]);
				break;
			}
			case Move::AdditionalInfo::PromotionToBishop:
			{
				board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::Bishop | (uint8_t)movingPieceColor));
				materialDisparity += materialMultiplier * (PositionEvaluator::piecesMaterial[Piece::Type::Bishop] - PositionEvaluator::piecesMaterial[Piece::Type::Pawn]);
				break;
			}
		}
	}
	else if (move.additionalInfo == Move::AdditionalInfo::EnPassant)
	{
		// Emptying taken pawn square
		//// Figuring out if it's white or black who took
		//// White
		if (targetY > sourceY)
		{
			board.pieces[targetY - 1][targetX].SetBitPiece(NO_PIECE);
		}
		//// Black
		else
		{
			board.pieces[targetY + 1][targetX].SetBitPiece(NO_PIECE);
		}

		board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::Pawn | (uint8_t)movingPieceColor));

		// Material update 
		materialDisparity += materialMultiplier * PositionEvaluator::piecesMaterial[Piece::Type::Pawn];
	}
	else if (move.additionalInfo == Move::AdditionalInfo::PawnDoubleForward)
	{
		board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)movingPieceType | (uint8_t)movingPieceColor));
		enPassantTarget = movingPieceColor == Piece::Color::White ? BoardIndicesToSquare(targetY - 1, targetX) : BoardIndicesToSquare(targetY + 1, targetX);
	}
	else
	{
		UpdateCastling(move.additionalInfo, targetY, targetX, (uint8_t)movingPieceColor);
	}

	// Losing castling rights after king or rook moves
	if (castlingRights != CastlingRights::None)
	{
		if (movingPieceType == Piece::Type::King)
		{
			if (movingPieceColor == Piece::Color::White)
			{
				castlingRights = (CastlingRights)((uint8_t)castlingRights & ~(uint8_t)CastlingRights::White);
			}
			else
			{
				castlingRights = (CastlingRights)((uint8_t)castlingRights & ~(uint8_t)CastlingRights::Black);
			}
		}
		else if (movingPieceType == Piece::Type::Rook)
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
	SquareToBoardIndices(move.sourceDuckSquare, sourceY, sourceX);
	SquareToBoardIndices(move.targetDuckSquare, targetY, targetX);

	board.pieces[sourceY][sourceX].SetBitPiece(NO_PIECE);
	board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::Duck | (uint8_t)Piece::Color::Both));

	// Updating other information (apart from new en passant target and playerToMove - it's done earlier)
	playerToMove = (playerToMove == PlayerColor::White ? PlayerColor::Black : PlayerColor::White);

	if (movingPieceColor == Piece::Color::Black)
	{
		++fullMovesCount;
	}

	return winnerColor;
}

void Position::UndoMove(const MoveMemento::PositionData& revertedMove)
{
	int sourceY;
	int sourceX;
	int targetY;
	int targetX;
	int duckSourceY;
	int duckSourceX;
	int duckTargetY;
	int duckTargetX;

	SquareToBoardIndices(revertedMove.move.sourceSquare, sourceY, sourceX);
	SquareToBoardIndices(revertedMove.move.targetSquare, targetY, targetX);
	SquareToBoardIndices(revertedMove.move.sourceDuckSquare, duckSourceY, duckSourceX);
	SquareToBoardIndices(revertedMove.move.targetDuckSquare, duckTargetY, duckTargetX);

	// What piece is moving
	Piece::Type movingPieceType = board.pieces[targetY][targetX].PieceType();
	Piece::Color movingPieceColor = board.pieces[targetY][targetX].PieceColor();

	int materialMultiplier = movingPieceColor == Piece::Color::White ? 1 : -1;

	// Promotions
	if ((Move::AdditionalInfo)((uint16_t)revertedMove.move.additionalInfo & Move::promotionChecker) != Move::AdditionalInfo::None)
	{
		// Material update
		materialDisparity -= materialMultiplier * (PositionEvaluator::piecesMaterial[movingPieceType] - PositionEvaluator::piecesMaterial[Piece::Type::Pawn]);

		movingPieceType = Piece::Type::Pawn;
	}

	Piece::Color opponentPiecesColor = movingPieceColor == Piece::Color::White ? Piece::Color::Black : Piece::Color::White;
	uint16_t capturedPiece;

	// Clearing target duck square
	board.pieces[duckTargetY][duckTargetX].SetBitPiece(NO_PIECE);

	// Piece was taken
	if ((Move::AdditionalInfo)(capturedPiece = (uint16_t)revertedMove.move.additionalInfo & Move::captureChecker) != Move::AdditionalInfo::None)
	{
		Piece::Type capturedPieceType;

		switch ((Move::AdditionalInfo)capturedPiece)
		{
		case Move::AdditionalInfo::CapturedPawn: { capturedPieceType = Piece::Type::Pawn; break; }
		case Move::AdditionalInfo::CapturedBishop: { capturedPieceType = Piece::Type::Bishop; break; }
		case Move::AdditionalInfo::CapturedKnight: { capturedPieceType = Piece::Type::Knight; break; }
		case Move::AdditionalInfo::CapturedRook: { capturedPieceType = Piece::Type::Rook; break; }
		case Move::AdditionalInfo::CapturedQueen: { capturedPieceType = Piece::Type::Queen; break; }
		case Move::AdditionalInfo::CapturedKing: { capturedPieceType = Piece::Type::King; break; }
		default: { capturedPieceType = Piece::Type::None; }
		}

		board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)capturedPieceType | (uint8_t)opponentPiecesColor));

		// Material update 
		materialDisparity -= materialMultiplier * PositionEvaluator::piecesMaterial[capturedPieceType];
	}
	else
	{
		board.pieces[targetY][targetX].SetBitPiece(NO_PIECE);

		if ((Move::AdditionalInfo)((uint16_t)revertedMove.move.additionalInfo & (uint16_t)Move::AdditionalInfo::EnPassant) != Move::AdditionalInfo::None)
		{
			// Current player is white, so the move was done by black
			if (playerToMove == PlayerColor::White)
			{
				board.pieces[targetY + 1][targetX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::Pawn | (uint8_t)opponentPiecesColor));
			}
			else
			{
				board.pieces[targetY - 1][targetX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::Pawn | (uint8_t)opponentPiecesColor));
			}

			materialDisparity -= materialMultiplier * PositionEvaluator::piecesMaterial[Piece::Type::Pawn];
		}

		Move::AdditionalInfo castlingInfo;

		if ((castlingInfo = (Move::AdditionalInfo)((uint16_t)revertedMove.move.additionalInfo & Move::castlingChecker)) != Move::AdditionalInfo::None)
		{
			// Moving rook to starting position
			// A bit of hardcoding, but these indices are always the same
			// Updating castling rights is commented out, because it's being recoverd from MoveMemento after refactor
			switch (castlingInfo)
			{
				case Move::AdditionalInfo::WhiteKingsideCastle:
				{
					board.pieces[0][5].SetBitPiece(NO_PIECE);
					board.pieces[0][7].SetBitPiece((uint8_t)Piece::Type::Rook | (uint8_t)movingPieceColor);
					//castlingRights = (CastlingRights)((uint8_t)castlingRights | (uint8_t)CastlingRights::WhiteKingside);
					break;
				}
				case Move::AdditionalInfo::WhiteQueensideCastle:
				{
					board.pieces[0][3].SetBitPiece(NO_PIECE);
					board.pieces[0][0].SetBitPiece((uint8_t)Piece::Type::Rook | (uint8_t)movingPieceColor);
					//castlingRights = (CastlingRights)((uint8_t)castlingRights | (uint8_t)CastlingRights::WhiteQueenside);
					break;
				}
				case Move::AdditionalInfo::BlackKingsideCastle:
				{
					board.pieces[7][5].SetBitPiece(NO_PIECE);
					board.pieces[7][7].SetBitPiece((uint8_t)Piece::Type::Rook | (uint8_t)movingPieceColor);
					//castlingRights = (CastlingRights)((uint8_t)castlingRights | (uint8_t)CastlingRights::BlackKingside);
					break;
				}
				case Move::AdditionalInfo::BlackQueensideCastle:
				{
					board.pieces[7][3].SetBitPiece(NO_PIECE);
					board.pieces[7][0].SetBitPiece((uint8_t)Piece::Type::Rook | (uint8_t)movingPieceColor);
					//castlingRights = (CastlingRights)((uint8_t)castlingRights | (uint8_t)CastlingRights::BlackQueenside);
					break;
				}
			}
		}
	}

	// Restoring previous moving piece position
	board.pieces[sourceY][sourceX].SetBitPiece((uint8_t)movingPieceType | (uint8_t)movingPieceColor);

	// Restoring previous duck position
	if (revertedMove.move.sourceDuckSquare != Square::None)
	{
		board.pieces[duckSourceY][duckSourceX].SetBitPiece((uint8_t)Piece::Type::Duck | (uint8_t)Piece::Color::Both);
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
}

Evaluation Position::CountMaterial()
{
	Evaluation result = 0;

	for (int y = 0; y < Board::HEIGHT; ++y)
	{
		for (int x = 0; x < Board::WIDTH; ++x)
		{
			Piece piece = board.pieces[y][x];

			result += PositionEvaluator::piecesMaterial[board.pieces[y][x].PieceType()] *
				(board.pieces[y][x].PieceColor() == Piece::Color::White ? 1 : -1);
		}
	}

	return result;
}

void Position::UpdateCastling(const Move::AdditionalInfo& moveInfo, const int& targetY, const int& targetX, const uint8_t& playerColor)
{
	Move::AdditionalInfo castlingInfo = (Move::AdditionalInfo)((uint16_t)moveInfo & Move::castlingChecker);
	Square rookSourceSquare;
	Square rookTargetSquare;

	switch (castlingInfo)
	{
		case Move::AdditionalInfo::WhiteKingsideCastle:
		{ 
			rookSourceSquare = Square::H1;
			rookTargetSquare = Square::F1;
			break;
		}
		case Move::AdditionalInfo::WhiteQueensideCastle:
		{ 
			rookSourceSquare = Square::A1;
			rookTargetSquare = Square::D1;
			break;
		}
		case Move::AdditionalInfo::BlackKingsideCastle:
		{ 
			rookSourceSquare = Square::H8; 
			rookTargetSquare = Square::F8; 
			break;
		}
		case Move::AdditionalInfo::BlackQueensideCastle: 
		{ 
			rookSourceSquare = Square::A8;
			rookTargetSquare = Square::D8;
			break;
		}
		default: { return; }
	}

	// Emptying rook square
	int rookX;
	int rookY;

	SquareToBoardIndices(rookSourceSquare, rookY, rookX);
	board.pieces[rookY][rookX].SetBitPiece(NO_PIECE);

	SquareToBoardIndices(rookTargetSquare, rookY, rookX);

	// Moving king
	board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::King | playerColor));

	// Moving rook
	board.pieces[rookY][rookX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::Rook | playerColor));
}