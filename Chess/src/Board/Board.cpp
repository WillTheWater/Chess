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
        if (bIsDragging) { return; }

        sf::Vector2i gridPos = WorldToGrid(MouseWorldPosition);

        if (gridPos != HoveredGridPos)
        {
            HoveredGridPos = gridPos;

            shared<ChessPiece> piece = GetPieceAt(gridPos);

            for (auto& p : Pieces)
            {
                if (p && p != piece)
                {
                    p->SetHovered(false);
                }
            }

            if (piece && IsPlayersPiece(piece.get()))
            {
                piece->SetHovered(true);
            }
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

        if (!piece) { return; }

        if (!IsPlayersPiece(piece.get())) { return; }

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
            sf::Vector2i to = WorldToGrid(MouseWorldPos);
            sf::Vector2i from = piece->GetGridPosition();

            if (HandleMove(piece, from, to))
            {
                piece->SetActorLocation(GridToCenterSquare(to));
            }
            else
            {
                piece->SetActorLocation(GridToCenterSquare(from));
            }
            piece->SetSelected(false);
        }
        SelectedPiece.reset();
        bIsDragging = false;
    }

    bool Board::IsInBounds(const sf::Vector2f& WorldPos)
    {
        float GridWidth = GridSize * SquareSize;
        float GridHeight = GridSize * SquareSize;

        float MinX = GRID_ABS_OFFSET_X;
        float MinY = GRID_ABS_OFFSET_Y;

        float MaxX = MinX + GridWidth;
        float MaxY = MinY + GridHeight;

        return WorldPos.x >= MinX && WorldPos.x < MaxX && WorldPos.y >= MinY && WorldPos.y < MaxY;
    }

    bool Board::IsInBounds(const sf::Vector2i& GridPos) const
    {
        return GridPos.x >= 0 && GridPos.x < GridSize && GridPos.y >= 0 && GridPos.y < GridSize;
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
        if (!Piece || !IsPlayersPiece(Piece.get()) || From == To || !IsInBounds(To)) { return false; }

        if (auto target = GetPieceAt(To))
        {
            if (target->GetColor() == Piece->GetColor()) { return false; }
        }

        switch (Piece->GetPieceType())
        {
        case EChessPieceType::Rook:   return IsRookMoveValid(Piece, From, To);
        case EChessPieceType::Bishop: return IsBishopMoveValid(Piece, From, To);
        case EChessPieceType::Queen:  return IsQueenMoveValid(Piece, From, To);
        case EChessPieceType::King:   return IsKingMoveValid(Piece, From, To);
        case EChessPieceType::Knight: return IsKnightMoveValid(Piece, From, To);
        case EChessPieceType::Pawn:   return IsPawnMoveValid(Piece, From, To);
        default: return false;
        }
    }

    bool Board::IsMoveLegal(shared<ChessPiece> Piece, sf::Vector2i From, sf::Vector2i To)
    {
        if (IsMoveValid(Piece, From, To))
        {
            shared<ChessPiece> TargetPiece = BoardGrid[To.x][To.y];

            BoardGrid[From.x][From.y] = nullptr;
            BoardGrid[To.x][To.y] = Piece;
            Piece->SetGridPosition(To);

            shared<ChessPiece> King = nullptr;
            for (const auto& P : Pieces)
            {
                if (P && P->GetPieceType() == EChessPieceType::King && P->GetColor() == Piece->GetColor())
                {
                    King = P;
                    break;
                }
            }

            bool bIsLegal = true;
            if (King)
            {
                if (IsSquareAttacked(King->GetGridPosition(), King->GetColor()))
                {
                    bIsLegal = false;
                }
            }

            BoardGrid[From.x][From.y] = Piece;
            BoardGrid[To.x][To.y] = TargetPiece;
            Piece->SetGridPosition(From);

            return bIsLegal;
        }
        return false;
    }
   
    bool Board::HandleMove(shared<ChessPiece> piece, sf::Vector2i from, sf::Vector2i to)
    {
        if (IsMoveValid(piece, from, to) && IsMoveLegal(piece, from, to))
        {
            EnPassant(piece, to, from);
            HandleCapture(to);

            BoardGrid[from.x][from.y] = nullptr;
            BoardGrid[to.x][to.y] = piece;

            piece->SetGridPosition(to);
            piece->SetHasMoved();

            SetEnPassantFlag(piece, to, from);
            PromotePawn(piece, to);
            HandleCastle(piece, from, to);
            SetCheckFlag();

            SwitchTurn();
            return true;
        }
        return false;
    }

    void Board::PromotePawn(we::shared<we::ChessPiece>& piece, sf::Vector2i& to)
    {
        if (piece->GetPieceType() == EChessPieceType::Pawn)
        {
            bool reachedBackRank = (piece->GetColor() == EChessColor::White && to.y == 0) ||
                (piece->GetColor() == EChessColor::Black && to.y == 7);

            if (reachedBackRank)
            {
                HandlePromotePawn(piece, to);
            }
        }
    }

    void Board::HandlePromotePawn(shared<ChessPiece>& pawn, const sf::Vector2i& pos)
    {
        BoardGrid[pos.x][pos.y] = nullptr;
        Pieces.erase(std::remove(Pieces.begin(), Pieces.end(), pawn), Pieces.end());
        pawn->Destroy();

        // TODO: Piece Selection
        SpawnPiece(EChessPieceType::Queen, pawn->GetColor(), pos);
    }

    void Board::SetEnPassantFlag(we::shared<we::ChessPiece>& piece, sf::Vector2i& to, sf::Vector2i& from)
    {
        // ---------- Set the "moved two" flag for pawns ----------
        if (piece->GetPieceType() == EChessPieceType::Pawn)
        {
            bool movedTwo = std::abs(to.y - from.y) == 2;
            piece->SetWasPawnMovedTwo(movedTwo);
        }
    }

    void Board::EnPassant(we::shared<we::ChessPiece>& piece, sf::Vector2i& to, sf::Vector2i& from)
    {
        if (piece->GetPieceType() == EChessPieceType::Pawn)
        {
            int direction = (piece->GetColor() == EChessColor::White) ? -1 : 1;

            if (std::abs(to.x - from.x) == 1 && (to.y - from.y) == direction)
            {
                if (GetPieceAt(to) == nullptr)
                {
                    sf::Vector2i sidePawnPos{ to.x, from.y };

                    if (auto sidePawn = GetPieceAt(sidePawnPos))
                    {
                        if (sidePawn->GetPieceType() == EChessPieceType::Pawn &&
                            sidePawn->GetWasPawnMovedTwo())
                        {
                            HandleCapture(sidePawnPos);
                        }
                    }
                }
            }
        }
    }

    void Board::HandleCastle(shared<ChessPiece> King, sf::Vector2i From, sf::Vector2i To)
    {
        if (!King || King->GetPieceType() != EChessPieceType::King)
            return;

        int deltaX = To.x - From.x;

        if (std::abs(deltaX) != 2)
            return;

        bool bKingside = (deltaX > 0);

        int rookX = bKingside ? 7 : 0;
        sf::Vector2i RookPos(rookX, From.y);

        auto Rook = GetPieceAt(RookPos);
        if (!Rook || Rook->GetPieceType() != EChessPieceType::Rook)
            return;

        sf::Vector2i NewRookPos = bKingside ? sf::Vector2i(To.x - 1, From.y) : sf::Vector2i(To.x + 1, From.y);

        BoardGrid[RookPos.x][RookPos.y] = nullptr;
        BoardGrid[NewRookPos.x][NewRookPos.y] = Rook;

        Rook->SetGridPosition(NewRookPos);
        Rook->SetActorLocation(GridToCenterSquare(NewRookPos));
        Rook->SetHasMoved();
    }

    void Board::HandleCapture(const sf::Vector2i& GridPos)
    {
        if (auto target = GetPieceAt(GridPos))
        {
            BoardGrid[GridPos.x][GridPos.y] = nullptr;
            Pieces.erase(std::remove(Pieces.begin(), Pieces.end(), target), Pieces.end());
            target->Destroy(); 
        }
    }

    bool Board::IsSquareAttacked(const sf::Vector2i& Pos, EChessColor DefenderColor) const
    {
        EChessColor AttackerColor = (DefenderColor == EChessColor::White) ? EChessColor::Black : EChessColor::White;

        for (const auto& Piece : Pieces)
        {
            if (!Piece || Piece->GetColor() != AttackerColor)
                continue;

            sf::Vector2i From = Piece->GetGridPosition();

            switch (Piece->GetPieceType())
            {
                case EChessPieceType::Pawn:
                {
                    int dir = (AttackerColor == EChessColor::White) ? -1 : 1;
                    if (std::abs(Pos.x - From.x) == 1 && Pos.y - From.y == dir)
                        return true;
                    break;
                }

                case EChessPieceType::Knight:
                {
                    int dx = std::abs(Pos.x - From.x);
                    int dy = std::abs(Pos.y - From.y);
                    if ((dx == 2 && dy == 1) || (dx == 1 && dy == 2))
                        return true;
                    break;
                }

                case EChessPieceType::King:
                {
                    int dx = std::abs(Pos.x - From.x);
                    int dy = std::abs(Pos.y - From.y);
                    if (dx <= 1 && dy <= 1)
                        return true;
                    break;
                }

                case EChessPieceType::Rook:
                case EChessPieceType::Bishop:
                case EChessPieceType::Queen:
                {
                    int dx = Pos.x - From.x;
                    int dy = Pos.y - From.y;

                    int stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
                    int stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);

                    if ((Piece->GetPieceType() == EChessPieceType::Rook && stepX != 0 && stepY != 0) ||
                        (Piece->GetPieceType() == EChessPieceType::Bishop && std::abs(stepX) != std::abs(stepY)))
                        break;

                    sf::Vector2i checkPos = From;
                    while (true)
                    {
                        checkPos.x += stepX;
                        checkPos.y += stepY;

                        if (checkPos == Pos)
                            return true;

                        if (BoardGrid[checkPos.x][checkPos.y])
                            break;

                        if (checkPos.x < 0 || checkPos.x >= GridSize || checkPos.y < 0 || checkPos.y >= GridSize)
                            break;
                    }

                    break;
                }
            }
        }

        return false;
    }

    void Board::SetCheckFlag()
    {
        for (const auto& Piece : Pieces)
        {
            if (Piece->GetPieceType() != EChessPieceType::King) { continue; }

            bool bInCheck = IsSquareAttacked(Piece->GetGridPosition(), Piece->GetColor());
            Piece->SetIsInCheck(bInCheck);

            if (bInCheck)
            {
                if (Piece->GetColor() == EChessColor::White)
                {
                    LOG("White King is in Check!")
                }
                else
                {
                    LOG("Black King is in Check!")
                }
            }

            // TODO: Checkmate
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
            if (GetPieceAt(pos)) { return false; }

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
            if (GetPieceAt(pos)) { return false; }

            pos.x += dx;
            pos.y += dy;
        }

        return true;
    }

    bool Board::IsKnightMoveValid(shared<ChessPiece> Piece, sf::Vector2i From, sf::Vector2i To) const
    {
        if (From == To) { return false; }

        int deltaX = std::abs(To.x - From.x);
        int deltaY = std::abs(To.y - From.y);

        return ((deltaX == 1 && deltaY == 2) || (deltaX == 2 && deltaY == 1));
    }

    bool Board::IsKingMoveValid(shared<ChessPiece> Piece, sf::Vector2i From, sf::Vector2i To) const
    {
        if (!Piece || From == To || Piece->GetPieceType() != EChessPieceType::King)
            return false;

        int signedDx = To.x - From.x;
        int deltaX = std::abs(signedDx);
        int deltaY = std::abs(To.y - From.y);

        // ---- Standard Movement ----
        if (deltaX <= 1 && deltaY <= 1)
        {
            auto target = GetPieceAt(To);

            if (target && target->GetColor() == Piece->GetColor())
                return false;

            if (!target && IsSquareAttacked(To, Piece->GetColor()))
                return false;

            return true;
        }

        // ---- Castling ----
        if (deltaY == 0 && deltaX == 2 && !Piece->GetHasMoved())
        {
            bool bKingside = (signedDx > 0);
            int rookX = bKingside ? 7 : 0;
            sf::Vector2i RookPos(rookX, From.y);

            auto Rook = GetPieceAt(RookPos);
            if (!Rook || Rook->GetPieceType() != EChessPieceType::Rook || Rook->GetHasMoved()) { return false; }

            int start = std::min(From.x, rookX) + 1;
            int end = std::max(From.x, rookX) - 1;
            for (int x = start; x <= end; x++)
            {
                if (GetPieceAt({ x, From.y })) { return false; }
            }

            if (Piece->GetIsInCheck() || IsSquareAttacked({ From.x + (bKingside ? 1 : -1), From.y }, Piece->GetColor()) || IsSquareAttacked(To, Piece->GetColor()))
            {
                return false;
            }
            return true;
        }

        return false;
    }

    bool Board::IsPawnMoveValid(shared<ChessPiece> Piece, sf::Vector2i From, sf::Vector2i To) const
    {
        if (!Piece || From == To)
            return false;

        const int direction = (Piece->GetColor() == EChessColor::White) ? -1 : 1;
        const int deltaX = To.x - From.x;
        const int deltaY = To.y - From.y;

        // ---------- Single move ----------
        if (deltaX == 0 && deltaY == direction)
        {
            return GetPieceAt(To) == nullptr;
        }

        // ---------- Double move ----------
        if (!Piece->GetHasMoved() && deltaX == 0 && deltaY == 2 * direction)
        {
            sf::Vector2i intermediate{ From.x, From.y + direction };
            return GetPieceAt(intermediate) == nullptr && GetPieceAt(To) == nullptr;
        }

        // ---------- Normal capture ----------
        if (std::abs(deltaX) == 1 && deltaY == direction)
        {
            if (auto target = GetPieceAt(To))
            {
                return target->GetColor() != Piece->GetColor();
            }

            // ---------- En-Passant ----------
            sf::Vector2i sidePawnPos{ To.x, From.y };

            if (auto sidePawn = GetPieceAt(sidePawnPos))
            {
                if (sidePawn->GetPieceType() == EChessPieceType::Pawn &&
                    sidePawn->GetColor() != Piece->GetColor() &&
                    sidePawn->GetWasPawnMovedTwo())
                {
                    return true;
                }
            }
            return false;
        }

        return false;
    }

    bool Board::IsPlayersPiece(const ChessPiece* Piece) const
    {
        return (CurrentTurn == EPlayerTurn::White && Piece->GetColor() == EChessColor::White) || (CurrentTurn == EPlayerTurn::Black && Piece->GetColor() == EChessColor::Black);
    }

    void Board::SwitchTurn()
    {
        CurrentTurn = (CurrentTurn == EPlayerTurn::White) ? EPlayerTurn::Black : EPlayerTurn::White;
        //const char* TurnName = (CurrentTurn == EPlayerTurn::White) ? "White" : "Black";
    }
}