#pragma once

#include "Framework/Actor.h"

namespace we
{
    // ----------------------------------------------------
    // Enums
    // ----------------------------------------------------
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

    // ----------------------------------------------------
    // Chess Piece Actor
    // ----------------------------------------------------
    class ChessPiece : public Actor
    {
    public:
        // ------------------------------------------------
        // Constructor & Engine Overrides
        // ------------------------------------------------
        ChessPiece(World* OwningWorld, EChessPieceType Type, EChessColor Color, const std::string& TexturePath = "/pieces.png");

        virtual bool ShouldTextureBeSmooth() const override { return true; }

        // ------------------------------------------------
        // Accessors
        // ------------------------------------------------
        EChessPieceType GetPieceType() const { return PieceType; }
        EChessColor GetColor() const { return Color; }

        sf::Vector2i GetGridPosition() const { return GridPosition; }
        void SetGridPosition(const sf::Vector2i& NewPosition);
        bool GetHasMoved() const { return bHasMoved; }
        void SetHasMoved();

        // ------------------------------------------------
        // Hover / Selection
        // ------------------------------------------------
        void SetHovered(bool bNewHovered);
        bool IsHovered() const { return bIsHovered; }

        void SetSelected(bool bNewSelected);
        bool IsSelected() const { return bIsSelected; }

    private:
        // ------------------------------------------------
        // Constants
        // ------------------------------------------------
        static constexpr int PIECE_SIZE = 78;
        static constexpr float GRID_ABS_OFFSET_X = 328.0f;
        static constexpr float GRID_ABS_OFFSET_Y = 49.0f;

        // ------------------------------------------------
        // Piece State
        // ------------------------------------------------
        EChessPieceType PieceType;
        EChessColor Color;

        bool bIsHovered = false;
        bool bIsSelected = false;
        bool bHasMoved = false;

        sf::Vector2i GridPosition{ 0, 0 };
        

        // ------------------------------------------------
        // Internals
        // ------------------------------------------------
        void ApplyPieceSubFrame();
        void UpdateSpritePosition();
        void UpdateVisualState();
    };
}
