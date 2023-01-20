#include "Position.h"

void Position::Udpdate(const Move& move)
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

	// Emptying source sqaure
	board.pieces[sourceY][sourceX].SetBitPiece((BitPiece)Piece::Type::None);

	// Filling target square
	switch (move.additionalInfo)
	{
		case Move::AdditionalInfo::None:
		{
			board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)movingPieceType | (uint8_t)movingPieceColor));
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

			SquareToBoardIndices(Square::H1, rookX, rookY);

			board.pieces[rookX][rookY].SetBitPiece((BitPiece)Piece::Type::None);

			SquareToBoardIndices(Square::F1, rookX, rookY);

			// Moving king
			board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::King | (uint8_t)movingPieceColor));

			// Moving rook
			board.pieces[rookX][rookY].SetBitPiece((BitPiece)((uint8_t)Piece::Type::Pawn | (uint8_t)movingPieceColor));

			// Removing castling rights
			castlingRights = (CastlingRights)((uint8_t)castlingRights ^ (uint8_t)CastlingRights::WhiteKingside);
		}
		case Move::AdditionalInfo::WhiteQueensideCastle:
		{
			// Emptying rook square
			int rookX;
			int rookY;

			SquareToBoardIndices(Square::A1, rookX, rookY);

			board.pieces[rookX][rookY].SetBitPiece((BitPiece)Piece::Type::None);

			SquareToBoardIndices(Square::D1, rookX, rookY);

			// Moving king
			board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::King | (uint8_t)movingPieceColor));

			// Moving rook
			board.pieces[rookX][rookY].SetBitPiece((BitPiece)((uint8_t)Piece::Type::Pawn | (uint8_t)movingPieceColor));

			// Removing castling rights
			castlingRights = (CastlingRights)((uint8_t)castlingRights ^ (uint8_t)CastlingRights::WhiteQueenside);
		}
		case Move::AdditionalInfo::BlackKingsideCastle:
		{
			// Emptying rook square
			int rookX;
			int rookY;

			SquareToBoardIndices(Square::H8, rookX, rookY);

			board.pieces[rookX][rookY].SetBitPiece((BitPiece)Piece::Type::None);

			SquareToBoardIndices(Square::F8, rookX, rookY);

			// Moving king
			board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::King | (uint8_t)movingPieceColor));

			// Moving rook
			board.pieces[rookX][rookY].SetBitPiece((BitPiece)((uint8_t)Piece::Type::Pawn | (uint8_t)movingPieceColor));

			// Removing castling rights
			castlingRights = (CastlingRights)((uint8_t)castlingRights ^ (uint8_t)CastlingRights::BlackKingside);
		}
		case Move::AdditionalInfo::BlackQueensideCastle:
		{
			// Emptying rook square
			int rookX;
			int rookY;

			SquareToBoardIndices(Square::A8, rookX, rookY);

			board.pieces[rookX][rookY].SetBitPiece((BitPiece)Piece::Type::None);

			SquareToBoardIndices(Square::D8, rookX, rookY);

			// Moving king
			board.pieces[targetY][targetX].SetBitPiece((BitPiece)((uint8_t)Piece::Type::King | (uint8_t)movingPieceColor));

			// Moving rook
			board.pieces[rookX][rookY].SetBitPiece((BitPiece)((uint8_t)Piece::Type::Pawn | (uint8_t)movingPieceColor));

			// Removing castling rights
			castlingRights = (CastlingRights)((uint8_t)castlingRights ^ (uint8_t)CastlingRights::BlackQueenside);
		}
	}
}

void Position::SquareToBoardIndices(const Square& square, int& y, int& x)
{
	y = ((uint8_t)square - 1) / Board::WIDTH;
	x = ((uint8_t)square - 1) % Board::WIDTH;
}
