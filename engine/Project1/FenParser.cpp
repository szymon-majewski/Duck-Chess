#include "FenParser.h"

void FenParser::ParseFen(const std::string& fen, Game& game) const
{
	int squaresFilled = 0;
	int squaresInCurrentRowFilled = 0;
	int x = 0;
	int y = 0;
	unsigned i;

	for (i = 0; i < fen.length(); ++i)
	{
		if (isdigit(fen[i]))
		{
			int digit = fen[i] - '0';

			if (digit < 1 || digit > 8 || squaresInCurrentRowFilled + digit > Board::WIDTH)
			{
				throw std::invalid_argument("Provided FEN included an invalid digit.");
			}

			squaresFilled += digit;
			squaresInCurrentRowFilled += digit;

			while (digit--)
			{
				game.board.pieces[y][x++].SetPiece(Piece::Type::None);
			}
		}
		else if (fen[i] == '/')
		{
			if (squaresInCurrentRowFilled == 8)
			{
				++y;
				x = 0;
				squaresInCurrentRowFilled = 0;
			}
			else
			{
				throw std::invalid_argument("Provided FEN had a row, that wasn't fully filled.");
			}
		}
		// Low abstracition
		else if (isalpha(fen[i]) || fen[i] == '@')
		{
			try
			{
				game.board.pieces[y][x++].SetPiece(Piece::PIECES_SYMBOLS_MAP.at(fen[i]));
			}
			catch (const std::out_of_range e)
			{
				throw std::invalid_argument("Provided FEN had an unrecognized symbol \"" + fen[i]);
			}

			++squaresFilled;
			++squaresInCurrentRowFilled;
		}

		if (squaresFilled == 64)
		{
			break;
		}
	}

	// TODO: Read metadata

	std::string metadata = fen.substr(i + 2);
	size_t position = 0;
	std::string tokens[METADATA_COUNT];
	int dataIndex = 0;

	while ((position = metadata.find(" ")) != std::string::npos)
	{
		tokens[dataIndex++] = metadata.substr(0, position);
		metadata.erase(0, position + 1);
	}
	tokens[dataIndex] = metadata;

	// Who is to move
	game.playerToMove = tokens[0] == "w" ? White : Black;

	// Castling rights
	game.castlingRights = None;

	for (int c = 0; c < tokens[1].length(); ++c)
	{
		switch (tokens[1][c])
		{
		case 'k': { game.castlingRights = (CastlingRights)(game.castlingRights | CastlingRights::BlackKingside); break; }
		case 'K': { game.castlingRights = (CastlingRights)(game.castlingRights | CastlingRights::WhiteKingside); break; }
		case 'q': { game.castlingRights = (CastlingRights)(game.castlingRights | CastlingRights::BlackQueenside); break; }
		case 'Q': { game.castlingRights = (CastlingRights)(game.castlingRights | CastlingRights::WhiteQueenside); break; }
		default: { throw std::invalid_argument("Provided FEN had invalid castling rights"); }
		}
	}

	// En Passant target
	toupper(tokens[2][0]);
	game.enPassantTarget = (Square)(tokens[2][0] - 'A' + tokens[2][1] - '0');

	// HalfmoveClock
	game.plyClock = stoi(tokens[3]);

	// Fullmoves
	game.fullMovesCount = stoi(tokens[4]);
}