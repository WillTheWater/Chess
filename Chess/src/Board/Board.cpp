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

        for (int y = 0; y < GridSize; ++y) // y is the row index (0 to 7)
        {
            for (int x = 0; x < GridSize; ++x) // x is the column index (0 to 7)
            {
                int pieceValue = InitialBoard[y][x];

                if (pieceValue != 0) // Only spawn a piece if the square is not empty (value 0)
                {
                    EChessColor color = (pieceValue > 0) ? EChessColor::White : EChessColor::Black;

                    // FIX: The InitialBoard values are (EChessPieceType + 1).
                    // We must subtract 1 from the absolute value to get the correct EChessPieceType index (0-5).
                    EChessPieceType type = static_cast<EChessPieceType>(std::abs(pieceValue) - 1);

                    // Passing the explicit std::string object (PieceTexturePath)
                    weak<ChessPiece> newPieceWeak = GetWorld()->SpawnActor<ChessPiece>(
                        type,
                        color,
                        PieceTexturePath
                    );

                    if (shared<ChessPiece> newPiece = newPieceWeak.lock())
                    {
                        // Pass the new sf::Vector2i for grid position
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
        // Define the size for each square
        sf::Vector2f SquareDimensions{ (float)SquareSize, (float)SquareSize };

        DebugGridSquares.clear();

        for (int y = 0; y < GridSize; ++y)
        {
            for (int x = 0; x < GridSize; ++x)
            {
                sf::RectangleShape Square;
                Square.setSize(SquareDimensions);

                // Calculate position: absolute offset + grid position * square size
                float PosX = GRID_ABS_OFFSET_X + x * SquareSize;
                float PosY = GRID_ABS_OFFSET_Y + y * SquareSize;

                Square.setPosition(sf::Vector2f{ PosX, PosY });

                // Using the high-visibility colors
                Square.setFillColor(sf::Color(0, 255, 0, 100)); // Bright Green, semi-transparent fill
                Square.setOutlineColor(sf::Color::Blue);       // Blue outline
                Square.setOutlineThickness(-2.0f);             // Thinner, inside border

                DebugGridSquares.push_back(Square);
            }
        }

        LOG("Debug: Drawing 8x8 grid of %dx%d squares starting at (%f, %f).",
            SquareSize, SquareSize, GRID_ABS_OFFSET_X, GRID_ABS_OFFSET_Y);
    }
}