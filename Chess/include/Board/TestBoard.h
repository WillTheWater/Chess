#pragma once
#include "Framework/Actor.h"
#include "Board/ChessPieces.h"

namespace we
{
    struct MoveResult
    {
        bool bValid = false;
        sf::Vector2i From;
        sf::Vector2i To;
        shared<ChessPiece> CapturedPiece;
        bool bPawnPromoted = false;
        EChessPieceType PromotionType;
        bool bCastling = false;
        sf::Vector2i RookFrom, RookTo;

        bool bEnPassant = false;
        bool bIsCheck = false;
        bool bIsCheckmate = false;
        bool bIsStalemate = false;
        bool bIsDraw = false;
    };

    enum class EPlayerTurn
    {
        White,
        Black
    };

	class TestBoard : public Actor
	{
    public:
        TestBoard(World* OwningWorld, const std::string& TexturePath = "/board.png");

        virtual void BeginPlay() override;
        virtual void Render(class Renderer& GameRenderer) override;

    private:
        List<shared<ChessPiece>> Pieces;
	};
}