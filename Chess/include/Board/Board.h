#pragma once
#include "Framework/Actor.h"
#include "Board/ChessPieces.h"
#include "Framework/Delegate.h"

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

    class Board : public Actor
    {
    public:
        Board(World* OwningWorld, const std::string& TexturePath = "/board.png");

        virtual void BeginPlay() override;
        virtual void Tick(float DeltaTime) override;
        virtual void Render(class Renderer& GameRenderer) override;

        Delegate<> OnCheckmate;
        Delegate<> OnStalemate;
        Delegate<> OnDraw;

    private:
        // ----------------------------------------------------
        // Board Constraints
        // ----------------------------------------------------
        static constexpr int GridSize = 8;
        static constexpr int SquareSize = 120;
        static constexpr float GRID_ABS_OFFSET_X = 480.f;
        static constexpr float GRID_ABS_OFFSET_Y = 60.f;
        static constexpr float BoardPixelWidth = 1920.f;
        static constexpr float BoardPixelHeight = 1080.f;

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
        bool bIsGameOver = false;

        // ----------------------------------------------------
        // Board World Conversion
        // ----------------------------------------------------
        sf::Vector2f GetBoardTopLeft() const;
        sf::Vector2i WorldToGrid(const sf::Vector2f& WorldPos);
        sf::Vector2f GridToWorld(const sf::Vector2i& GridPos);
        sf::Vector2f GridToCenterSquare(const sf::Vector2i& GridPos);
        std::string GridToAlgebraic(const sf::Vector2i& GridPos);

        // ----------------------------------------------------
        // Input Handling
        // ----------------------------------------------------
        void HandleInput();
        void HandleMouseHover();
        bool IsInBounds(const sf::Vector2i& GridPos) const;
        void HandleDragStart(const sf::Vector2f& MousePos);
        void HandleDragTick(const sf::Vector2f& MousePos);
        void HandleDragEnd(const sf::Vector2f& MousePos);
        void UpdateBoard(const MoveResult& Result);
        sf::Vector2i MousePixelPosition;
        sf::Vector2f MouseWorldPosition;
        sf::Vector2i HoveredGridPos{ -1, -1 };
        weak<ChessPiece> HoveredPiece;
        bool bIsDragging = false;
        bool bLeftMouseButtonPressedLastFrame = false;
        sf::Vector2i DragStartGridPosition{ -1, -1 };

        // ----------------------------------------------------
        // Pieces Helpers
        // ----------------------------------------------------
        std::string GetPieceName(EChessPieceType Type);
        shared<ChessPiece> GetPieceAt(const sf::Vector2i& GridPos) const;
        shared<ChessPiece> GetPieceAt(shared<ChessPiece> InBoard[GridSize][GridSize], const sf::Vector2i& GridPos) const;
        sf::Vector2i GetKing(shared<ChessPiece> Board[GridSize][GridSize], EChessColor Color);
        bool IsPlayersPiece(const ChessPiece* Piece) const;

        // ----------------------------------------------------
        // Game Logic
        // ----------------------------------------------------

        bool IsMoveValid(shared<ChessPiece> Board[GridSize][GridSize], shared<ChessPiece> Piece, sf::Vector2i From, sf::Vector2i To) const;
        bool IsMoveLegal(shared<ChessPiece> Board[GridSize][GridSize], shared<ChessPiece> Piece, sf::Vector2i From, sf::Vector2i To);
        optional<MoveResult> HandleMove(shared<ChessPiece> piece, sf::Vector2i from, sf::Vector2i to);
        void SwitchTurn();

        void Capture(shared<ChessPiece> TargetPiece);
        void EnPassant(shared<ChessPiece>& Piece, sf::Vector2i& To, sf::Vector2i& From, shared<ChessPiece> SimBoard[GridSize][GridSize], MoveResult& Result);
        void Move(shared<ChessPiece> PieceToMove, sf::Vector2i From, sf::Vector2i To);
        void Castle(shared<ChessPiece> Rook, sf::Vector2i From, sf::Vector2i To);
        void Castle(shared<ChessPiece>& Piece, sf::Vector2i& To, sf::Vector2i& From, MoveResult& Result);
        void PromotePawn(const sf::Vector2i& pos, EChessPieceType PromotionType);
        void PromotePawn(shared<ChessPiece>& Piece, sf::Vector2i& To, MoveResult& Result);
        bool IsSquareAttacked(shared<ChessPiece> Board[GridSize][GridSize], const sf::Vector2i& Pos, EChessColor DefenderColor) const;
        void CheckmateOrStalemate(shared<ChessPiece> SimBoard[GridSize][GridSize], EChessColor OpponentColor, MoveResult& Result);
        void Draw(shared<ChessPiece> SimBoard[GridSize][GridSize], MoveResult& Result);

        bool IsRookMoveValid(shared<ChessPiece> Board[GridSize][GridSize], sf::Vector2i From, sf::Vector2i To) const;
        bool IsBishopMoveValid(shared<ChessPiece> Board[GridSize][GridSize], sf::Vector2i From, sf::Vector2i To) const;
        bool IsQueenMoveValid(shared<ChessPiece> Board[GridSize][GridSize], sf::Vector2i From, sf::Vector2i To) const;
        bool IsKnightMoveValid(sf::Vector2i From, sf::Vector2i To) const;
        bool IsKingMoveValid(shared<ChessPiece> Board[GridSize][GridSize], shared<ChessPiece> Piece, sf::Vector2i From, sf::Vector2i To) const;
        bool IsPawnMoveValid(shared<ChessPiece> Board[GridSize][GridSize], shared<ChessPiece> Piece, sf::Vector2i From, sf::Vector2i To) const;

        // ----------------------------------------------------
        // Window Functionality
        // ----------------------------------------------------
        bool bIsDraggingWindow = false;
        sf::Vector2i WindowDragOffset;
        sf::RenderWindow* m_WindowRef = nullptr;
    };
}