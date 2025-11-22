#include "Board/ChessPieces.h"

namespace we
{
	ChessPiece::ChessPiece(World* OwningWorld, EChessPieceType Type, EChessColor Color, const std::string& TexturePath)
		: Actor{OwningWorld, TexturePath},
		PieceType { Type },
		Color { Color }
	{
		SetTexture(TexturePath);
        ApplyPieceSubFrame();
	}

    void ChessPiece::ApplyPieceSubFrame()
    {
        int x_index = static_cast<int>(PieceType);

        int y_index = (Color == EChessColor::White) ? 0 : 1;

        sf::IntRect PieceRect{ sf::Vector2i{PIECE_SIZE * x_index, PIECE_SIZE * y_index}, sf::Vector2i{PIECE_SIZE, PIECE_SIZE} };

        SetTextureRect(PieceRect);
    }

    void ChessPiece::SetGridPosition(sf::Vector2i& NewPosition)
    {
        GridPosition = NewPosition;
        UpdateSpritePosition();
    }

    void ChessPiece::SetHovered(bool bNewHovered)
    {
        if (bIsHovered != bNewHovered)
        {
            bIsHovered = bNewHovered;
            UpdateVisualState();
        }
    }

    void ChessPiece::SetSelected(bool bNewSelected)
    {
        if (bIsSelected != bNewSelected)
        {
            bIsSelected = bNewSelected;
            UpdateVisualState();
        }
    }

    void ChessPiece::UpdateSpritePosition()
    {
        float CenterOffset = PIECE_SIZE / 2.0f;

        float RelativeX = GridPosition.x * PIECE_SIZE + CenterOffset;
        float RelativeY = GridPosition.y * PIECE_SIZE + CenterOffset;

        float PixelX = GRID_ABS_OFFSET_X + RelativeX;
        float PixelY = GRID_ABS_OFFSET_Y + RelativeY;

        SetActorLocation(sf::Vector2f{ PixelX, PixelY });
    }
    void ChessPiece::UpdateVisualState()
    {
        if (bIsSelected)
        {
            GetSprite().setColor(sf::Color(180, 255, 180));
        }
        else if (bIsHovered)
        {
            GetSprite().setColor(sf::Color(220, 220, 220));
        }
        else
        {
            GetSprite().setColor(sf::Color::White);
        }
    }
}