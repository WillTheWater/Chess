#include "Board/Board.h"
#include "Framework/World.h"
#include <sstream>

namespace we
{
	Board::Board(World* OwningWorld, const std::string& TexturePath)
		: Actor{OwningWorld, TexturePath},
        Pieces{}
	{
		SetTexture(TexturePath);
        
	}

	void Board::BeginPlay()
	{
		Actor::BeginPlay();
        SetActorLocation(sf::Vector2f{ (float)GetWindowSize().x / 2, (float)GetWindowSize().y / 2 });
        DrawDebugGrid();
		InitializePieces();
	}

    void Board::Tick(float DeltaTime)
    {
        sf::RenderWindow* Window = GetWorld()->GetRenderWindow();

        // 2. TRACK MOUSE POSITION
        sf::Vector2i PixelPosition = sf::Mouse::getPosition(*Window);
        sf::Vector2f MousePosition = Window->mapPixelToCoords(PixelPosition);


        // 3. APPLY HOVER LOGIC
        HandleMouseHover(MousePosition);

        // 4. APPLY CLICK LOGIC
        bool bLeftMouseButtonPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        if (bLeftMouseButtonPressed && !bLeftMouseButtonPressedLastFrame)
        {
            HandleMouseClick(MousePosition);
        }

        bLeftMouseButtonPressedLastFrame = bLeftMouseButtonPressed;

        Actor::Tick(DeltaTime);
    }

    void Board::Render(sf::RenderWindow& Window)
    {
        Actor::Render(Window);

        // DEBUG Shapes
       /* for (const auto& Square : DebugGridSquares)
        {
            Window.draw(Square);
        }*/

        for (const auto& Piece : Pieces)
        {
            if (Piece)
            {
                Piece->Render(Window);
            }
        }
    }

    sf::Vector2i Board::WorldToGrid(const sf::Vector2f& WorldPos)
    {
        float RelativeX = WorldPos.x - GRID_ABS_OFFSET_X;
        float RelativeY = WorldPos.y - GRID_ABS_OFFSET_Y;

        int GridX = static_cast<int>(std::floor(RelativeX / SquareSize));
        int GridY = static_cast<int>(std::floor(RelativeY / SquareSize));

        GridX = std::max(0, std::min(GridSize - 1, GridX));
        GridY = std::max(0, std::min(GridSize - 1, GridY));

        return { GridX, GridY };
    }

    sf::Vector2f Board::GridToWorld(const sf::Vector2i& GridPos)
    {
        float PixelX = GRID_ABS_OFFSET_X + GridPos.x * SquareSize;
        float PixelY = GRID_ABS_OFFSET_Y + GridPos.y * SquareSize;

        return { PixelX, PixelY };
    }

    std::string Board::GetPieceName(EChessPieceType Type)
    {
        switch (Type)
        {
        case EChessPieceType::King: return "King";
        case EChessPieceType::Queen: return "Queen";
        case EChessPieceType::Bishop: return "Bishop";
        case EChessPieceType::Knight: return "Knight";
        case EChessPieceType::Rook: return "Rook";
        case EChessPieceType::Pawn: return "Pawn";
        default: return "Unknown";
        }
    }

    std::string Board::GridToAlgebraic(const sf::Vector2i& GridPos)
    {
        if (GridPos.x < 0 || GridPos.x >= 8 || GridPos.y < 0 || GridPos.y >= 8)
        {
            return "Invalid";
        }

        // Convert 0-7 x-coordinate to 'a'-'h' file character
        char File = 'a' + GridPos.x;

        // Convert 0-7 y-coordinate to '8'-'1' rank character
        char Rank = '8' - GridPos.y;

        std::stringstream ss;
        ss << File << Rank;
        return ss.str();
    }

    void Board::InitializePieces()
	{
        const std::string PieceTexturePath = "/pieces.png";

        for (int y = 0; y < GridSize; ++y)
        {
            for (int x = 0; x < GridSize; ++x)
            {
                int pieceValue = InitialBoard[y][x];

                if (pieceValue != 0)
                {
                    EChessColor color = (pieceValue > 0) ? EChessColor::White : EChessColor::Black;

                    EChessPieceType type = static_cast<EChessPieceType>(std::abs(pieceValue) - 1);

                    weak<ChessPiece> newPieceWeak = GetWorld()->SpawnActor<ChessPiece>(
                        type,
                        color,
                        PieceTexturePath
                    );

                    if (shared<ChessPiece> newPiece = newPieceWeak.lock())
                    {
                        newPiece->SetGridPosition(sf::Vector2i{ x, y });
                        Pieces.push_back(newPiece);
                    }
                }
            }
        }

        LOG("Initialized %zu chess pieces.", Pieces.size());
    }

