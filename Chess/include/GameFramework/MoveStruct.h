#pragma once

#include <SFML/System/Vector2.hpp>
#include "GameFramework/EnumTypes.h"

namespace we
{
    struct Move
    {
        sf::Vector2i From;
        sf::Vector2i To;
        EMoveType MoveType;
        EChessPieceType PromotionChoice;

        Move()
            : From(0, 0)
            , To(0, 0)
            , MoveType(EMoveType::Normal)
            , PromotionChoice(EChessPieceType::Queen)
        {
        }

        Move(const sf::Vector2i& InFrom, const sf::Vector2i& InTo, EMoveType InMoveType, EChessPieceType InPromotionChoice = EChessPieceType::Queen)
            : From(InFrom)
            , To(InTo)
            , MoveType(InMoveType)
            , PromotionChoice(InPromotionChoice)
        {
        }
    };
}