#include "Position.h"

extern void SquareToBoardIndices(const Square& square, int& y, int& x);
extern Square BoardIndicesToSquare(int y, int x);

void Position::UndoMove(std::tuple<FullMove, Square, uint8_t> revertedMoveTuple)
{
	FullMove revertedMove = std::get<0>(revertedMoveTuple);
	int sourceY;
	int sourceX;
	int targetY;
	int targetX;
	int duckSourceY;
	int duckSourceX;
	int duckTargetY;
	int duckTargetX;

	SquareToBoardIndices(revertedMove.sourceSquare, sourceY, sourceX);
	SquareToBoardIndices(revertedMove.targetSquare, targetY, targetX);
	SquareToBoardIndices(revertedMove.sourceDuckSquare, duckSourceY, duckSourceX);
	SquareToBoardIndices(revertedMove.targetDuckSquare, duckTargetY, duckTargetX);

	// What piece is moving
	Piece::Type movingPieceType = (Move::AdditionalInfo)((uint16_t)revertedMove.additionalInfo & (uint16_t)Move::promotionChecker) != Move::AdditionalInfo::None ?
		Piece::Type::Pawn :
		board.pieces[targetY][targetX].PieceType();
	Piece::Color movingPieceColor = board.pieces[targetY][targetX].PieceColor();

	Piece::Color opponentPiecesColor = movingPieceColor == Piece::Color::White ? Piece::Color::Black : Piece::Color::White;
	uint16_t capturedPiece;

	// Clearing target duck square
	board.pieces[duckTargetY][duckTargetX].SetBitPiece((BitPiece)Piece::Type::None);

	// Piece was taken
	if ((Move::AdditionalInfo)(capturedPiece = (uint16_t)revertedMove.additionalInfo & Move::captureChecker) != Move::AdditionalInfo::None)
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
	}
	else
	{
		board.pieces[targetY][targetX].SetBitPiece((BitPiece)Piece::Type::None);

		if ((Move::AdditionalInfo)((uint16_t)revertedMove.additionalInfo & (uint16_t)Move::AdditionalInfo::EnPassant) != Move::AdditionalInfo::None)
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
		}

		Move::AdditionalInfo castlingInfo;

		if ((castlingInfo = (Move::AdditionalInfo)((uint16_t)revertedMove.additionalInfo & Move::castlingChecker)) != Move::AdditionalInfo::None)
		{
			// Moving rook to starting position
			// A bit of hardcoding, but these indices are always the same
			switch (castlingInfo)
			{
			case Move::AdditionalInfo::WhiteKingsideCastle:
			{
				board.pieces[0][5].SetBitPiece((BitPiece)Piece::Type::None);
				board.pieces[0][7].SetBitPiece((uint8_t)Piece::Type::Rook | (uint8_t)movingPieceColor);
				castlingRights = (CastlingRights)((uint8_t)castlingRights | (uint8_t)CastlingRights::WhiteKingside);
			}
			case Move::AdditionalInfo::WhiteQueensideCastle:
			{
				board.pieces[0][3].SetBitPiece((BitPiece)Piece::Type::None);
				board.pieces[0][0].SetBitPiece((uint8_t)Piece::Type::Rook | (uint8_t)movingPieceColor);
				castlingRights = (CastlingRights)((uint8_t)castlingRights | (uint8_t)CastlingRights::WhiteQueenside);
			}
			case Move::AdditionalInfo::BlackKingsideCastle:
			{
				board.pieces[7][5].SetBitPiece((BitPiece)Piece::Type::None);
				board.pieces[7][7].SetBitPiece((uint8_t)Piece::Type::Rook | (uint8_t)movingPieceColor);
				castlingRights = (CastlingRights)((uint8_t)castlingRights | (uint8_t)CastlingRights::BlackKingside);
			}
			case Move::AdditionalInfo::BlackQueensideCastle:
			{
				board.pieces[7][3].SetBitPiece((BitPiece)Piece::Type::None);
				board.pieces[7][0].SetBitPiece((uint8_t)Piece::Type::Rook | (uint8_t)movingPieceColor);
				castlingRights = (CastlingRights)((uint8_t)castlingRights | (uint8_t)CastlingRights::BlackQueenside);
			}
			}
		}
	}

	// Restoring previous moving piece position
	board.pieces[sourceY][sourceX].SetBitPiece((uint8_t)movingPieceType | (uint8_t)movingPieceColor);

	// Restoring previous duck position
	board.pieces[duckSourceY][duckSourceX].SetBitPiece((uint8_t)Piece::Type::Duck | (uint8_t)Piece::Color::Both);

	enPassantTarget = std::get<1>(revertedMoveTuple);
	plyClock = std::get<2>(revertedMoveTuple);

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
	board.pieces[sourceY][sourceX].SetBitPiece((BitPiece)Piece::Type::None);

	// Filling target square
	switch (move.additionalInfo)
	{
		case Move::AdditionalInfo::None:
		case Move::AdditionalInfo::CapturedPawn:
		case Move::AdditionalInfo::CapturedBishop:
		case Move::AdditionalInfo::CapturedKnight:
		case Move::AdditionalInfo::CapturedRook:
		case Move::AdditionalInfo::CapturedQueen:
		case Move::AdditionalInfo::CapturedKing:
		{
			board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)movingPieceType | (uint8_t)movingPieceColor));
			break;
		}
		case Move::AdditionalInfo::PawnDoubleForward:
		{
			board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)movingPieceType | (uint8_t)movingPieceColor));
			enPassantTarget = movingPieceColor == Piece::Color::White ? BoardIndicesToSquare(targetY - 1, targetX) : BoardIndicesToSquare(targetY + 1, targetX);
			break;
		}
		case Move::AdditionalInfo::PromotionToQueen:
		{
			board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::Queen | (uint8_t)movingPieceColor));
			break;
		}
		case Move::AdditionalInfo::PromotionToRook:
		{
			board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::Rook | (uint8_t)movingPieceColor));
			break;
		}
		case Move::AdditionalInfo::PromotionToKnight:
		{
			board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::Knight | (uint8_t)movingPieceColor));
			break;
		}
		case Move::AdditionalInfo::PromotionToBishop:
		{
			board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::Bishop | (uint8_t)movingPieceColor));
			break;
		}
		case Move::AdditionalInfo::EnPassant:
		{
			// Emptying taken pawn square
			// Figuring out if it's white or black who took
			// White
			if (targetY > sourceY)
			{
				board.pieces[targetY + 1][targetX].SetBitPiece((BitPiece)Piece::Type::None);

			}
			// Black
			else
			{
				board.pieces[targetY - 1][targetX].SetBitPiece((BitPiece)Piece::Type::None);

			}

			board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::Pawn | (uint8_t)movingPieceColor));
			break;
		}
		case Move::AdditionalInfo::WhiteKingsideCastle:
		{
			// Emptying rook square
			int rookX;
			int rookY;

			SquareToBoardIndices(Square::H1, rookY, rookX);

			board.pieces[rookY][rookX].SetBitPiece((BitPiece)Piece::Type::None);

			SquareToBoardIndices(Square::F1, rookY, rookX);

			// Moving king
			board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::King | (uint8_t)movingPieceColor));

			// Moving rook
			board.pieces[rookY][rookX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::Pawn | (uint8_t)movingPieceColor));

			// Removing castling rights
			castlingRights = (CastlingRights)((uint8_t)castlingRights ^ (uint8_t)CastlingRights::WhiteKingside);
		}
		case Move::AdditionalInfo::WhiteQueensideCastle:
		{
			// Emptying rook square
			int rookX;
			int rookY;

			SquareToBoardIndices(Square::A1, rookY, rookX);

			board.pieces[rookY][rookX].SetBitPiece((BitPiece)Piece::Type::None);

			SquareToBoardIndices(Square::D1, rookY, rookX);

			// Moving king
			board.pieces[targetY][rookX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::King | (uint8_t)movingPieceColor));

			// Moving rook
			board.pieces[rookY][rookX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::Pawn | (uint8_t)movingPieceColor));

			// Removing castling rights
			castlingRights = (CastlingRights)((uint8_t)castlingRights ^ (uint8_t)CastlingRights::WhiteQueenside);
		}
		case Move::AdditionalInfo::BlackKingsideCastle:
		{
			// Emptying rook square
			int rookX;
			int rookY;

			SquareToBoardIndices(Square::H8, rookY, rookX);

			board.pieces[rookY][rookX].SetBitPiece((BitPiece)Piece::Type::None);

			SquareToBoardIndices(Square::F8, rookY, rookX);

			// Moving king
			board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::King | (uint8_t)movingPieceColor));

			// Moving rook
			board.pieces[rookY][rookX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::Pawn | (uint8_t)movingPieceColor));

			// Removing castling rights
			castlingRights = (CastlingRights)((uint8_t)castlingRights ^ (uint8_t)CastlingRights::BlackKingside);
		}
		case Move::AdditionalInfo::BlackQueensideCastle:
		{
			// Emptying rook square
			int rookX;
			int rookY;

			SquareToBoardIndices(Square::A8, rookY, rookX);

			board.pieces[rookY][rookX].SetBitPiece((BitPiece)Piece::Type::None);

			SquareToBoardIndices(Square::D8, rookY, rookX);

			// Moving king
			board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::King | (uint8_t)movingPieceColor));

			// Moving rook
			board.pieces[rookY][rookX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::Pawn | (uint8_t)movingPieceColor));

			// Removing castling rights
			castlingRights = (CastlingRights)((uint8_t)castlingRights ^ (uint8_t)CastlingRights::BlackQueenside);
		}
	}

	// Move the duck
	SquareToBoardIndices(move.sourceDuckSquare, sourceY, sourceX);
	SquareToBoardIndices(move.targetDuckSquare, targetY, targetX);

	board.pieces[sourceY][sourceX].SetBitPiece((BitPiece)Piece::Type::None);
	board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::Duck | (uint8_t)Piece::Color::Both));

	// Updating other information (apart from new en passant target - it's done earlier)
	playerToMove = (playerToMove == PlayerColor::White ? PlayerColor::Black : PlayerColor::White);

	if (movingPieceColor == Piece::Color::Black)
	{
		++fullMovesCount;
	}

	return winnerColor;
}