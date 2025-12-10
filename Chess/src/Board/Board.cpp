#include "Board/Board.h"
#include "Framework/World.h"
#include <sstream>

namespace we
{
    // -------------------------------------------------------------------------
    // Constructor
    // -------------------------------------------------------------------------
    Board::Board(World* OwningWorld, const std::string& TexturePath)
        : Actor{ OwningWorld, TexturePath }
        , Pieces{}
    {
        SetTexture(TexturePath);
    }

    // -------------------------------------------------------------------------
    // BeginPlay
    // -------------------------------------------------------------------------
    void Board::BeginPlay()
    {
        Actor::BeginPlay();

        SetActorLocation(sf::Vector2f{ float(GetWindowSize().x) / 2.0f, float(GetWindowSize().y) / 2.0f });

        InitializeBoard();
    }

    // -------------------------------------------------------------------------
    // Tick
    // -------------------------------------------------------------------------
    void Board::Tick(float DeltaTime)
    {
        HandleInput();
        Actor::Tick(DeltaTime);
    }

    // -------------------------------------------------------------------------
    // Render
    // -------------------------------------------------------------------------
    void Board::Render(sf::RenderWindow& Window)
    {
        Actor::Render(Window);

        for (const auto& Piece : Pieces)
        {
            if (Piece) { Piece->Render(Window); }
        }
    }

    // -------------------------------------------------------------------------
    // Grid World Conversion
    // -------------------------------------------------------------------------
    sf::Vector2i Board::WorldToGrid(const sf::Vector2f& WorldPos)
    {
        float RelativeX = WorldPos.x - GRID_ABS_OFFSET_X;
        float RelativeY = WorldPos.y - GRID_ABS_OFFSET_Y;

        int GridX = static_cast<int>(std::floor(RelativeX / SquareSize));
        int GridY = static_cast<int>(std::floor(RelativeY / SquareSize));

        GridX = std::clamp(GridX, 0, GridSize - 1);
        GridY = std::clamp(GridY, 0, GridSize - 1);

        return { GridX, GridY };
    }

    sf::Vector2f Board::GridToWorld(const sf::Vector2i& GridPos)
    {
        float PixelX = GRID_ABS_OFFSET_X + GridPos.x * SquareSize;
        float PixelY = GRID_ABS_OFFSET_Y + GridPos.y * SquareSize;

        return { PixelX, PixelY };
    }

    sf::Vector2f Board::GridToCenterSquare(const sf::Vector2i& GridPos)
    {
        return {
        GRID_ABS_OFFSET_X + GridPos.x * SquareSize + SquareSize * 0.5f,
        GRID_ABS_OFFSET_Y + GridPos.y * SquareSize + SquareSize * 0.5f
        };
    }

    std::string Board::GridToAlgebraic(const sf::Vector2i& GridPos)
    {
        if (GridPos.x < 0 || GridPos.x >= 8 || GridPos.y < 0 || GridPos.y >= 8) { return "Invalid"; }

        char File = 'a' + GridPos.x;
        char Rank = '8' - GridPos.y;

        std::stringstream ss;
        ss << File << Rank;
        return ss.str();
    }

    // -------------------------------------------------------------------------
    // Initialization
    // -------------------------------------------------------------------------
    void Board::InitializeBoard()
    {
        ClearBoard();

        for (int y = 0; y < GridSize; ++y)
        {
            for (int x = 0; x < GridSize; ++x)
            {
                int value = InitialBoard[y][x];
                if (value != 0)
                {
                    EChessColor color = GetPieceColor(value);
                    EChessPieceType type = GetPieceType(value);
                    SpawnPiece(type, color, { x, y });
                }
            }
        }
    }

    void Board::ClearBoard()
    {
        for (auto& Piece : Pieces)
        {
            if (Piece)
            {
                Piece->Destroy();
            }
        }

        for (int y = 0; y < GridSize; ++y)
        {
            for (int x = 0; x < GridSize; ++x)
            {
                BoardGrid[x][y] = nullptr;
            }
        }
        Pieces.clear();
        SelectedPiece.reset();
    }

