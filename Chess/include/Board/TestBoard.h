#pragma once
#include "Framework/Actor.h"
#include "Board/ChessPieces.h"

namespace we
{
    struct MoveResult
    {
        bool bValid = false;
        sf::Vector2i From;
        sf::Vector2i To;
        shared<ChessPiece> CapturedPiece;
        bool bPawnPromoted = false;
        EChessPieceType PromotionType;
        bool bCastling = false;
        sf::Vector2i RookFrom, RookTo;

        bool bEnPassant = false;
        bool bIsCheck = false;
        bool bIsCheckmate = false;
        bool bIsStalemate = false;
        bool bIsDraw = false;
    };

    enum class EPlayerTurn
    {
        White,
        Black
    };

	class TestBoard : public Actor
	{
    public:
        TestBoard(World* OwningWorld, const std::string& TexturePath = "/board.png");

        virtual void BeginPlay() override;
        virtual void Render(class Renderer& GameRenderer) override;

    private: 
        // ----------------------------------------------------
        // Board Constraints
        // ----------------------------------------------------
        static constexpr int GridSize = 8;
        static constexpr int SquareSize = 120;
        static constexpr float GRID_ABS_OFFSET_X = 480.f;
        static constexpr float GRID_ABS_OFFSET_Y = 60.f;
        shared<ChessPiece> BoardGrid[GridSize][GridSize] = {};

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
       // Initialization
       // ----------------------------------------------------
        void InitializeBoard();
        void ClearBoard();
        EChessColor GetPieceColor(int value);
        EChessPieceType GetPieceType(int value);
        EPlayerTurn CurrentTurn = EPlayerTurn::White;
        void SpawnPiece(EChessPieceType type, EChessColor color, const sf::Vector2i& pos);
        weak<ChessPiece> SelectedPiece;
        List<shared<ChessPiece>> Pieces;

        // ----------------------------------------------------
        // Board World Conversion
        // ----------------------------------------------------
        sf::Vector2i WorldToGrid(const sf::Vector2f& WorldPos);
        sf::Vector2f GridToWorld(const sf::Vector2i& GridPos);
        sf::Vector2f GridToCenterSquare(const sf::Vector2i& GridPos);
        std::string GridToAlgebraic(const sf::Vector2i& GridPos);
	};
}