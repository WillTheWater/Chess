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

        bool GetHasMoved() const { return bHasMoved; }
        void SetHasMoved() { bHasMoved = true; }
        bool GetWasPawnMovedTwo() const { return bWasPawnMovedTwo; }
        void SetWasPawnMovedTwo(bool Move) { bWasPawnMovedTwo = Move; }
        bool GetIsInCheck() const { return bIsInCheck; }
        void SetIsInCheck(bool Check) { bIsInCheck = Check; }
        bool GetIsInCheckmate() const { return bIsInCheckmate; }
        void SetIsInCheckmate(bool Checkmate) { bIsInCheckmate = Checkmate; }
        sf::Vector2i GetGridPosition() const { return GridPosition; }
        void SetGridPosition(const sf::Vector2i& NewPosition) { GridPosition = NewPosition; }

        // ------------------------------------------------
        // Hover / Selection
        // ------------------------------------------------
        void SetHovered(bool NewHovered);
        bool IsHovered() const { return bIsHovered; }

        void SetSelected(bool bNewSelected);
        bool IsSelected() const { return bIsSelected; }

    private:
        // ------------------------------------------------
        // Piece Data
        // ------------------------------------------------
        void SetSpriteSheetFrame();
        void UpdateVisualState();

        static constexpr int PIECE_SIZE = 78;
        EChessPieceType PieceType;
        EChessColor Color;
        bool bIsHovered = false;
        bool bIsSelected = false;
        bool bHasMoved = false;
        bool bWasPawnMovedTwo = false;
        bool bIsInCheck = false;
        bool bIsInCheckmate = false;
        sf::Vector2i GridPosition = { 0,0 };
    };
}
