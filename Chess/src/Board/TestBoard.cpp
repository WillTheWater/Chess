#include "Board/TestBoard.h"
#include "Framework/Renderer.h"
#include "Framework/World.h"
#include <sstream>

namespace we
{
	TestBoard::TestBoard(World* OwningWorld, const std::string& TexturePath)
		: Actor{OwningWorld, TexturePath}
        , Pieces{}
	{
	}

	void TestBoard::BeginPlay()
	{
        SetActorLocation(sf::Vector2f{ float(GetWindowSize().x) / 2.0f, float(GetWindowSize().y) / 2.0f });
        InitializeBoard();
	}

	void TestBoard::Render(Renderer& GameRenderer)
	{
		Actor::Render(GameRenderer);
		
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

    void TestBoard::InitializeBoard()
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

    void TestBoard::ClearBoard()
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

    EChessColor TestBoard::GetPieceColor(int value)
    {
        return (value > 0) ? EChessColor::White : EChessColor::Black;
    }

    EChessPieceType TestBoard::GetPieceType(int value)
    {
        int id = std::abs(value) - 1;
        return static_cast<EChessPieceType>(id);
    }

    void TestBoard::SpawnPiece(EChessPieceType type, EChessColor color, const sf::Vector2i& pos)
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
    sf::Vector2i TestBoard::WorldToGrid(const sf::Vector2f& WorldPos)
    {
        float RelativeX = WorldPos.x - GRID_ABS_OFFSET_X;
        float RelativeY = WorldPos.y - GRID_ABS_OFFSET_Y;

        int GridX = static_cast<int>(std::floor(RelativeX / SquareSize));
        int GridY = static_cast<int>(std::floor(RelativeY / SquareSize));

        GridX = std::clamp(GridX, 0, GridSize - 1);
        GridY = std::clamp(GridY, 0, GridSize - 1);

        return { GridX, GridY };
    }

    sf::Vector2f TestBoard::GridToWorld(const sf::Vector2i& GridPos)
    {
        float PixelX = GRID_ABS_OFFSET_X + GridPos.x * SquareSize;
        float PixelY = GRID_ABS_OFFSET_Y + GridPos.y * SquareSize;

        return { PixelX, PixelY };
    }

    sf::Vector2f TestBoard::GridToCenterSquare(const sf::Vector2i& GridPos)
    {
        return {
        GRID_ABS_OFFSET_X + GridPos.x * SquareSize + SquareSize * 0.5f,
        GRID_ABS_OFFSET_Y + GridPos.y * SquareSize + SquareSize * 0.5f
        };
    }

    std::string TestBoard::GridToAlgebraic(const sf::Vector2i& GridPos)
    {
        if (GridPos.x < 0 || GridPos.x >= 8 || GridPos.y < 0 || GridPos.y >= 8) { return "Invalid"; }

        char File = 'a' + GridPos.x;
        char Rank = '8' - GridPos.y;

        std::stringstream ss;
        ss << File << Rank;
        return ss.str();
    }
}