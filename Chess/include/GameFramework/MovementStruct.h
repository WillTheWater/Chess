#pragma once

#include <SFML/System/Vector2.hpp>
#include "GameFramework/EnumTypes.h"

namespace we
{
    struct MovePattern
    {
        sf::Vector2i Offset;
        bool bRepeatable;
        EMoveType DefaultMoveType;

        MovePattern(const sf::Vector2i& InOffset, bool InRepeatable = false, EMoveType InMoveType = EMoveType::Normal)
            : Offset(InOffset), 
            bRepeatable(InRepeatable), 
            DefaultMoveType(InMoveType)
        {
        }
    };
}