#pragma once

#include <string>
#include <stdexcept>
#include "Board.h"
#include "Position.h"

class FenParser
{
public:

    Position ParseFen(const std::string& fen) const;
    const static int METADATA_COUNT = 5;

private:

    std::string* SplitFenMetadata(std::string& metadata) const;
    CastlingRights ParseCastilngRights(std::string castlingRightsString) const;
};