    EChessColor Board::GetPieceColor(int value)
    {
        return (value > 0) ? EChessColor::White : EChessColor::Black;
    }

    EChessPieceType Board::GetPieceType(int value)
    {
        int id = std::abs(value) - 1;
        return static_cast<EChessPieceType>(id);
    }

    void Board::SpawnPiece(EChessPieceType type, EChessColor color, const sf::Vector2i& pos)
    {
        weak<ChessPiece> pieceWeak = GetWorld()->SpawnActor<ChessPiece>(type, color);
        shared<ChessPiece> piece = pieceWeak.lock();

        if (!piece) { return; }

        piece->SetGridPosition(pos);
        piece->SetActorLocation(GridToCenterSquare(pos));
        BoardGrid[pos.x][pos.y] = piece;
        Pieces.push_back(piece);
    }

    // -------------------------------------------------------------------------
    // Input Handling
    // -------------------------------------------------------------------------
    void Board::UpdateMousePosition()
    {
        sf::RenderWindow* Window = GetWorld()->GetRenderWindow();
        if (!Window) { return; }

        MousePixelPosition = sf::Mouse::getPosition(*Window);
        MouseWorldPosition = Window->mapPixelToCoords(MousePixelPosition);
    }

    void Board::HandleMouseHover()
    {
        if (bIsDragging) return;

        sf::Vector2i gridPos = WorldToGrid(MouseWorldPosition);

        if (gridPos != HoveredGridPos)
        {
            HoveredGridPos = gridPos;

            shared<ChessPiece> piece = GetPieceAt(gridPos);

            for (auto& p : Pieces)
            {
                if (p && p != piece) p->SetHovered(false);
            }

            if (piece) piece->SetHovered(true);
        }
    }

    void Board::HandleInput()
    {
        UpdateMousePosition();

        bool bLeftMouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        if (bLeftMouseDown)
        {
            if (!bLeftMouseButtonPressedLastFrame)
                HandleDragStart(MouseWorldPosition);
            else if (bIsDragging)
                HandleDragTick(MouseWorldPosition);
        }
        else if (bLeftMouseButtonPressedLastFrame)
        {
            HandleDragEnd(MouseWorldPosition);
        }

        bLeftMouseButtonPressedLastFrame = bLeftMouseDown;

        if (!bIsDragging)
            HandleMouseHover();
    }

    // -------------------------------------------------------------------------
    // Piece Drag & Drop
    // -------------------------------------------------------------------------
    void Board::HandleDragStart(const sf::Vector2f& MousePos)
    {
        sf::Vector2i gridPos = WorldToGrid(MousePos);
        shared<ChessPiece> piece = GetPieceAt(gridPos);

        if (!piece) return;

        SelectedPiece = piece;
        bIsDragging = true;

        piece->SetSelected(true);
        piece->SetHovered(false);
    }

    void Board::HandleDragTick(const sf::Vector2f& MousePos)
    {
        if (auto piece = SelectedPiece.lock())
        {
            piece->SetActorLocation(MousePos);
        }
    }

    void Board::HandleDragEnd(const sf::Vector2f& MouseWorldPos)
    {
        if (auto piece = SelectedPiece.lock())
        {
            sf::Vector2i releasePos = WorldToGrid(MouseWorldPos);

            if (IsOutOfBounds(MouseWorldPos) || GetPieceAt(releasePos))
            {
                piece->SetActorLocation(GridToCenterSquare(piece->GetGridPosition()));
            }
            else
            {
                BoardGrid[piece->GetGridPosition().x][piece->GetGridPosition().y] = nullptr;
                BoardGrid[releasePos.x][releasePos.y] = piece;

                piece->SetGridPosition(releasePos);
                piece->SetActorLocation(GridToCenterSquare(releasePos));
            }
            piece->SetSelected(false);
        }
        SelectedPiece.reset();
        bIsDragging = false;
    }

