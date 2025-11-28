#pragma once

namespace we
{
    enum class EChessPieceType
    {
        King = 0,
        Queen,
        Bishop,
        Knight,
        Rook,
        Pawn
    };

    enum class EChessColor
    {
        White,
        Black
    };

    enum class EMoveType
    {
        Normal,
        DoubleAdvance,
        Capture,
        EnPassant,
        Promotion,
        CastleKingSide,
        CastleQueenSide
    };
}