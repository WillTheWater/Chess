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

        SetActorLocation(sf::Vector2f{float(GetWindowSize().x) / 2.0f, float(GetWindowSize().y) / 2.0f});

        InitializeBoard();
        //LOG("White's Turn!")
    }

    // -------------------------------------------------------------------------
    // Tick
    // -------------------------------------------------------------------------
    void Board::Tick(float DeltaTime)
    {
        sf::RenderWindow* Window = GetWorld()->GetRenderWindow();

        if (!Window)
        {
            Actor::Tick(DeltaTime);
            return;
        }

        // Mouse Position
        sf::Vector2i PixelPosition = sf::Mouse::getPosition(*Window);
        sf::Vector2f MousePosition = Window->mapPixelToCoords(PixelPosition);

        bool bLeftMouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        // Drag State
        if (bLeftMouseDown)
        {
            if (!bLeftMouseButtonPressedLastFrame)
            {
                HandleDragStart(MousePosition);
            }
            else if (bIsDragging)
            {
                HandleDragTick(MousePosition);
            }
        }
        else if (bLeftMouseButtonPressedLastFrame)
        {
            HandleDragEnd(MousePosition);
        }

        bLeftMouseButtonPressedLastFrame = bLeftMouseDown;

        // Hover Logic
        if (!bIsDragging)
        {
            HandleMouseHover(MousePosition);
        }

        Actor::Tick(DeltaTime);
    }

    // -------------------------------------------------------------------------
    // Render
    // -------------------------------------------------------------------------
    void Board::Render(sf::RenderWindow& Window)
    {
        Actor::Render(Window);

        /*
        for (const auto& Square : DebugGridSquares)
            Window.draw(Square);
        */

        for (const auto& Piece : Pieces)
        {
            if (Piece) { Piece->Render(Window); }
        }
    }

    // -------------------------------------------------------------------------
    // Valid Move Logic
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
                ChessPiece* P = GetPieceAt({ x, y }).get();
                if (!P) continue;
                if (P->GetPieceType() == EChessPieceType::King && P->GetColor() == KingColor)
                {
                    King = P;
                    break;
                }
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
                ChessPiece* P = GetPieceAt({ x, y }).get();
                if (!P) continue;
                if (P->GetColor() == KingColor) continue;

                // Instead of using IsMoveValid blindly, use a function that only checks if the piece **can attack that square** ignoring check/pin
                if (CanPieceAttackSquare(P, KingPos))
                {
                    return true;
                }
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
                if (GetPieceAt({ x, y })) return false;
                x += stepX;
                y += stepY;
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
                if (GetPieceAt({ x, y })) return false;
                x += stepX;
                y += stepY;
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
                    if (GetPieceAt({ x, y })) return false;
                    x += stepX;
                    y += stepY;
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
                    if (GetPieceAt({ x, y })) return false;
                    x += stepX;
                    y += stepY;
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
            if (GetPieceAt(pos)) { return false; }

            pos.x += dx;
            pos.y += dy;
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
            if (GetPieceAt(pos))
            {
                return false;
            }
            pos.x += dx;
            pos.y += dy;
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
            if (GetPieceAt(pos))
            {
                return false;
            }
            pos.x += dx;
            pos.y += dy;
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
            shared<ChessPiece> Rook = GetPieceAt(RookPos);
            if (!Rook || Rook->GetPieceType() != EChessPieceType::Rook || Rook->GetColor() != Color || Rook->GetHasMoved())
            {
                return false;
            }

            // Path between king and rook must be empty
            int start = std::min(From.x, rookX) + 1;
            int end = std::max(From.x, rookX) - 1;

            for (int x = start; x <= end; x++)
            {
                if (GetPieceAt({ x, From.y }) != nullptr)
                    return false;
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

        shared<ChessPiece> targetPiece = GetPieceAt(To);

        // Forward move
        if (deltaX == 0)
        {
            // Cannot move forward if blocked
            if (targetPiece) { return false; }

            // Move 1 square
            if (deltaY == direction) { return true; }

            // Move 2 squares from start position
            if (!Piece->GetHasMoved() && deltaY == 2 * direction)
            {
                sf::Vector2i middleSquare = { From.x, From.y + direction };
                if (GetPieceAt(middleSquare)) { return false; }
                return true;
            }

            return false;
        }

        // Diagonal capture
        if (std::abs(deltaX) == 1 && deltaY == direction)
        {
            // Capturing an enemy piece
            if (targetPiece && targetPiece->GetColor() != Piece->GetColor()) { return true; }

            // En-passant
            if (!targetPiece)
            {
                sf::Vector2i enemyPos = { To.x, From.y };
                shared<ChessPiece> enemyPawn = GetPieceAt(enemyPos);

                if (enemyPawn &&
                    enemyPawn->GetPieceType() == EChessPieceType::Pawn &&
                    enemyPawn->GetColor() != Piece->GetColor() &&
                    enemyPawn->GetWasPawnMovedTwo())
                {
                    return true;
                }
            }

            return false;
        }

        return false;
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
                    EChessColor color = GetColorFromInitialValue(value);
                    EChessPieceType type = GetTypeFromInitialValue(value);
                    SpawnInitialPiece(type, color, { x, y });
                }
            }
        }
    }

    void Board::ClearBoard()
    {
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

    EChessColor Board::GetColorFromInitialValue(int value)
    {
        return (value > 0) ? EChessColor::White : EChessColor::Black;
    }

    EChessPieceType Board::GetTypeFromInitialValue(int value)
    {
        int id = std::abs(value) - 1;
        return static_cast<EChessPieceType>(id);
    }

    void Board::SpawnInitialPiece(EChessPieceType type, EChessColor color, const sf::Vector2i& pos)
    {
        weak<ChessPiece> pieceWeak = GetWorld()->SpawnActor<ChessPiece>(type, color);
        shared<ChessPiece> piece = pieceWeak.lock();

        if (!piece) { return; }

        piece->SetGridPosition(pos);
        SetPieceAt(pos, piece);
        Pieces.push_back(piece);
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

    // -------------------------------------------------------------------------
    // Debug Grid Drawing
    // -------------------------------------------------------------------------
    void Board::DrawDebugGrid()
    {
        DebugGridSquares.clear();

        sf::Vector2f size{ float(SquareSize), float(SquareSize) };

        for (int y = 0; y < GridSize; ++y)
        {
            for (int x = 0; x < GridSize; ++x)
            {
                sf::RectangleShape Square;
                Square.setSize(size);

                Square.setPosition({GRID_ABS_OFFSET_X + x * SquareSize, GRID_ABS_OFFSET_Y + y * SquareSize});

                Square.setFillColor(sf::Color(0, 255, 0, 100));
                Square.setOutlineColor(sf::Color::Blue);
                Square.setOutlineThickness(-2.0f);

                DebugGridSquares.push_back(Square);
            }
        }
    }

    // -------------------------------------------------------------------------
    // Hover Detection
    // -------------------------------------------------------------------------
    void Board::HandleMouseHover(const sf::Vector2f& MousePos)
    {
        // Convert mouse to grid position
        sf::Vector2i GridPos = WorldToGrid(MousePos);

        // Bounds check
        if (GridPos.x < 0 || GridPos.x >= GridSize || GridPos.y < 0 || GridPos.y >= GridSize)
        {
            if (HoveredPiece)
            {
                HoveredPiece->SetHovered(false);
                HoveredPiece = nullptr;
            }
            return;
        }

        // Get the piece directly from the grid
        ChessPiece* Piece = GetPieceAt(GridPos).get(); // raw pointer

        // Skip if no piece or not current player's
        if (!Piece || !IsPlayersPiece(Piece))
        {
            if (HoveredPiece)
            {
                HoveredPiece->SetHovered(false);
                HoveredPiece = nullptr;
            }
            return;
        }

        // Unhover previous piece if different
        if (HoveredPiece && HoveredPiece != Piece)
        {
            HoveredPiece->SetHovered(false);
        }

        // Hover new piece
        if (!Piece->IsSelected() && !Piece->IsHovered())
        {
            Piece->SetHovered(true);
        }

        // Cache hovered piece
        HoveredPiece = Piece;
    }

    bool Board::IsWorldPositionInGridBounds(const sf::Vector2f& WorldPos)
    {
        float GridWidth = GridSize * SquareSize;
        float GridHeight = GridSize * SquareSize;

        float MinX = GRID_ABS_OFFSET_X;
        float MinY = GRID_ABS_OFFSET_Y;

        float MaxX = MinX + GridWidth;
        float MaxY = MinY + GridHeight;

        return (WorldPos.x >= MinX && WorldPos.x < MaxX && WorldPos.y >= MinY && WorldPos.y < MaxY);
    }

    void Board::CleanupDragState(ChessPiece* Piece)
    {
        Piece->SetSelected(false);
        SelectedPiece.reset();
        DraggingPiece = nullptr;
        bIsDragging = false;
    }

    // -------------------------------------------------------------------------
    // Movement Logic
    // -------------------------------------------------------------------------
    void Board::TryMovePiece(shared<ChessPiece> Piece, const sf::Vector2i& TargetGridPos)
    {
        if (!Piece)
            return;

        sf::Vector2i From = Piece->GetGridPosition();
        sf::Vector2i To = TargetGridPos;
        EChessColor MovingColor = Piece->GetColor();
        bool bMoveIsSuccessful = true;

        shared<ChessPiece> CapturedPiece = GetPieceAt(To);
        shared<ChessPiece> TempCaptured = nullptr;

        // 1. Validate normal move (without check consideration)
        if (!IsMoveValid(Piece, From, To))
        {
            bMoveIsSuccessful = false;
        }

        // 2. En Passant capture
        if (!CapturedPiece && Piece->GetPieceType() == EChessPieceType::Pawn)
        {
            int direction = (MovingColor == EChessColor::White) ? -1 : 1;
            if (abs(To.x - From.x) == 1 && To.y - From.y == direction)
            {
                sf::Vector2i enemyPos = { To.x, From.y };
                shared<ChessPiece> EP = GetPieceAt(enemyPos);
                if (EP && EP->GetPieceType() == EChessPieceType::Pawn &&
                    EP->GetColor() != MovingColor && EP->GetWasPawnMovedTwo())
                {
                    CapturedPiece = EP;
                }
            }
        }

        // 3. Simulate the move to see if king would be in check
        if (bMoveIsSuccessful)
        {
            // Temporarily remove captured piece
            if (CapturedPiece)
            {
                TempCaptured = CapturedPiece;
                SetPieceAt(CapturedPiece->GetGridPosition(), nullptr);
            }

            // Temporarily move piece
            SetPieceAt(From, nullptr);
            SetPieceAt(To, Piece);
            Piece->SetGridPosition(To);

            // Check king safety
            if (IsKingInCheck(MovingColor))
            {
                bMoveIsSuccessful = false;
            }

            // Restore board
            SetPieceAt(From, Piece);
            SetPieceAt(To, TempCaptured);
            Piece->SetGridPosition(From);
        }

        // 4. Apply move if legal
        if (bMoveIsSuccessful)
        {
            // Capture
            if (CapturedPiece)
            {
                sf::Vector2i capPos = CapturedPiece->GetGridPosition();
                SetPieceAt(capPos, nullptr);

                Pieces.erase(std::remove(Pieces.begin(), Pieces.end(), CapturedPiece), Pieces.end());
                CapturedPiece->Destroy();
            }

            // Update grid and piece
            SetPieceAt(From, nullptr);
            SetPieceAt(To, Piece);
            Piece->SetGridPosition(To);
            Piece->SetHasMoved();

            // Pawn two-step flag
            if (Piece->GetPieceType() == EChessPieceType::Pawn)
            {
                Piece->SetWasPawnMovedTwo(abs(To.y - From.y) == 2);
                for (auto& P : Pieces)
                {
                    if (P && P != Piece && P->GetPieceType() == EChessPieceType::Pawn)
                        P->SetWasPawnMovedTwo(false);
                }
            }

            // Castling
            if (Piece->GetPieceType() == EChessPieceType::King)
            {
                int dx = To.x - From.x;

                // King-side
                if (dx == 2)
                {
                    sf::Vector2i rookFrom = { 7, From.y };
                    sf::Vector2i rookTo = { To.x - 1, From.y };
                    shared<ChessPiece> Rook = GetPieceAt(rookFrom);
                    if (Rook && !Rook->GetHasMoved() &&
                        !IsSquareAttacked({ From.x + 1, From.y }, MovingColor) &&
                        !IsSquareAttacked(To, MovingColor))
                    {
                        SetPieceAt(rookFrom, nullptr);
                        SetPieceAt(rookTo, Rook);
                        Rook->SetGridPosition(rookTo);
                        Rook->SetHasMoved();
                    }
                }

                // Queen-side
                else if (dx == -2)
                {
                    sf::Vector2i rookFrom = { 0, From.y };
                    sf::Vector2i rookTo = { To.x + 1, From.y };
                    shared<ChessPiece> Rook = GetPieceAt(rookFrom);
                    if (Rook && !Rook->GetHasMoved() &&
                        !IsSquareAttacked({ From.x - 1, From.y }, MovingColor) &&
                        !IsSquareAttacked(To, MovingColor))
                    {
                        SetPieceAt(rookFrom, nullptr);
                        SetPieceAt(rookTo, Rook);
                        Rook->SetGridPosition(rookTo);
                        Rook->SetHasMoved();
                    }
                }
            }

            SwitchTurn();
        }

        CleanupDragState(Piece.get());
    }

    // -------------------------------------------------------------------------
    // Drag Start
    // -------------------------------------------------------------------------
    void Board::HandleDragStart(const sf::Vector2f& MousePos)
    {
        sf::Vector2i gridPos = WorldToGrid(MousePos);
        shared<ChessPiece> PieceSP = GetPieceAt(gridPos);
        ChessPiece* Piece = PieceSP.get();

        if (!Piece || !IsPlayersPiece(Piece)) return;

        bIsDragging = true;
        DraggingPiece = Piece;
        DragStartGridPosition = Piece->GetGridPosition();

        Piece->SetSelected(true);
        Piece->SetHovered(false);
    }

    // -------------------------------------------------------------------------
    // Drag Tick
    // -------------------------------------------------------------------------
    void Board::HandleDragTick(const sf::Vector2f& MousePos)
    {
        if (!DraggingPiece) return;

        // Optional: only update if mouse moved
        static sf::Vector2f LastMousePos = MousePos;
        if (MousePos != LastMousePos)
        {
            DraggingPiece->SetActorLocation(MousePos);
            LastMousePos = MousePos;
        }
    }

    // -------------------------------------------------------------------------
    // Drag End
    // -------------------------------------------------------------------------
    void Board::HandleDragEnd(const sf::Vector2f& MousePos)
    {
        if (!DraggingPiece)
            return;

        ChessPiece* piece = DraggingPiece;

        // Validate move
        if (!IsPlayersPiece(piece) || !IsWorldPositionInGridBounds(MousePos))
        {
            piece->SetGridPosition(DragStartGridPosition);
            CleanupDragState(piece);
        }
        else
        {
            sf::Vector2i dropGrid = WorldToGrid(MousePos);
            shared<ChessPiece> PieceSP = BoardGrid[piece->GetGridPosition().x][piece->GetGridPosition().y];
            TryMovePiece(PieceSP, dropGrid);
        }

        bIsDragging = false;
        DraggingPiece = nullptr;
    }

    bool Board::IsPlayersPiece(const ChessPiece* Piece) const
    {
        return (CurrentTurn == EPlayerTurn::White && Piece->GetColor() == EChessColor::White) || (CurrentTurn == EPlayerTurn::Black && Piece->GetColor() == EChessColor::Black);
    }

    void Board::SwitchTurn()
    {
        CurrentTurn = (CurrentTurn == EPlayerTurn::White)? EPlayerTurn::Black : EPlayerTurn::White;
        const char* TurnName = (CurrentTurn == EPlayerTurn::White) ? "White" : "Black";
        //LOG("It's %s's turn.", TurnName);
    }
}