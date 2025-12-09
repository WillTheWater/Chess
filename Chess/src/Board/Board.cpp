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

        InitializePieces();
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
            for (const auto& OtherPiece : Pieces)
            {
                if (!OtherPiece || OtherPiece->IsPendingDestroy()) { continue; }

                if (OtherPiece->GetGridPosition() == pos) { return false; }
            }

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
            for (const auto& OtherPiece : Pieces)
            {
                if (!OtherPiece || OtherPiece->IsPendingDestroy()) { continue; }

                if (OtherPiece->GetGridPosition() == pos) { return false; }
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
            for (const auto& OtherPiece : Pieces)
            {
                if (!OtherPiece || OtherPiece->IsPendingDestroy()) { continue; }

                if (OtherPiece->GetGridPosition() == pos) { return false; }
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

        // Check if target square contains an enemy piece
        shared<ChessPiece> targetPiece = nullptr;
        for (auto& Other : Pieces)
        {
            if (Other && !Other->IsPendingDestroy() && Other->GetGridPosition() == To)
            {
                targetPiece = Other;
                break;
            }
        }

        // Forward move
        if (deltaX == 0)
        {
            // Cannot move forward
            if (targetPiece) { return false; }

            // Move 1 square
            if (deltaY == direction) { return true; }

            // Move 2 squares
            if (!Piece->GetHasMoved() && deltaY == 2 * direction)
            {
                // Check squares are empty
                sf::Vector2i middleSquare = { From.x, From.y + direction };

                for (auto& Other : Pieces)
                {
                    if (!Other || Other->IsPendingDestroy()) { continue; }

                    if (Other->GetGridPosition() == middleSquare) { return false; }
                }
                return true;
            }
            return false;
        }

        // Diagonal capture
        if (std::abs(deltaX) == 1 && deltaY == direction)
        {
            // capturing an enemy piece
            if (targetPiece && targetPiece->GetColor() != Piece->GetColor()) { return true; }

            // EN-PASSANT
            if (!targetPiece)
            {
                sf::Vector2i enemyPos = { To.x, From.y };

                shared<ChessPiece> enemyPawn = nullptr;
                for (auto& Other : Pieces)
                {
                    if (!Other || Other->IsPendingDestroy()) { continue; }

                    if (Other->GetGridPosition() == enemyPos &&
                        Other->GetPieceType() == EChessPieceType::Pawn &&
                        Other->GetColor() != Piece->GetColor())
                    {
                        enemyPawn = Other;
                        break;
                    }
                }

                if (enemyPawn)
                {
                    // Enemy pawn must have moved two squares on the previous turn
                    if (enemyPawn->GetWasPawnMovedTwo())
                    {
                        return true;
                    }
                }
            }
            return false;
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

   /* shared<ChessPiece> Board::GetPieceAt(sf::Vector2i Position) const
    {
        for (const auto& FoundPiece : Pieces)
        {
            if (!FoundPiece || FoundPiece->IsPendingDestroy()) { continue; }

            if (FoundPiece->GetGridPosition() == Position) { return FoundPiece; }
        }

        return nullptr;
    }*/

    void Board::InitializePieces()
    {
        for (int y = 0; y < GridSize; ++y)
        {
            for (int x = 0; x < GridSize; ++x)
            {
                int value = InitialBoard[y][x];
                if (value == 0) { continue; }

                EChessColor color = (value > 0)? EChessColor::White : EChessColor::Black;

                EChessPieceType type = static_cast<EChessPieceType>(std::abs(value) - 1);

                weak<ChessPiece> newPieceWeak = GetWorld()->SpawnActor<ChessPiece>(type, color);

                if (auto newPiece = newPieceWeak.lock())
                {
                    newPiece->SetGridPosition({ x, y });
                    Pieces.push_back(newPiece);
                }
            }
        }

        //LOG("Initialized %zu chess pieces.", Pieces.size());
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
        shared<ChessPiece> HoveredPiece = nullptr;
        shared<ChessPiece> LastHoveredPiece = nullptr;

        for (auto it = Pieces.rbegin(); it != Pieces.rend(); ++it)
        {
            shared<ChessPiece> Piece = *it;
            if (!Piece || Piece->IsPendingDestroy()) { continue; }

            // ---- SKIP PIECES NOT BELONGING TO CURRENT PLAYER ----
            if (!IsPlayersPiece(Piece.get())) { continue; }

            if (Piece->IsPointInBounds(MousePos))
            {
                HoveredPiece = Piece;
            }

            if (Piece->IsHovered())
            {
                LastHoveredPiece = Piece;
            }
        }

        if (LastHoveredPiece && LastHoveredPiece != HoveredPiece)
        {
            LastHoveredPiece->SetHovered(false);
        }

        if (HoveredPiece && !HoveredPiece->IsSelected() && !HoveredPiece->IsHovered())
        {
            HoveredPiece->SetHovered(true);
        }
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

    void Board::CleanupDragState(shared<ChessPiece> Piece)
    {
        Piece->SetSelected(false);
        SelectedPiece.reset();
        DraggingPiece.reset();
        bIsDragging = false;
    }

    // -------------------------------------------------------------------------
    // Movement Logic
    // -------------------------------------------------------------------------
    void Board::TryMovePiece(shared<ChessPiece> Piece, const sf::Vector2i& TargetGridPos)
    {
        if (!Piece) return;

        sf::Vector2i From = Piece->GetGridPosition();
        sf::Vector2i To = TargetGridPos;
        sf::Vector2i FinalGridPos = To;
        bool bMoveIsSuccessful = true;

        // ------ Check move validity --------------------
        if (!IsMoveValid(Piece, From, To))
        {
            FinalGridPos = DragStartGridPosition;
            bMoveIsSuccessful = false;

           // LOG("Invalid Move: %s cannot move from %s to %s.", GetPieceName(Piece->GetPieceType()).c_str(), GridToAlgebraic(From).c_str(), GridToAlgebraic(To).c_str());
        }
        else
        {
            // ---- Check for captures ----------------
            shared<ChessPiece> CapturedPiece = nullptr;

            for (const auto& OtherPiece : Pieces)
            {
                if (!OtherPiece || OtherPiece->IsPendingDestroy() || OtherPiece == Piece) { continue; }

                if (OtherPiece->GetGridPosition() == To)
                {
                    if (OtherPiece->GetColor() == Piece->GetColor() || OtherPiece->GetPieceType() == EChessPieceType::King)
                    {
                        // Cannot capture own piece or king
                        FinalGridPos = DragStartGridPosition;
                        bMoveIsSuccessful = false;
                    }
                    else
                    {
                        CapturedPiece = OtherPiece;
                    }
                    break;
                }
            }

            // ------ EN-PASSANT CAPTURE CHECK -------------
            if (!CapturedPiece && Piece->GetPieceType() == EChessPieceType::Pawn)
            {
                int direction = (Piece->GetColor() == EChessColor::White)? -1 : 1;

                // Pawn moves diagonally but the target square is empty -> possible en-passant
                if (std::abs(To.x - From.x) == 1 && To.y - From.y == direction)
                {
                    sf::Vector2i enemyPos = { To.x, From.y }; // the bypassed pawn

                    for (auto& OtherPiece : Pieces)
                    {
                        if (!OtherPiece || OtherPiece->IsPendingDestroy()) { continue; }

                        if (OtherPiece->GetGridPosition() == enemyPos && OtherPiece->GetPieceType() == EChessPieceType::Pawn && OtherPiece->GetColor() != Piece->GetColor() && OtherPiece->GetWasPawnMovedTwo())
                        {
                            CapturedPiece = OtherPiece;
                            break;
                        }
                    }
                }
            }

            // ------ Execute capture -------------
            if (CapturedPiece)
            {
                sf::Vector2i capturedPos = CapturedPiece->GetGridPosition();
                CapturedPiece->Destroy();

                //LOG("Captured %s at %s!", GetPieceName(CapturedPiece->GetPieceType()).c_str(), GridToAlgebraic(capturedPos).c_str());
            }
        }

        // ------ UPDATE PAWN DOUBLE-MOVE FLAG -------------
        if (Piece->GetPieceType() == EChessPieceType::Pawn)
        {
            int moveDist = std::abs(To.y - From.y);

            // This pawn moved two squares -> eligible to be captured en-passant next turn
            Piece->SetWasPawnMovedTwo(moveDist == 2);

            // All other pawns lose their en-passant flag
            for (auto& Pawns : Pieces)
            {
                if (Pawns && Pawns != Piece && Pawns->GetPieceType() == EChessPieceType::Pawn)
                {
                    Pawns->SetWasPawnMovedTwo(false);
                }
            }
        }

        // ------ CASTLING ROOK MOVEMENT -------------
        if (Piece->GetPieceType() == EChessPieceType::King)
        {
            int dx = FinalGridPos.x - From.x;

            // King-side castling (move rook from h-file)
            if (dx == 2)
            {
                sf::Vector2i rookFrom = { 7, From.y };
                sf::Vector2i rookTo = { FinalGridPos.x - 1, From.y };

                shared<ChessPiece> Rook = GetPieceAt(rookFrom);
                if (Rook && Rook->GetPieceType() == EChessPieceType::Rook && !Rook->GetHasMoved())
                {
                    Rook->SetGridPosition(rookTo);
                    Rook->SetHasMoved();
                }
            }

            // Queen-side castling (move rook from a-file)
            else if (dx == -2)
            {
                sf::Vector2i rookFrom = { 0, From.y };
                sf::Vector2i rookTo = { FinalGridPos.x + 1, From.y };

                shared<ChessPiece> Rook = GetPieceAt(rookFrom);
                if (Rook && Rook->GetPieceType() == EChessPieceType::Rook && !Rook->GetHasMoved())
                {
                    Rook->SetGridPosition(rookTo);
                    Rook->SetHasMoved();
                }
            }
        }

        // ----- Move piece and clean up -----------
        Piece->SetGridPosition(FinalGridPos);
        CleanupDragState(Piece);

        // ------ Logging and turn switch -----------
        std::string name = GetPieceName(Piece->GetPieceType());
        std::string start = GridToAlgebraic(From);
        std::string end = GridToAlgebraic(FinalGridPos);

        if (bMoveIsSuccessful && FinalGridPos != From)
        {
            //LOG("%s to %s", name.c_str(), end.c_str());
            Piece->SetHasMoved();
            SwitchTurn();
        }
        else if (!bMoveIsSuccessful)
        {
            //LOG("Invalid Move: %s returned to %s.", name.c_str(), start.c_str());
        }
    }

    // -------------------------------------------------------------------------
    // Drag Start
    // -------------------------------------------------------------------------
    void Board::HandleDragStart(const sf::Vector2f& MousePos)
    {
        for (auto it = Pieces.rbegin(); it != Pieces.rend(); ++it)
        {
            shared<ChessPiece> Piece = *it;

            if (Piece && !Piece->IsPendingDestroy() && Piece->IsPointInBounds(MousePos))
            {
                // ---- TURN CHECK -----------------------
                if (!IsPlayersPiece(Piece.get())) { return; }

                bIsDragging = true;
                DraggingPiece = Piece;
                DragStartGridPosition = Piece->GetGridPosition();

                Piece->SetSelected(true);
                Piece->SetHovered(false);

                return;
            }
        }
    }

    // -------------------------------------------------------------------------
    // Drag Tick
    // -------------------------------------------------------------------------
    void Board::HandleDragTick(const sf::Vector2f& MousePos)
    {
        if (auto Piece = DraggingPiece.lock())
        {
            Piece->SetActorLocation(MousePos);
        }
    }

    // -------------------------------------------------------------------------
    // Drag End
    // -------------------------------------------------------------------------
    void Board::HandleDragEnd(const sf::Vector2f& MousePos)
    {
        if (auto Piece = DraggingPiece.lock())
        {
            if (!IsPlayersPiece(Piece.get()))
            {
                Piece->SetGridPosition(DragStartGridPosition);
                CleanupDragState(Piece);
                bIsDragging = false;
                DraggingPiece.reset();
                return;
            }

            if (!IsWorldPositionInGridBounds(MousePos))
            {
                Piece->SetGridPosition(DragStartGridPosition);
                //LOG("Invalid Move: Dropped out of bounds. %s returned to %s.", GetPieceName(Piece->GetPieceType()).c_str(), GridToAlgebraic(DragStartGridPosition).c_str());

                CleanupDragState(Piece);
                return;
            }

            sf::Vector2i DropGrid = WorldToGrid(MousePos);
            TryMovePiece(Piece, DropGrid);
        }

        bIsDragging = false;
        DraggingPiece.reset();
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