    bool Board::IsOutOfBounds(const sf::Vector2f& WorldPos)
    {
        float GridWidth = GridSize * SquareSize;
        float GridHeight = GridSize * SquareSize;

        float MinX = GRID_ABS_OFFSET_X;
        float MinY = GRID_ABS_OFFSET_Y;

        float MaxX = MinX + GridWidth;
        float MaxY = MinY + GridHeight;

        return !(WorldPos.x >= MinX && WorldPos.x < MaxX && WorldPos.y >= MinY && WorldPos.y < MaxY);
    }

    // -------------------------------------------------------------------------
    // Piece Helpers
    // -------------------------------------------------------------------------
    std::string Board::GetPieceName(EChessPieceType Type)
    {
        switch (Type)
        {
        case EChessPieceType::King:   return "King";
        case EChessPieceType::Queen:  return "Queen";
        case EChessPieceType::Bishop: return "Bishop";
        case EChessPieceType::Knight: return "Knight";
        case EChessPieceType::Rook:   return "Rook";
        case EChessPieceType::Pawn:   return "Pawn";
        default:                      return "Unknown";
        }
    }

    shared<ChessPiece> Board::GetPieceAt(const sf::Vector2i& GridPos) const
    {
        if (GridPos.x < 0 || GridPos.x >= GridSize || GridPos.y < 0 || GridPos.y >= GridSize)
            return nullptr;

        return BoardGrid[GridPos.x][GridPos.y];
    }
        
    // -------------------------------------------------------------------------
    // Game Logic
    // -------------------------------------------------------------------------
    bool Board::IsMoveValid(shared<ChessPiece> Piece, sf::Vector2i From, sf::Vector2i To) const
    {
        switch (Piece->GetPieceType())
        {
        case EChessPieceType::Rook:   return IsRookMoveValid(Piece, From, To);
        case EChessPieceType::Bishop: return IsBishopMoveValid(Piece, From, To);
        case EChessPieceType::Queen:  return IsQueenMoveValid(Piece, From, To);
        case EChessPieceType::King:   return IsKingMoveValid(Piece, From, To);
        case EChessPieceType::Knight: return IsKnightMoveValid(Piece, From, To);
        case EChessPieceType::Pawn:   return IsPawnMoveValid(Piece, From, To);
        default: return true;
        }
    }

    bool Board::IsSquareAttacked(const sf::Vector2i& Pos, EChessColor DefenderColor) const
    {
        EChessColor AttackerColor = (DefenderColor == EChessColor::White) ? EChessColor::Black : EChessColor::White;

        for (const auto& Piece : Pieces)
        {
            if (!Piece || Piece->IsPendingDestroy())
                continue;

            // Only consider enemy pieces
            if (Piece->GetColor() != AttackerColor)
                continue;

            sf::Vector2i From = Piece->GetGridPosition();

            // Skip the piece if it can't reach the target square by its normal move rules
            if (!IsMoveValid(Piece, From, Pos))
                continue;

            // Pawns are special: only diagonal captures count as attacks
            if (Piece->GetPieceType() == EChessPieceType::Pawn)
            {
                int dir = (AttackerColor == EChessColor::White) ? -1 : 1;
                if (abs(Pos.x - From.x) == 1 && Pos.y - From.y == dir)
                    return true;
            }
            else
            {
                return true; // any other piece can attack
            }
        }

        return false;
    }

    bool Board::IsKingInCheck(EChessColor KingColor) const
    {
        // 1. Find the king
        ChessPiece* King = nullptr;
        for (int y = 0; y < 8; ++y)
        {
            for (int x = 0; x < 8; ++x)
            {
                // TODO
            }
            if (King) break;
        }

        if (!King) return false; // should never happen

        sf::Vector2i KingPos = King->GetGridPosition();

        // 2. Check all squares for opponent attacks
        for (int y = 0; y < 8; ++y)
        {
            for (int x = 0; x < 8; ++x)
            {
                // TODO
            }
        }

        return false;
    }

