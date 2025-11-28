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
        ApplyPieceSubFrame();
        SetMovePattern();
    }

    // ----------------------------------------------------
    // Piece Movement Patterns
    // ----------------------------------------------------
    void ChessPiece::SetMovePattern()
    {
        switch (GetPieceType())
        {
        case EChessPieceType::Pawn:
            MovePatterns[EChessPieceType::Pawn] = {
                MovePattern({ 0, 1 }, false, EMoveType::Normal),
                MovePattern({ 0, 2 }, false, EMoveType::DoubleAdvance),
                MovePattern({ -1, 1 }, false, EMoveType::Capture),
                MovePattern({ 1, 1 }, false, EMoveType::Capture)
            };
            break;

        case EChessPieceType::Rook:
            MovePatterns[EChessPieceType::Rook] = {
                MovePattern({ 0, 1 }, true),
                MovePattern({ 0, -1 }, true),
                MovePattern({ 1, 0 }, true),
                MovePattern({ -1, 0 }, true)
            };
            break;

        case EChessPieceType::Bishop:
            MovePatterns[EChessPieceType::Bishop] = {
                MovePattern({ 1, 1 }, true),
                MovePattern({ -1, 1 }, true),
                MovePattern({ 1, -1 }, true),
                MovePattern({ -1, -1 }, true)
            };
            break;

        case EChessPieceType::Queen:
            MovePatterns[EChessPieceType::Queen] = {
                MovePattern({ 0, 1 }, true),
                MovePattern({ 0, -1 }, true),
                MovePattern({ 1, 0 }, true),
                MovePattern({ -1, 0 }, true),
                MovePattern({ 1, 1 }, true),
                MovePattern({ -1, 1 }, true),
                MovePattern({ 1, -1 }, true),
                MovePattern({ -1, -1 }, true)
            };
            break;

        case EChessPieceType::Knight:
            MovePatterns[EChessPieceType::Knight] = {
                MovePattern({ 1, 2 }, false),
                MovePattern({ -1, 2 }, false),
                MovePattern({ 1, -2 }, false),
                MovePattern({ -1, -2 }, false),
                MovePattern({ 2, 1 }, false),
                MovePattern({ -2, 1 }, false),
                MovePattern({ 2, -1 }, false),
                MovePattern({ -2, -1 }, false)
            };
            break;

        case EChessPieceType::King:
            MovePatterns[EChessPieceType::King] = {
                MovePattern({ 0, 1 }, false),
                MovePattern({ 0, -1 }, false),
                MovePattern({ 1, 0 }, false),
                MovePattern({ -1, 0 }, false),
                MovePattern({ 1, 1 }, false),
                MovePattern({ -1, 1 }, false),
                MovePattern({ 1, -1 }, false),
                MovePattern({ -1, -1 }, false)
            };
            break;

        default:
            break;
        }
    }

    // ----------------------------------------------------
    // Sub-frame (sprite sheet lookup)
    // ----------------------------------------------------
    void ChessPiece::ApplyPieceSubFrame()
    {
        const int x_index = static_cast<int>(PieceType);
        const int y_index = (Color == EChessColor::White) ? 0 : 1;

        sf::IntRect pieceRect{
            sf::Vector2i{ PIECE_SIZE * x_index, PIECE_SIZE * y_index },
            sf::Vector2i{ PIECE_SIZE, PIECE_SIZE }
        };

        SetTextureRect(pieceRect);
    }

    // ----------------------------------------------------
    // Grid Position
    // ----------------------------------------------------
    void ChessPiece::SetGridPosition(const sf::Vector2i& NewPosition)
    {
        GridPosition = NewPosition;
        UpdateSpritePosition();
    }

    void ChessPiece::UpdateSpritePosition()
    {
        const float centerOffset = PIECE_SIZE / 2.0f;

        const float relativeX = GridPosition.x * PIECE_SIZE + centerOffset;
        const float relativeY = GridPosition.y * PIECE_SIZE + centerOffset;

        const float pixelX = GRID_ABS_OFFSET_X + relativeX;
        const float pixelY = GRID_ABS_OFFSET_Y + relativeY;

        SetActorLocation(sf::Vector2f{ pixelX, pixelY });
    }

    // ----------------------------------------------------
    // Hover and Selection
    // ----------------------------------------------------
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
