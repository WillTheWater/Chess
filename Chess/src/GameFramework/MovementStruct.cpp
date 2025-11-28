#include "GameFramework/MovementStruct.h"

namespace we
{
    const Map<EChessPieceType, List<MovePattern>> PieceMovePatterns =
    {
        // Pawn
        { EChessPieceType::Pawn, {
            MovePattern({ 0, -1 }, false),
            MovePattern({ 0, -2 }, false),
            MovePattern({ -1, -1 }, false),
            MovePattern({ 1, -1 }, false)
        }},

        // Rook
        { EChessPieceType::Rook, {
            MovePattern({ 0, 1 }, true),
            MovePattern({ 0, -1 }, true),
            MovePattern({ 1, 0 }, true),
            MovePattern({ -1, 0 }, true)
        }},

        // Bishop
        { EChessPieceType::Bishop, {
            MovePattern({ 1, 1 }, true),
            MovePattern({ -1, 1 }, true),
            MovePattern({ 1, -1 }, true),
            MovePattern({ -1, -1 }, true)
        }},

        // Queen
        { EChessPieceType::Queen, {
            MovePattern({ 0, 1 }, true),
            MovePattern({ 0, -1 }, true),
            MovePattern({ 1, 0 }, true),
            MovePattern({ -1, 0 }, true),
            MovePattern({ 1, 1 }, true),
            MovePattern({ -1, 1 }, true),
            MovePattern({ 1, -1 }, true),
            MovePattern({ -1, -1 }, true)
        }},

        // Knight
        { EChessPieceType::Knight, {
            MovePattern({ 1, 2 }, false),
            MovePattern({ -1, 2 }, false),
            MovePattern({ 1, -2 }, false),
            MovePattern({ -1, -2 }, false),
            MovePattern({ 2, 1 }, false),
            MovePattern({ -2, 1 }, false),
            MovePattern({ 2, -1 }, false),
            MovePattern({ -2, -1 }, false)
        }},

        // King
        { EChessPieceType::King, {
            MovePattern({ 0, 1 }, false),
            MovePattern({ 0, -1 }, false),
            MovePattern({ 1, 0 }, false),
            MovePattern({ -1, 0 }, false),
            MovePattern({ 1, 1 }, false),
            MovePattern({ -1, 1 }, false),
            MovePattern({ 1, -1 }, false),
            MovePattern({ -1, -1 }, false)
        }}
    };
}