    bool Board::CanPieceAttackSquare(ChessPiece* Piece, const sf::Vector2i& TargetPos) const
    {
        if (!Piece) return false;

        sf::Vector2i From = Piece->GetGridPosition();
        sf::Vector2i To = TargetPos;

        switch (Piece->GetPieceType())
        {
        case EChessPieceType::Pawn:
        {
            int dir = (Piece->GetColor() == EChessColor::White) ? -1 : 1;
            // Pawns attack diagonally only
            if (abs(To.x - From.x) == 1 && (To.y - From.y) == dir)
                return true;
            return false;
        }

        case EChessPieceType::Knight:
        {
            int dx = abs(To.x - From.x);
            int dy = abs(To.y - From.y);
            return (dx == 2 && dy == 1) || (dx == 1 && dy == 2);
        }

        case EChessPieceType::Bishop:
        {
            int dx = To.x - From.x;
            int dy = To.y - From.y;
            if (abs(dx) != abs(dy)) return false;

            int stepX = (dx > 0) ? 1 : -1;
            int stepY = (dy > 0) ? 1 : -1;
            int x = From.x + stepX;
            int y = From.y + stepY;
            while (x != To.x && y != To.y)
            {
                // TODO
            }
            return true;
        }

        case EChessPieceType::Rook:
        {
            if (From.x != To.x && From.y != To.y) return false;
            int stepX = (To.x - From.x) != 0 ? ((To.x - From.x) > 0 ? 1 : -1) : 0;
            int stepY = (To.y - From.y) != 0 ? ((To.y - From.y) > 0 ? 1 : -1) : 0;

            int x = From.x + stepX;
            int y = From.y + stepY;
            while (x != To.x || y != To.y)
            {
               // TODO
            }
            return true;
        }

        case EChessPieceType::Queen:
        {
            // Combine rook + bishop logic
            int dx = To.x - From.x;
            int dy = To.y - From.y;

            if (dx == 0 || dy == 0)
            {
                // Rook-like move
                int stepX = (dx != 0) ? (dx > 0 ? 1 : -1) : 0;
                int stepY = (dy != 0) ? (dy > 0 ? 1 : -1) : 0;
                int x = From.x + stepX;
                int y = From.y + stepY;
                while (x != To.x || y != To.y)
                {
                    // TODO
                }
                return true;
            }
            else if (abs(dx) == abs(dy))
            {
                // Bishop-like move
                int stepX = (dx > 0) ? 1 : -1;
                int stepY = (dy > 0) ? 1 : -1;
                int x = From.x + stepX;
                int y = From.y + stepY;
                while (x != To.x && y != To.y)
                {
                    // TODO
                }
                return true;
            }
            return false;
        }

        case EChessPieceType::King:
        {
            int dx = abs(To.x - From.x);
            int dy = abs(To.y - From.y);
            return (dx <= 1 && dy <= 1);
        }
        }

        return false;
    }

    bool Board::IsRookMoveValid(shared<ChessPiece> Piece, sf::Vector2i From, sf::Vector2i To) const
    {
        bool sameRow = (From.y == To.y);
        bool sameCol = (From.x == To.x);

        if (!sameRow && !sameCol) { return false; }

        int dx = (To.x > From.x) ? 1 : (To.x < From.x ? -1 : 0);
        int dy = (To.y > From.y) ? 1 : (To.y < From.y ? -1 : 0);

        sf::Vector2i pos = From;
        pos.x += dx;
        pos.y += dy;

        while (pos != To)
        {
            // TODO
        }

        return true;
    }

