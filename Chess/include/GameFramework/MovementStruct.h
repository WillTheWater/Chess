#pragma once

#include <SFML/System/Vector2.hpp>
#include "GameFramework/EnumTypes.h"
#include "Framework/Core.h"

namespace we
{
    struct MovePattern
    {
        sf::Vector2i MovementVector;
        bool bRepeatable;

        MovePattern(const sf::Vector2i& InMovement, bool InRepeatable = false)
            : MovementVector(InMovement), 
              bRepeatable(InRepeatable)
        {
        }

        
    };

    // ----------------------------------------------------
    // Piece Movement Patterns
    // ----------------------------------------------------
    extern const Map<EChessPieceType, List<MovePattern>> PieceMovePatterns;
}