#include "Board/Board.h"
#include "Framework/Renderer.h"
#include "Framework/World.h"
#include <sstream>

namespace we
{
    Board::Board(World* OwningWorld, const std::string& TexturePath)
        : Actor{ OwningWorld, TexturePath }
        , Pieces{}
    {
    }

    void Board::BeginPlay()
    {
        SetActorLocation(sf::Vector2f{ float(GetWindowSize().x) / 2.0f, float(GetWindowSize().y) / 2.0f });
        InitializeBoard();
    }

    void Board::Tick(float DeltaTime)
    {
        HandleInput();
    }

    void Board::Render(Renderer& GameRenderer)
    {
        Actor::Render(GameRenderer);
        sf::RenderWindow& Window = GameRenderer.GetRenderWindow();
        MousePixelPosition = sf::Mouse::getPosition(Window);
        MouseWorldPosition = Window.mapPixelToCoords(MousePixelPosition);

        for (const auto& Piece : Pieces)
        {
            if (Piece && Piece != SelectedPiece.lock())
            {
                Piece->Render(GameRenderer);
            }
        }

        if (auto Dragged = SelectedPiece.lock())
        {
            Dragged->Render(GameRenderer);
        }
    }

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
        CurrentTurn = EPlayerTurn::White;
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
    // Input Handling
    // -------------------------------------------------------------------------
    void Board::HandleInput()
    {
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
        {
            HandleMouseHover();
        }
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

            auto MoveSim = HandleMove(piece, from, to);

            if (MoveSim.has_value())
            {
                UpdateBoard(MoveSim.value());
                SwitchTurn();
            }
            else
            {
                piece->SetActorLocation(GridToCenterSquare(from));
            }

            SelectedPiece.reset();
            bIsDragging = false;
        }
    }

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
        return BoardGrid[GridPos.x][GridPos.y];
    }

    shared<ChessPiece> Board::GetPieceAt(shared<ChessPiece> InBoard[GridSize][GridSize], const sf::Vector2i& GridPos) const
    {
        return InBoard[GridPos.x][GridPos.y];
    }

    sf::Vector2i Board::GetKing(shared<ChessPiece> Board[GridSize][GridSize], EChessColor Color)
    {
        for (int x = 0; x < GridSize; ++x)
            for (int y = 0; y < GridSize; ++y)
                if (Board[x][y] &&
                    Board[x][y]->GetPieceType() == EChessPieceType::King &&
                    Board[x][y]->GetColor() == Color)
                    return { x, y };

        return { -1, -1 };
    }

    bool Board::IsPlayersPiece(const ChessPiece* Piece) const
    {
        return (CurrentTurn == EPlayerTurn::White && Piece->GetColor() == EChessColor::White) || (CurrentTurn == EPlayerTurn::Black && Piece->GetColor() == EChessColor::Black);
    }

    void Board::UpdateBoard(const MoveResult& Result)
    {
        if (!Result.bValid) { return; }

        // ----------------------------------------------------
        // Handle Capture
        // ----------------------------------------------------
        if (Result.CapturedPiece)
        {
            Capture(Result.CapturedPiece);
        }

        // ----------------------------------------------------
        // Move Piece
        // ----------------------------------------------------

        Move(BoardGrid[Result.From.x][Result.From.y], Result.From, Result.To);

        // ----------------------------------------------------
        // Handle Castling
        // ----------------------------------------------------
        if (Result.bCastling)
        {
            Castle(BoardGrid[Result.RookFrom.x][Result.RookFrom.y], Result.RookFrom, Result.RookTo);
        }

        // ----------------------------------------------------
        // Pawn Promotion 
        // ----------------------------------------------------
        if (Result.bPawnPromoted)
        {
            PromotePawn(Result.To, Result.PromotionType);
        }

        // ----------------------------------------------------
        // Check / Checkmate / Stalemate / Draw
        // ----------------------------------------------------
        if (Result.bIsCheckmate)
        {
            OnCheckmate.Broadcast();
        }
        else if (Result.bIsStalemate)
        {
            OnStalemate.Broadcast();
        }
        else if (Result.bIsDraw)
        {
            LOG("Draw")
        }
        else if (Result.bIsCheck)
        {
            LOG("Check!")
        }
    }

    bool Board::IsInBounds(const sf::Vector2i& GridPos) const
    {
        return GridPos.x >= 0 && GridPos.x < GridSize && GridPos.y >= 0 && GridPos.y < GridSize;
    }

    // -------------------------------------------------------------------------
    // Game Logic
    // -------------------------------------------------------------------------
    bool Board::IsMoveValid(shared<ChessPiece> Board[GridSize][GridSize], shared<ChessPiece> Piece, sf::Vector2i From, sf::Vector2i To) const
    {
        if (!Piece || From == To || !IsInBounds(To)) { return false; }

        if (auto target = Board[To.x][To.y])
        {
            if (target->GetColor() == Piece->GetColor())
                return false;
        }

        switch (Piece->GetPieceType())
        {
        case EChessPieceType::Rook:
            return IsRookMoveValid(Board, From, To);

        case EChessPieceType::Bishop:
            return IsBishopMoveValid(Board, From, To);

        case EChessPieceType::Queen:
            return IsQueenMoveValid(Board, From, To);

        case EChessPieceType::Knight:
            return IsKnightMoveValid(From, To);

        case EChessPieceType::King:
            return IsKingMoveValid(Board, Piece, From, To);

        case EChessPieceType::Pawn:
            return IsPawnMoveValid(Board, Piece, From, To);

        default:
            return false;
        }
    }

    bool Board::IsMoveLegal(shared<ChessPiece> Board[GridSize][GridSize], shared<ChessPiece> Piece, sf::Vector2i From, sf::Vector2i To)
    {
        auto captured = Board[To.x][To.y];

        Board[To.x][To.y] = Piece;
        Board[From.x][From.y] = nullptr;

        sf::Vector2i kingPos =
            (Piece->GetPieceType() == EChessPieceType::King)
            ? To
            : GetKing(Board, Piece->GetColor());

        bool legal =
            kingPos.x != -1 &&
            !IsSquareAttacked(Board, kingPos, Piece->GetColor());

        Board[From.x][From.y] = Piece;
        Board[To.x][To.y] = captured;

        return legal;
    }

    optional<MoveResult> Board::HandleMove(shared<ChessPiece> Piece, sf::Vector2i From, sf::Vector2i To)
    {
        // ----------------------------------------------------
        // Create Simulation
        // ----------------------------------------------------
        shared<ChessPiece> SimBoard[GridSize][GridSize] = {};
        for (int x = 0; x < GridSize; ++x)
            for (int y = 0; y < GridSize; ++y)
                SimBoard[x][y] = BoardGrid[x][y];

        // ----------------------------------------------------
        // Validate Move
        // ----------------------------------------------------
        if (!IsMoveValid(SimBoard, Piece, From, To) || !IsMoveLegal(SimBoard, Piece, From, To)) { return std::nullopt; }

        auto Captured = SimBoard[To.x][To.y];
        SimBoard[From.x][From.y] = nullptr;
        SimBoard[To.x][To.y] = Piece;

        MoveResult Result{};
        Result.bValid = true;
        Result.From = From;
        Result.To = To;
        Result.CapturedPiece = Captured;

        // ----------------------------------------------------
        // En-Passant Detection
        // ----------------------------------------------------
        EnPassant(Piece, To, From, SimBoard, Result);

        // ----------------------------------------------------
        // Castling detection
        // ----------------------------------------------------
        Castle(Piece, To, From, Result);

        // ----------------------------------------------------
        // Pawn promotion
        // ----------------------------------------------------
        PromotePawn(Piece, To, Result);

        // ----------------------------------------------------
        // Check detection
        // ----------------------------------------------------
        EChessColor OpponentColor = (Piece->GetColor() == EChessColor::White) ? EChessColor::Black : EChessColor::White;

        sf::Vector2i OpponentKing = GetKing(SimBoard, OpponentColor);

        Result.bIsCheck = (OpponentKing.x != -1) && IsSquareAttacked(SimBoard, OpponentKing, OpponentColor);

        // ----------------------------------------------------
        // Checkmate / Stalemate / Draw
        // ----------------------------------------------------
        CheckmateOrStalemate(SimBoard, OpponentColor, Result);
        Draw(SimBoard, Result);

        return Result;
    }

    void Board::CheckmateOrStalemate(shared<ChessPiece>  SimBoard[GridSize][GridSize], EChessColor OpponentColor, MoveResult& Result)
    {
        bool bOpponentHasMove = false;

        for (int fx = 0; fx < GridSize && !bOpponentHasMove; ++fx)
        {
            for (int fy = 0; fy < GridSize && !bOpponentHasMove; ++fy)
            {
                auto OppPiece = SimBoard[fx][fy];
                if (!OppPiece || OppPiece->GetColor() != OpponentColor)
                    continue;

                sf::Vector2i OppFrom{ fx, fy };

                for (int tx = 0; tx < GridSize && !bOpponentHasMove; ++tx)
                {
                    for (int ty = 0; ty < GridSize; ++ty)
                    {
                        sf::Vector2i OppTo{ tx, ty };

                        if (!IsMoveValid(SimBoard, OppPiece, OppFrom, OppTo))
                            continue;

                        if (IsMoveLegal(SimBoard, OppPiece, OppFrom, OppTo))
                        {
                            bOpponentHasMove = true;
                            break;
                        }
                    }
                }
            }
        }

        if (Result.bIsCheck)
        {
            Result.bIsCheckmate = !bOpponentHasMove;
        }
        else
        {
            Result.bIsStalemate = !bOpponentHasMove;
        }

        Result.bIsDraw = Result.bIsStalemate;
    }

    void Board::Draw(shared<ChessPiece> SimBoard[GridSize][GridSize], MoveResult& Result)
    {
    }

    void Board::Capture(shared<ChessPiece> TargetPiece)
    {
        if (!TargetPiece) return;

        sf::Vector2i GridPos = TargetPiece->GetGridPosition();

        BoardGrid[GridPos.x][GridPos.y] = nullptr;
        Pieces.erase(std::remove(Pieces.begin(), Pieces.end(), TargetPiece), Pieces.end());

        TargetPiece->Destroy();
    }

    void Board::EnPassant(shared<ChessPiece>& Piece, sf::Vector2i& To, sf::Vector2i& From, shared<ChessPiece>  SimBoard[GridSize][GridSize], MoveResult& Result)
    {
        if (Piece->GetPieceType() == EChessPieceType::Pawn)
        {
            int dy = To.y - From.y;
            int dir = (Piece->GetColor() == EChessColor::White) ? -1 : 1;

            Piece->SetWasPawnMovedTwo(std::abs(dy) == 2);

            for (int x = 0; x < GridSize; ++x)
            {
                for (int y = 0; y < GridSize; ++y)
                {
                    auto P = SimBoard[x][y];
                    if (P && P != Piece && P->GetPieceType() == EChessPieceType::Pawn &&
                        P->GetColor() == Piece->GetColor())
                    {
                        P->SetWasPawnMovedTwo(false);
                    }
                }
            }

            if (std::abs(To.x - From.x) == 1 && dy == dir && !BoardGrid[To.x][To.y])
            {
                sf::Vector2i sidePawnPos{ To.x, From.y };
                auto sidePawn = BoardGrid[sidePawnPos.x][sidePawnPos.y];

                if (sidePawn && sidePawn->GetPieceType() == EChessPieceType::Pawn &&
                    sidePawn->GetWasPawnMovedTwo())
                {
                    Result.bEnPassant = true;
                    Result.CapturedPiece = sidePawn;
                }
            }
        }
    }

    void Board::Move(shared<ChessPiece> PieceToMove, sf::Vector2i From, sf::Vector2i To)
    {
        BoardGrid[From.x][From.y] = nullptr;
        BoardGrid[To.x][To.y] = PieceToMove;

        PieceToMove->SetGridPosition(To);
        PieceToMove->SetActorLocation(GridToCenterSquare(To));
        PieceToMove->SetHasMoved();
    }

    void Board::Castle(shared<ChessPiece> Rook, sf::Vector2i From, sf::Vector2i To)
    {
        BoardGrid[From.x][From.y] = nullptr;
        BoardGrid[To.x][To.y] = Rook;

        Rook->SetGridPosition(To);
        Rook->SetActorLocation(GridToCenterSquare(To));
        Rook->SetHasMoved();
    }

    void Board::Castle(shared<ChessPiece>& Piece, sf::Vector2i& To, sf::Vector2i& From, MoveResult& Result)
    {
        if (Piece->GetPieceType() == EChessPieceType::King &&
            std::abs(To.x - From.x) == 2)
        {
            Result.bCastling = true;

            int RookFromX = (To.x > From.x) ? 7 : 0;
            int RookToX = (To.x > From.x) ? To.x - 1 : To.x + 1;

            Result.RookFrom = { RookFromX, From.y };
            Result.RookTo = { RookToX, From.y };
        }
    }

    void Board::PromotePawn(const sf::Vector2i& pos, EChessPieceType PromotionType)
    {
        shared<ChessPiece> pawn = BoardGrid[pos.x][pos.y];
        EChessColor PieceColor = pawn->GetColor();
        if (!pawn || pawn->GetPieceType() != EChessPieceType::Pawn) { return; }

        BoardGrid[pos.x][pos.y] = nullptr;
        Pieces.erase(std::remove(Pieces.begin(), Pieces.end(), pawn), Pieces.end());
        pawn->Destroy();

        SpawnPiece(PromotionType, PieceColor, pos);
    }

    void Board::PromotePawn(shared<ChessPiece>& Piece, sf::Vector2i& To, MoveResult& Result)
    {
        if (Piece->GetPieceType() == EChessPieceType::Pawn)
        {
            int LastRank = (Piece->GetColor() == EChessColor::White) ? 0 : 7;
            if (To.y == LastRank)
            {
                Result.bPawnPromoted = true;
                Result.PromotionType = EChessPieceType::Queen;
            }
        }
    }

    bool Board::IsSquareAttacked(shared<ChessPiece> Board[GridSize][GridSize], const sf::Vector2i& Pos, EChessColor DefenderColor) const
    {
        EChessColor AttackerColor = (DefenderColor == EChessColor::White) ? EChessColor::Black : EChessColor::White;

        for (int x = 0; x < GridSize; ++x)
        {
            for (int y = 0; y < GridSize; ++y)
            {
                auto Piece = Board[x][y];
                if (!Piece || Piece->GetColor() != AttackerColor)
                    continue;

                sf::Vector2i From{ x, y };

                switch (Piece->GetPieceType())
                {
                case EChessPieceType::Pawn:
                {
                    int dir = (AttackerColor == EChessColor::White) ? -1 : 1;

                    if (Pos.y - From.y == dir &&
                        std::abs(Pos.x - From.x) == 1)
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

                    if (Piece->GetPieceType() == EChessPieceType::Rook &&
                        dx != 0 && dy != 0)
                        break;

                    if (Piece->GetPieceType() == EChessPieceType::Bishop &&
                        std::abs(dx) != std::abs(dy))
                        break;

                    int stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
                    int stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);

                    sf::Vector2i checkPos = From;

                    while (true)
                    {
                        checkPos.x += stepX;
                        checkPos.y += stepY;

                        if (checkPos.x < 0 || checkPos.x >= GridSize ||
                            checkPos.y < 0 || checkPos.y >= GridSize)
                            break;

                        if (checkPos == Pos)
                            return true;

                        if (Board[checkPos.x][checkPos.y])
                            break;
                    }
                    break;
                }
                }
            }
        }
        return false;
    }

    void Board::SwitchTurn()
    {
        CurrentTurn = (CurrentTurn == EPlayerTurn::White) ? EPlayerTurn::Black : EPlayerTurn::White;
    }

    bool Board::IsRookMoveValid(shared<ChessPiece> Board[GridSize][GridSize], sf::Vector2i From, sf::Vector2i To) const
    {
        if (From.x != To.x && From.y != To.y)
            return false;

        int dx = (To.x > From.x) ? 1 : (To.x < From.x ? -1 : 0);
        int dy = (To.y > From.y) ? 1 : (To.y < From.y ? -1 : 0);

        sf::Vector2i pos = From;
        pos.x += dx;
        pos.y += dy;

        while (pos != To)
        {
            if (Board[pos.x][pos.y])
                return false;

            pos.x += dx;
            pos.y += dy;
        }
        return true;
    }

    bool Board::IsBishopMoveValid(shared<ChessPiece> Board[GridSize][GridSize], sf::Vector2i From, sf::Vector2i To) const
    {
        int dx = To.x - From.x;
        int dy = To.y - From.y;

        if (std::abs(dx) != std::abs(dy))
            return false;

        dx = (dx > 0) ? 1 : -1;
        dy = (dy > 0) ? 1 : -1;

        sf::Vector2i pos = From;
        pos.x += dx;
        pos.y += dy;

        while (pos != To)
        {
            if (Board[pos.x][pos.y])
                return false;

            pos.x += dx;
            pos.y += dy;
        }
        return true;
    }

    bool Board::IsQueenMoveValid(shared<ChessPiece> Board[GridSize][GridSize], sf::Vector2i From, sf::Vector2i To) const
    {
        return IsRookMoveValid(Board, From, To) || IsBishopMoveValid(Board, From, To);
    }

    bool Board::IsKnightMoveValid(sf::Vector2i From, sf::Vector2i To) const
    {
        int dx = std::abs(To.x - From.x);
        int dy = std::abs(To.y - From.y);

        return (dx == 2 && dy == 1) || (dx == 1 && dy == 2);
    }

    bool Board::IsKingMoveValid(shared<ChessPiece> Board[GridSize][GridSize], shared<ChessPiece> Piece, sf::Vector2i From, sf::Vector2i To) const
    {
        int dx = std::abs(To.x - From.x);
        int dy = std::abs(To.y - From.y);

        // ---- Normal move ----
        if (dx <= 1 && dy <= 1)
        {
            auto captured = Board[To.x][To.y];

            if (captured && captured->GetColor() == Piece->GetColor())
                return false;

            // --- simulate king move ---
            Board[To.x][To.y] = Piece;
            Board[From.x][From.y] = nullptr;

            bool safe = !IsSquareAttacked(Board, To, Piece->GetColor());

            // --- undo ---
            Board[From.x][From.y] = Piece;
            Board[To.x][To.y] = captured;

            return safe;
        }

        // ---- Castling ----
        if (dy == 0 && dx == 2 && !Piece->GetHasMoved())
        {
            int dir = (To.x > From.x) ? 1 : -1;
            int rookX = (dir == 1) ? 7 : 0;

            auto rook = Board[rookX][From.y];
            if (!rook ||
                rook->GetPieceType() != EChessPieceType::Rook ||
                rook->GetHasMoved())
                return false;

            // Path clear
            for (int x = From.x + dir; x != rookX; x += dir)
            {
                if (Board[x][From.y])
                    return false;
            }

            // King not in / through / into check
            if (IsSquareAttacked(Board, From, Piece->GetColor()) ||
                IsSquareAttacked(Board, { From.x + dir, From.y }, Piece->GetColor()) ||
                IsSquareAttacked(Board, To, Piece->GetColor()))
                return false;

            return true;
        }

        return false;
    }

    bool Board::IsPawnMoveValid(shared<ChessPiece> Board[GridSize][GridSize], shared<ChessPiece> Piece, sf::Vector2i From, sf::Vector2i To) const
    {
        int dir = (Piece->GetColor() == EChessColor::White) ? -1 : 1;
        int dx = To.x - From.x;
        int dy = To.y - From.y;

        // ---- Forward 1 ----
        if (dx == 0 && dy == dir && !Board[To.x][To.y])
            return true;

        // ---- Forward 2 ----
        if (dx == 0 && dy == 2 * dir && !Piece->GetHasMoved())
        {
            sf::Vector2i mid{ From.x, From.y + dir };
            if (!Board[mid.x][mid.y] && !Board[To.x][To.y])
                return true;
        }

        // ---- Diagonal capture ----
        if (std::abs(dx) == 1 && dy == dir)
        {
            auto target = Board[To.x][To.y];

            // Normal capture
            if (target && target->GetColor() != Piece->GetColor())
                return true;

            // ---- En passant ----
            sf::Vector2i sidePawnPos{ To.x, From.y };
            auto sidePawn = Board[sidePawnPos.x][sidePawnPos.y];

            if (sidePawn &&
                sidePawn->GetPieceType() == EChessPieceType::Pawn &&
                sidePawn->GetColor() != Piece->GetColor() &&
                sidePawn->GetWasPawnMovedTwo())
            {
                return true;
            }
        }

        return false;
    }
}