    bool Board::IsBishopMoveValid(shared<ChessPiece> Piece, sf::Vector2i From, sf::Vector2i To) const
    {
        if (From == To) { return false; }

        int deltaX = To.x - From.x;
        int deltaY = To.y - From.y;

        if (std::abs(deltaX) != std::abs(deltaY)) { return false; }

        int dx = (deltaX > 0) ? 1 : -1;
        int dy = (deltaY > 0) ? 1 : -1;

        sf::Vector2i pos = From;
        pos.x += dx;
        pos.y += dy;

        while (pos != To)
        {
            // TODO
        }

        return true;
    }

    bool Board::IsQueenMoveValid(shared<ChessPiece> Piece, sf::Vector2i From, sf::Vector2i To) const
    {
        if (From == To) { return false; }

        int deltaX = To.x - From.x;
        int deltaY = To.y - From.y;

        bool isDiagonal = (std::abs(deltaX) == std::abs(deltaY));
        bool isStraight = (deltaX == 0 || deltaY == 0);

        if (!isDiagonal && !isStraight) { return false; }

        int dx = (deltaX == 0) ? 0 : (deltaX > 0 ? 1 : -1);
        int dy = (deltaY == 0) ? 0 : (deltaY > 0 ? 1 : -1);

        sf::Vector2i pos = From;
        pos.x += dx;
        pos.y += dy;

        while (pos != To)
        {
            // TODO
        }

        return true;
    }

    bool Board::IsKingMoveValid(shared<ChessPiece> Piece, sf::Vector2i From, sf::Vector2i To) const
    {
        if (!Piece || (From == To) || (Piece->GetPieceType() != EChessPieceType::King)) return false;

        int signedDx = To.x - From.x;
        int deltaX = std::abs(signedDx);
        int deltaY = std::abs(To.y - From.y);

        // ---- Standard king move ----
        if (deltaX <= 1 && deltaY <= 1) { return true; }

        // ---- Castling attempt? ----
        if (deltaY == 0 && deltaX == 2)
        {
            // King has moved
            if (Piece->GetHasMoved()) { return false; }

            EChessColor Color = Piece->GetColor();

            // Kingside or Queenside
            bool bKingside = (signedDx > 0);

            int rookX = bKingside ? 7 : 0;
            sf::Vector2i RookPos(rookX, From.y);

            // Rook must exist and not have moved
            // TODO

            // Path between king and rook must be empty
            int start = std::min(From.x, rookX) + 1;
            int end = std::max(From.x, rookX) - 1;

            for (int x = start; x <= end; x++)
            {
                // TODO
            }

            return true;
        }

        return false;
    }

    bool Board::IsKnightMoveValid(shared<ChessPiece> Piece, sf::Vector2i From, sf::Vector2i To) const
    {
        if (From == To) { return false; }

        int deltaX = std::abs(To.x - From.x);
        int deltaY = std::abs(To.y - From.y);

        return ((deltaX == 1 && deltaY == 2) || (deltaX == 2 && deltaY == 1));
    }

    bool Board::IsPawnMoveValid(shared<ChessPiece> Piece, sf::Vector2i From, sf::Vector2i To) const
    {
        if (!Piece || From == To) { return false; }

        int direction = (Piece->GetColor() == EChessColor::White) ? -1 : 1;
        int startRow = (Piece->GetColor() == EChessColor::White) ? 6 : 1;

        int deltaX = To.x - From.x;
        int deltaY = To.y - From.y;

        // TODO

        return false;
    }
        
    void Board::TryMovePiece(shared<ChessPiece> Piece, const sf::Vector2i& TargetGridPos)
    {
        // TODO
    }

    bool Board::IsPlayersPiece(const ChessPiece* Piece) const
    {
        return (CurrentTurn == EPlayerTurn::White && Piece->GetColor() == EChessColor::White) || (CurrentTurn == EPlayerTurn::Black && Piece->GetColor() == EChessColor::Black);
    }

    void Board::SwitchTurn()
    {
        CurrentTurn = (CurrentTurn == EPlayerTurn::White) ? EPlayerTurn::Black : EPlayerTurn::White;
        const char* TurnName = (CurrentTurn == EPlayerTurn::White) ? "White" : "Black";
    }
}