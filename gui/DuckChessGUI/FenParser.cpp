#include "FenParser.h"

extern Square SquareIdToSquare(std::string squareId);

Position FenParser::ParseFen(const std::string& fen) const
{
    int squaresFilled = 0;
    int squaresInCurrentRowFilled = 0;
    int x = 0;
    int y = 0;
    unsigned i;
    Position position;

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
            x += digit;
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

        else if (isalpha(fen[i]))
        {
            try
            {
                SetBit(position.board.PIECES_SYMBOLS_BITBOARDS_MAP.at(fen[i]), Board::WIDTH - y - 1, x);
                SetBit(position.board.bitBoards[Board::Type::All][islower(fen[i]) ? Board::Color::Black : Board::Color::White], Board::WIDTH - y - 1, x);
                ++x;
                ++squaresFilled;
                ++squaresInCurrentRowFilled;
                //position.board.pieces[Board::WIDTH - y - 1][x++].SetBitPiece(Piece::PIECES_SYMBOLS_MAP.at(fen[i]));
            }
            catch (const std::out_of_range e)
            {
                throw std::invalid_argument("Provided FEN had an unrecognized symbol \"" + fen[i]);
            }
        }
        else if (fen[i] == position.board.PIECES_SYMBOLS_BITPIECES_MAP.at((BitPiece)((uint8_t)PieceType::Duck | (uint8_t)PieceColor::Both)))
        {
            SetBit(position.board.duck, Board::WIDTH - y - 1, x);
            ++x;
            ++squaresFilled;
            ++squaresInCurrentRowFilled;
        }

        if (squaresFilled == 64)
        {
            break;
        }
    }

    std::string metadata = fen.substr(i + 2);
    std::string* tokens = SplitFenMetadata(metadata);

    // Player to move
    position.playerToMove = tokens[0] == "w" ? PlayerColor::White : PlayerColor::Black;

    // Castling rights
    position.castlingRights = ParseCastilngRights(tokens[1]);

    // En Passant target
    position.enPassantTarget = SquareIdToSquare(tokens[2]);

    // Halfmove clock
    position.plyClock = stoi(tokens[3]);

    // Fullmoves
    position.fullMovesCount = stoi(tokens[4]);

    delete[] tokens;

    position.board.UpdateEmpties();

    return position;
}

std::string* FenParser::SplitFenMetadata(std::string& metadata) const
{
    std::string* tokens = new std::string[METADATA_COUNT];
    size_t position = 0;
    int dataIndex = 0;

    while ((position = metadata.find(" ")) != std::string::npos)
    {
        tokens[dataIndex++] = metadata.substr(0, position);
        metadata.erase(0, position + 1);
    }
    tokens[dataIndex] = metadata;

    return tokens;
}

CastlingRights FenParser::ParseCastilngRights(std::string castlingRightsString) const
{
    CastlingRights resultCastlingRights = CastlingRights::None;

    if (castlingRightsString == "-")
    {
        return resultCastlingRights;
    }

    for (int c = 0; c < castlingRightsString.length(); ++c)
    {
        switch (castlingRightsString[c])
        {
        case 'k': { resultCastlingRights = (CastlingRights)((uint8_t)resultCastlingRights | (uint8_t)CastlingRights::BlackKingside); break; }
        case 'K': { resultCastlingRights = (CastlingRights)((uint8_t)resultCastlingRights | (uint8_t)CastlingRights::WhiteKingside); break; }
        case 'q': { resultCastlingRights = (CastlingRights)((uint8_t)resultCastlingRights | (uint8_t)CastlingRights::BlackQueenside); break; }
        case 'Q': { resultCastlingRights = (CastlingRights)((uint8_t)resultCastlingRights | (uint8_t)CastlingRights::WhiteQueenside); break; }
        default: { throw std::invalid_argument("Provided FEN had invalid castling rights"); }
        }
    }

    return resultCastlingRights;
}
