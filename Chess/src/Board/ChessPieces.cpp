#include "Board/ChessPieces.h"

namespace we
{
    // ----------------------------------------------------
    // Constructor
    // ----------------------------------------------------
    ChessPiece::ChessPiece(World* OwningWorld, EChessPieceType Type, EChessColor Color, const std::string& TexturePath)
        : Actor{ OwningWorld, TexturePath }
        , PieceType{ Type }
        , Color{ Color }
    {
        SetTexture(TexturePath);
        SetSpriteSheetFrame();
    }

    // ----------------------------------------------------
    // Sprite Sheet Setting
    // ----------------------------------------------------
    void ChessPiece::SetSpriteSheetFrame()
    {
        const int x_index = static_cast<int>(PieceType);
        const int y_index = (Color == EChessColor::White) ? 0 : 1;

        sf::IntRect pieceRect{sf::Vector2i{ PIECE_SIZE * x_index, PIECE_SIZE * y_index }, sf::Vector2i{ PIECE_SIZE, PIECE_SIZE }};

        SetTextureRect(pieceRect);
    }

    // ----------------------------------------------------
    // Hover and Selection
    // ----------------------------------------------------
    void ChessPiece::SetHovered(bool NewHovered)
    {
        if (bIsHovered == NewHovered) return;
        bIsHovered = NewHovered;
        UpdateVisualState();
    }

    void ChessPiece::SetSelected(bool bNewSelected)
    {
        if (bIsSelected == bNewSelected) { return; }
        bIsSelected = bNewSelected;
        UpdateVisualState();
    }

    // ----------------------------------------------------
    // Visual State
    // ----------------------------------------------------
    void ChessPiece::UpdateVisualState()
    {
        if (bIsSelected)
        {
            GetSprite().setColor(sf::Color(255, 255, 255, 80));
        }
        else if (bIsHovered)
        {
            GetSprite().setColor(sf::Color(255, 255, 255, 150));
        }
        else
        {
            GetSprite().setColor(sf::Color::White);
        }
    }
}
