#include "Board/Board.h"
#include "Framework/World.h"
#include "GameFramework/MovementStruct.h"

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
    // BeginPlay()
    // -------------------------------------------------------------------------
    void Board::BeginPlay()
    {
        Actor::BeginPlay();

        SetActorLocation(sf::Vector2f{
            float(GetWindowSize().x) / 2.0f,
            float(GetWindowSize().y) / 2.0f
            });

        InitializePieces();
    }

    static bool bLeftMouseButtonPressedLastFrame = false;

    // -------------------------------------------------------------------------
    // Tick()
    // -------------------------------------------------------------------------
    void Board::Tick(float DeltaTime)
    {
        sf::RenderWindow* Window = GetWorld()->GetRenderWindow();

        if (!Window)
        {
            Actor::Tick(DeltaTime);
            return;
        }

        // ---------------------------------------------------------------------
        // 1. Mouse Position
        // ---------------------------------------------------------------------
        sf::Vector2i PixelPosition = sf::Mouse::getPosition(*Window);
        sf::Vector2f MousePosition = Window->mapPixelToCoords(PixelPosition);

        bool bLeftMouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        // ---------------------------------------------------------------------
        // 2. Drag State Machine
        // ---------------------------------------------------------------------
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

        // ---------------------------------------------------------------------
        // 3. Hover Logic (only if not dragging)
        // ---------------------------------------------------------------------
        if (!bIsDragging)
        {
            HandleMouseHover(MousePosition);
        }

        Actor::Tick(DeltaTime);
    }

    // -------------------------------------------------------------------------
    // Render()
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
            if (Piece)
                Piece->Render(Window);
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
        if (GridPos.x < 0 || GridPos.x >= 8 || GridPos.y < 0 || GridPos.y >= 8)
            return "Invalid";

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

    shared<ChessPiece> Board::GetPieceAtGrid(const sf::Vector2i& GridPos) const
    {
        for (const auto& Piece : Pieces)
        {
            if (Piece && !Piece->IsPendingDestroy() && Piece->GetGridPosition() == GridPos)
                return Piece;
        }
        return nullptr;
    }

    void Board::InitializePieces()
    {
        for (int y = 0; y < GridSize; ++y)
        {
            for (int x = 0; x < GridSize; ++x)
            {
                int value = InitialBoard[y][x];
                if (value == 0) continue;

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

        LOG("Initialized %zu chess pieces.", Pieces.size());
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

                Square.setPosition({
                    GRID_ABS_OFFSET_X + x * SquareSize,
                    GRID_ABS_OFFSET_Y + y * SquareSize
                    });

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
            if (!Piece || Piece->IsPendingDestroy())
                continue;

            if (Piece->IsPointInBounds(MousePos))
                HoveredPiece = Piece;

            if (Piece->IsHovered())
                LastHoveredPiece = Piece;
        }

        if (LastHoveredPiece && LastHoveredPiece != HoveredPiece)
            LastHoveredPiece->SetHovered(false);

        if (HoveredPiece &&
            !HoveredPiece->IsSelected() &&
            !HoveredPiece->IsHovered())
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
        if (IsMoveValid(Piece, TargetGridPos))
        {
            Piece->SetGridPosition(TargetGridPos);
        }
    }

    bool Board::IsMoveValid(shared<ChessPiece> Piece, const sf::Vector2i& TargetGridPos)
    {
        const auto MovePieceType = Piece->GetPieceType();
        const auto MovePieceColor = Piece->GetColor();

        const auto& MovePiecePatterns = PieceMovePatterns.at(MovePieceType);

        const auto MovePieceStart = Piece->GetGridPosition();
        const auto MovePieceEnd = TargetGridPos;

        auto TargetPiece = GetPieceAtGrid(TargetGridPos);

        const auto MovePath = TargetGridPos - MovePieceStart;
        sf::Vector2i NormalizedDirection = { 0, 0 };

        // Start = Target
        if (MovePieceStart == TargetGridPos)
            return false;

        // Target has a piece AND it is the same color
        if (TargetPiece && TargetPiece->GetColor() == MovePieceColor)
            return false;

        // Pattern matching
        for (const auto& Pattern : MovePiecePatterns)
        {
            // Non-repeated movement
            if (!Pattern.bRepeatable)
            {
                if (MovePath == Pattern.MovementVector)
                    return true;
            }
            else // Repeated movement (rook, bishop, queen)
            {
                if (CalculateMovement(MovePath, Pattern.MovementVector, NormalizedDirection))
                {
                    if (CheckForObstruction(MovePieceStart, TargetGridPos, NormalizedDirection))
                        return true;
                }
            }
        }

        return false;
    }

    bool Board::CalculateMovement(const sf::Vector2i& MovePath, const sf::Vector2i& MovementVector, sf::Vector2i& OutDirection)
    {
        // Not Moved
        if (MovementVector.x == 0 && MovementVector.y == 0) return false;

        // Horizontal/Vertical Movement
        if (MovementVector.x == 0 && MovePath.x == 0)
        {
            if (MovePath.y % MovementVector.y == 0)
            {
                OutDirection = { 0, MovePath.y > 0 ? 1 : -1 };
                return true;
            }
        }
        else if (MovementVector.y == 0 && MovePath.y == 0)
        {
            if (MovePath.x % MovementVector.x == 0)
            {
                OutDirection = { MovePath.x > 0 ? 1 : -1, 0 };
                return true;
            }
        }
        // Diagonal Movement
        else if (std::abs(MovePath.x) == std::abs(MovePath.y) && MovePath.x % MovementVector.x == 0)
        {
            OutDirection = { MovePath.x > 0 ? 1 : -1, MovePath.y > 0 ? 1 : -1 };
            return true;
        }

        return false;
    }

    bool Board::CheckForObstruction(const sf::Vector2i& StartPos, const sf::Vector2i& EndPos, const sf::Vector2i& Direction)
    {
        sf::Vector2i CurrentPos = StartPos;

        CurrentPos += Direction;

        while (CurrentPos != EndPos)
        {
            if (GetPieceAtGrid(CurrentPos))
            {
                LOG("Obstruction found along the path")
                return false;
            }
            CurrentPos += Direction;
        }

        return true;
    }

    // -------------------------------------------------------------------------
    // Drag Start
    // -------------------------------------------------------------------------
    void Board::HandleDragStart(const sf::Vector2f& MousePos)
    {
        for (auto it = Pieces.rbegin(); it != Pieces.rend(); ++it)
        {
            shared<ChessPiece> Piece = *it;

            if (Piece &&
                !Piece->IsPendingDestroy() &&
                Piece->IsPointInBounds(MousePos))
            {
                bIsDragging = true;
                DraggingPiece = Piece;
                DragStartGridPosition = Piece->GetGridPosition();

                Piece->SetSelected(true);
                Piece->SetHovered(false);

                //LOG("Started dragging %s from %s", GetPieceName(Piece->GetPieceType()).c_str(), GridToAlgebraic(DragStartGridPosition).c_str());

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
            if (!IsWorldPositionInGridBounds(MousePos))
            {
                Piece->SetGridPosition(DragStartGridPosition);

                LOG("Invalid Move: Dropped out of bounds. %s returned to %s.", GetPieceName(Piece->GetPieceType()).c_str(), GridToAlgebraic(DragStartGridPosition).c_str());

                CleanupDragState(Piece);
                return;
            }

            sf::Vector2i DropGrid = WorldToGrid(MousePos);
            TryMovePiece(Piece, DropGrid);
        }

        bIsDragging = false;
        DraggingPiece.reset();
    }
}