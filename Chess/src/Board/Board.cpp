#include "Board/Board.h"
#include "Framework/World.h"

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

    static bool bLeftMouseButtonPressedLastFrame = false;

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
            if (ClickedPiece == CurrentSelectedPiece)
            {
                ClickedPiece->SetSelected(false);
                SelectedPiece.reset();
                LOG("Deselected piece at (%d, %d)", ClickedPiece->GetGridPosition().x, ClickedPiece->GetGridPosition().y);
            }
            else
            {
                if (CurrentSelectedPiece)
                {
                    CurrentSelectedPiece->SetSelected(false);
                }

                ClickedPiece->SetSelected(true);
                SelectedPiece = ClickedPiece;
                LOG("Selected piece: %s at (%d, %d)",
                    (ClickedPiece->GetColor() == EChessColor::White ? "White" : "Black"),
                    ClickedPiece->GetGridPosition().x, ClickedPiece->GetGridPosition().y);
            }
        }
        else if (CurrentSelectedPiece)
        {
            CurrentSelectedPiece->SetSelected(false);
            SelectedPiece.reset();
            LOG("Deselected piece (clicked empty space)");
        }
    }
}