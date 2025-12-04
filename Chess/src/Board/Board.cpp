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
        LOG("White's Turn!")
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

    void Board::InitializePieces()
    {
        const std::string Texture = "/pieces.png";

        for (int y = 0; y < GridSize; ++y)
        {
            for (int x = 0; x < GridSize; ++x)
            {
                int value = InitialBoard[y][x];
                if (value == 0) continue;

                EChessColor color = (value > 0)
                    ? EChessColor::White
                    : EChessColor::Black;

                EChessPieceType type = static_cast<EChessPieceType>(std::abs(value) - 1);

                weak<ChessPiece> newPieceWeak = GetWorld()->SpawnActor<ChessPiece>(
                    type, color, Texture
                );

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

            // ---- SKIP PIECES NOT BELONGING TO CURRENT PLAYER ----
            if (!IsPlayersPiece(Piece.get())) { continue; }

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
    // Movement / Drag-End Logic
    // -------------------------------------------------------------------------
    void Board::TryMovePiece(shared<ChessPiece> Piece, const sf::Vector2i& TargetGridPos)
    {
        sf::Vector2i FinalGridPos = TargetGridPos;
        bool bMoveIsSuccessful = true;

        shared<ChessPiece> CapturedPiece = nullptr;

        for (const auto& OtherPiece : Pieces)
        {
            if (!OtherPiece || OtherPiece->IsPendingDestroy() || OtherPiece == Piece) { continue; }

            if (OtherPiece->GetGridPosition() == TargetGridPos)
            {
                // ---- CAPTURE LOGIC ------------------------------------------
                if (OtherPiece->GetColor() == Piece->GetColor())
                {
                    // Cannot move onto your own piece
                    FinalGridPos = DragStartGridPosition;
                    bMoveIsSuccessful = false;
                }
                else if (OtherPiece->GetPieceType() == EChessPieceType::King)
                {
                    // Cannot capture king
                    FinalGridPos = DragStartGridPosition;
                    bMoveIsSuccessful = false;
                }
                else
                {
                    // Valid capture
                    CapturedPiece = OtherPiece;
                }
                break;
            }
        }

        // ---- EXECUTE MOVE -----------------------------------------------
        Piece->SetGridPosition(FinalGridPos);

        if (CapturedPiece)
        {
            CapturedPiece->Destroy();
            LOG("Captured %s at %s!",
                GetPieceName(CapturedPiece->GetPieceType()).c_str(),
                GridToAlgebraic(FinalGridPos).c_str());
        }

        CleanupDragState(Piece);

        std::string name = GetPieceName(Piece->GetPieceType());
        std::string start = GridToAlgebraic(DragStartGridPosition);
        std::string end = GridToAlgebraic(FinalGridPos);

        if (bMoveIsSuccessful && FinalGridPos != DragStartGridPosition)
        {
            LOG("%s to %s", name.c_str(), end.c_str());

            SwitchTurn();
        }
        else if (!bMoveIsSuccessful)
        {
            LOG("Invalid Move: %s returned to %s.", name.c_str(), start.c_str());
        }
        else
        {
            // No movement
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

            if (Piece &&
                !Piece->IsPendingDestroy() &&
                Piece->IsPointInBounds(MousePos))
            {
                // ---- TURN CHECK -------------------------------------------------
                if (!IsPlayersPiece(Piece.get()))
                {
                    return;
                }

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
            // SAFETY: In case something slipped through
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
                LOG("Invalid Move: Dropped out of bounds. %s returned to %s.",
                    GetPieceName(Piece->GetPieceType()).c_str(),
                    GridToAlgebraic(DragStartGridPosition).c_str());

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
        return (CurrentTurn == EPlayerTurn::White && Piece->GetColor() == EChessColor::White) ||
            (CurrentTurn == EPlayerTurn::Black && Piece->GetColor() == EChessColor::Black);
    }

    void Board::SwitchTurn()
    {
        CurrentTurn = (CurrentTurn == EPlayerTurn::White)
            ? EPlayerTurn::Black
            : EPlayerTurn::White;
        const char* TurnName = (CurrentTurn == EPlayerTurn::White) ? "White" : "Black";
        LOG("It's %s's turn.", TurnName);
    }
}
