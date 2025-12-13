#pragma once

#include "Framework/Actor.h"
#include "Board/ChessPieces.h"

namespace we
{
    // ----------------------------------------------------
    // Move Simulation Struct
    // ----------------------------------------------------
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
        // Board Constraints
        // ----------------------------------------------------
        static constexpr int GridSize = 8;
        static constexpr int SquareSize = 78;
        static constexpr float GRID_ABS_OFFSET_X = 328.0f;
        static constexpr float GRID_ABS_OFFSET_Y = 49.0f;
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
        void SpawnPiece(EChessPieceType type, EChessColor color, const sf::Vector2i& pos);

        // ----------------------------------------------------
        // Board World Conversion
        // ----------------------------------------------------
        sf::Vector2i WorldToGrid(const sf::Vector2f& WorldPos);
        sf::Vector2f GridToWorld(const sf::Vector2i& GridPos);
        sf::Vector2f GridToCenterSquare(const sf::Vector2i& GridPos);
        std::string GridToAlgebraic(const sf::Vector2i& GridPos);

        // ----------------------------------------------------
        // Input Handling
        // ----------------------------------------------------
        sf::Vector2i MousePixelPosition;
        sf::Vector2f MouseWorldPosition;
        void UpdateMousePosition();
        sf::Vector2i HoveredGridPos{ -1, -1 };
        weak<ChessPiece> HoveredPiece;
        void HandleMouseHover();
        void HandleInput();

        // ----------------------------------------------------
        // Drag & Drop Handling
        // ----------------------------------------------------
        void HandleDragStart(const sf::Vector2f& MousePos);
        void HandleDragTick(const sf::Vector2f& MousePos);
        void HandleDragEnd(const sf::Vector2f& MousePos);
        void Board::UpdateBoard(const MoveResult& Result);

        bool IsInBounds(const sf::Vector2i& GridPos) const;
        bool bIsDragging = false;
        bool bLeftMouseButtonPressedLastFrame = false;
        weak<ChessPiece> SelectedPiece;
        sf::Vector2i DragStartGridPosition{ -1, -1 };

        // ----------------------------------------------------
        // Pieces Helpers
        // ----------------------------------------------------
        List<shared<ChessPiece>> Pieces;
        std::string GetPieceName(EChessPieceType Type);
        shared<ChessPiece> GetPieceAt(const sf::Vector2i& GridPos) const;
        bool IsPlayersPiece(const ChessPiece* Piece) const;

        // ----------------------------------------------------
        // Game Logic
        // ----------------------------------------------------
        EPlayerTurn CurrentTurn = EPlayerTurn::White;

        bool Board::IsMoveValid(shared<ChessPiece> Piece, sf::Vector2i From, sf::Vector2i To, shared<ChessPiece> Board[GridSize][GridSize]) const;
        bool Board::IsMoveLegal(shared<ChessPiece> Piece, sf::Vector2i From, sf::Vector2i To, shared<ChessPiece> Board[GridSize][GridSize]);
        optional<MoveResult> HandleMove(shared<ChessPiece> piece, sf::Vector2i from, sf::Vector2i to);

        void Move(shared<ChessPiece> PieceToMove, sf::Vector2i From, sf::Vector2i To);
        void Castle(shared<ChessPiece> Rook, sf::Vector2i From, sf::Vector2i To);
        void PromotePawn(const sf::Vector2i& pos, EChessPieceType PromotionType);
        void EnPassant(we::shared<we::ChessPiece>& piece, sf::Vector2i& to, sf::Vector2i& from);
        void Capture(const sf::Vector2i& GridPos);
        bool IsSquareAttacked(const sf::Vector2i& Pos, EChessColor DefenderColor) const;
        bool IsSquareAttacked(const sf::Vector2i& Pos, EChessColor DefenderColor, shared<ChessPiece> Board[GridSize][GridSize]) const;
        void Check();
        void Checkmate();
        void Stalemate();
        void SwitchTurn();
               
        bool IsRookMoveValid(shared<ChessPiece> Board[GridSize][GridSize], sf::Vector2i From, sf::Vector2i To) const;
        bool IsBishopMoveValid(shared<ChessPiece> Board[GridSize][GridSize], sf::Vector2i From, sf::Vector2i To) const;
        bool IsQueenMoveValid(shared<ChessPiece> Board[GridSize][GridSize], sf::Vector2i From, sf::Vector2i To) const;
        bool IsKnightMoveValid(sf::Vector2i From, sf::Vector2i To) const;
        bool IsKingMoveValid(shared<ChessPiece> Board[GridSize][GridSize], shared<ChessPiece> Piece, sf::Vector2i From, sf::Vector2i To) const;
        bool IsPawnMoveValid(shared<ChessPiece> Board[GridSize][GridSize], shared<ChessPiece> Piece, sf::Vector2i From, sf::Vector2i To) const;

        static sf::Vector2i FindKing(
            shared<ChessPiece> Board[GridSize][GridSize],
            EChessColor Color)
        {
            for (int x = 0; x < GridSize; ++x)
                for (int y = 0; y < GridSize; ++y)
                    if (Board[x][y] &&
                        Board[x][y]->GetPieceType() == EChessPieceType::King &&
                        Board[x][y]->GetColor() == Color)
                        return { x, y };

            return { -1, -1 };
        }
    };
}