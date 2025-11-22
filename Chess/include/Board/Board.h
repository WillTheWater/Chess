#pragma once

#include "Framework/Actor.h"
#include "Board/ChessPieces.h"

namespace we
{
	class Board : public Actor
	{
	public:
		Board(World* OwningWolrd, const std::string& TexturePath = "/board.png");

		virtual void BeginPlay() override;
        virtual void Tick(float DeltaTime) override;
        virtual void Render(sf::RenderWindow& Window) override;

	private:
        static constexpr int GridSize = 8;
        static constexpr int SquareSize = 78;
        static constexpr float GRID_ABS_OFFSET_X = 328.0f;
        static constexpr float GRID_ABS_OFFSET_Y = 49.0f;

        // Initial Board layout. Positive values are White, Negative are Black.
     // reserving 0 for an empty square: 1=King, 2=Queen, 3=Bishop, 4=Knight, 5=Rook, 6=Pawn
        static constexpr int InitialBoard[GridSize][GridSize] = {
            {-5, -4, -3, -2, -1, -3, -4, -5}, // Rank 8 (Index 0): -R -N -B -Q -K -B -N -R (Black)
            {-6, -6, -6, -6, -6, -6, -6, -6}, // Rank 7 (Index 1): -P -P -P -P -P -P -P -P (Black)
            { 0,  0,  0,  0,  0,  0,  0,  0}, // Rank 6 (Index 2)
            { 0,  0,  0,  0,  0,  0,  0,  0}, // Rank 5 (Index 3)
            { 0,  0,  0,  0,  0,  0,  0,  0}, // Rank 4 (Index 4)
            { 0,  0,  0,  0,  0,  0,  0,  0}, // Rank 3 (Index 5)
            { 6,  6,  6,  6,  6,  6,  6,  6}, // Rank 2 (Index 6): +P +P +P +P +P +P +P +P (White)
            { 5,  4,  3,  2,  1,  3,  4,  5}  // Rank 1 (Index 7): +R +N +B +Q +K +B +N +R (White)
        };

        List<shared<ChessPiece>> Pieces;
        weak<ChessPiece> SelectedPiece;

        void InitializePieces();
        void DrawDebugGrid();
        List<sf::RectangleShape> DebugGridSquares;
        void HandleMouseHover(const sf::Vector2f& MousePos);
        void HandleMouseClick(const sf::Vector2f& MousePos);
	};
}