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

    void ChessPiece::UpdateSpritePosition()
    {
        float CenterOffset = PIECE_SIZE / 2.0f; // 40.0f

        // 1. Calculate the position relative to the grid's (0, 0) top-left corner.
        float RelativeX = GridPosition.x * PIECE_SIZE + CenterOffset;
        float RelativeY = GridPosition.y * PIECE_SIZE + CenterOffset;

        // 2. Add the absolute screen offset of the grid's top-left corner.
        float PixelX = GRID_ABS_OFFSET_X + RelativeX;
        float PixelY = GRID_ABS_OFFSET_Y + RelativeY;

        SetActorLocation(sf::Vector2f{ PixelX, PixelY });
    }
}