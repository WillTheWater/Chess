#pragma once

#include "Framework/Actor.h"
#include "Board/ChessPieces.h"

namespace we
{
    // ----------------------------------------------------
    // Board Actor
    // ----------------------------------------------------
    class Board : public Actor
    {
    public:
        // ----------------------------------------------------
        // Constructor & Engine Overrides
        // ----------------------------------------------------
        Board(World* OwningWorld, const std::string& TexturePath = "/board.png");

        virtual void BeginPlay() override;
        virtual void Tick(float DeltaTime) override;
        virtual void Render(sf::RenderWindow& Window) override;

    private:
        // ----------------------------------------------------
        // Board Constants
        // ----------------------------------------------------
        static constexpr int GridSize = 8;
        static constexpr int SquareSize = 78;

        static constexpr float GRID_ABS_OFFSET_X = 328.0f;
        static constexpr float GRID_ABS_OFFSET_Y = 49.0f;

        // 1 = King, 2 = Queen, 3 = Bishop, 4 = Knight, 5 = Rook, 6 = Pawn
        static constexpr int InitialBoard[GridSize][GridSize] = {
            {-5, -4, -3, -2, -1, -3, -4, -5},
            {-6, -6, -6, -6, -6, -6, -6, -6},
            { 0,  0,  0,  0,  0,  0,  0,  0},
            { 0,  0,  0,  0,  0,  0,  0,  0},
            { 0,  0,  0,  0,  0,  0,  0,  0},
            { 0,  0,  0,  0,  0,  0,  0,  0},
            { 6,  6,  6,  6,  6,  6,  6,  6},
            { 5,  4,  3,  2,  1,  3,  4,  5}
        };

        // ----------------------------------------------------
        // Board World Conversion
        // ----------------------------------------------------
        sf::Vector2i WorldToGrid(const sf::Vector2f& WorldPos);
        sf::Vector2f GridToWorld(const sf::Vector2i& GridPos);
        std::string GridToAlgebraic(const sf::Vector2i& GridPos);

        // ----------------------------------------------------
        // Piece Helpers
        // ----------------------------------------------------
        void InitializePieces();
        std::string GetPieceName(EChessPieceType Type);

        List<shared<ChessPiece>> Pieces;
        weak<ChessPiece> SelectedPiece;

        // ----------------------------------------------------
        // Drag & Drop Handling
        // ----------------------------------------------------
        void HandleDragStart(const sf::Vector2f& MousePos);
        void HandleDragTick(const sf::Vector2f& MousePos);
        void HandleDragEnd(const sf::Vector2f& MousePos);

        void TryMovePiece(shared<ChessPiece> Piece, const sf::Vector2i& TargetGridPos);
        void HandleMouseHover(const sf::Vector2f& MousePos);
        bool IsWorldPositionInGridBounds(const sf::Vector2f& WorldPos);
        void CleanupDragState(shared<ChessPiece> Piece);

        bool bIsDragging = false;
        weak<ChessPiece> DraggingPiece;
        sf::Vector2i DragStartGridPosition{ -1, -1 };

        // ----------------------------------------------------
        // Debug Visualization
        // ----------------------------------------------------
        void DrawDebugGrid();
        List<sf::RectangleShape> DebugGridSquares;
    };
}