    void Board::DrawDebugGrid()
    {
        sf::Vector2f SquareDimensions{ (float)SquareSize, (float)SquareSize };

        DebugGridSquares.clear();

        for (int y = 0; y < GridSize; ++y)
        {
            for (int x = 0; x < GridSize; ++x)
            {
                sf::RectangleShape Square;
                Square.setSize(SquareDimensions);

                
                float PosX = GRID_ABS_OFFSET_X + x * SquareSize;
                float PosY = GRID_ABS_OFFSET_Y + y * SquareSize;

                Square.setPosition(sf::Vector2f{ PosX, PosY });

                Square.setFillColor(sf::Color(0, 255, 0, 100));
                Square.setOutlineColor(sf::Color::Blue);
                Square.setOutlineThickness(-2.0f); 

                DebugGridSquares.push_back(Square);
            }
        }

        //LOG("Debug: Drawing 8x8 grid of %dx%d squares starting at (%f, %f).", SquareSize, SquareSize, GRID_ABS_OFFSET_X, GRID_ABS_OFFSET_Y);
    }

    void Board::HandleMouseHover(const sf::Vector2f& MousePos)
    {
        bool pieceWasHovered = false;

        for (auto it = Pieces.rbegin(); it != Pieces.rend(); ++it)
        {
            shared<ChessPiece> Piece = *it;
            if (Piece && !Piece->IsPendingDestroy())
            {
                if (!pieceWasHovered && Piece->IsPointInBounds(MousePos))
                {
                    Piece->SetHovered(true);
                    pieceWasHovered = true;
                }
                else
                {
                    Piece->SetHovered(false);
                }
            }
        }
    }

    void Board::HandleMouseClick(const sf::Vector2f& MousePos)
    {
        shared<ChessPiece> ClickedPiece = nullptr;
        for (auto it = Pieces.rbegin(); it != Pieces.rend(); ++it)
        {
            shared<ChessPiece> Piece = *it;
            if (Piece && !Piece->IsPendingDestroy() && Piece->IsPointInBounds(MousePos))
            {
                ClickedPiece = Piece;
                break;
            }
        }

        shared<ChessPiece> CurrentSelectedPiece = SelectedPiece.lock();

        if (ClickedPiece)
        {
            std::string pieceName = GetPieceName(ClickedPiece->GetPieceType());
            std::string algebraicPos = GridToAlgebraic(ClickedPiece->GetGridPosition());

            if (ClickedPiece == CurrentSelectedPiece)
            {
                // Case 1: Clicked the piece that is already selected -> Deselect it.
                ClickedPiece->SetSelected(false);
                SelectedPiece.reset();
                
                LOG("Deselected %s at (%s)", pieceName.c_str(), algebraicPos.c_str());
            }
            else
            {
                // Case 2: Clicked a new piece -> Select new.

                if (CurrentSelectedPiece)
                {
                    CurrentSelectedPiece->SetSelected(false);
                }

                ClickedPiece->SetSelected(true);
                SelectedPiece = ClickedPiece;

                LOG("Selected %s: %s at (%s)",
                    (ClickedPiece->GetColor() == EChessColor::White ? "White" : "Black"),
                    pieceName.c_str(), algebraicPos.c_str());
            }
        }
        else if (CurrentSelectedPiece)
        {
            // Case 3: Clicked empty space while a piece was selected -> Deselect it.
            CurrentSelectedPiece->SetSelected(false);

            std::string pieceName = GetPieceName(CurrentSelectedPiece->GetPieceType());
            std::string algebraicPos = GridToAlgebraic(CurrentSelectedPiece->GetGridPosition());

            SelectedPiece.reset();

            LOG("Deselected %s at (%s) (clicked empty space)", pieceName.c_str(), algebraicPos.c_str());
        }
    }
}