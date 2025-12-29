#include "GameFramework/StartGame.h"
#include "Framework/World.h"
#include "Board/Board.h"

namespace we
{
	StartGame::StartGame(World* World)
		: Level{World}
		, ChessBoard{}
	{
	}

	void StartGame::BeginLevel()
	{
		SpawnBoard();
		if (!ChessBoard.expired())
		{
			ChessBoard.lock()->OnCheckmate.Bind(GetWeakObject(), &StartGame::Checkmate);
			ChessBoard.lock()->OnStalemate.Bind(GetWeakObject(), &StartGame::Stalemate);
			ChessBoard.lock()->OnDraw.Bind(GetWeakObject(), &StartGame::Draw);
			ChessBoard.lock()->OnPromotionRequested.Bind(GetWeakObject(), &StartGame::Promotion);
		}
	}

	void StartGame::TickLevel(float DeltaTime)
	{
	}

	void StartGame::Checkmate(EPlayerTurn Winner)
	{
		OnCheckmate.Broadcast(Winner);
	}

	void StartGame::Stalemate()
	{
		OnStalemate.Broadcast();
	}

	void StartGame::Draw()
	{
		OnDraw.Broadcast();
	}

	void StartGame::Promotion(sf::Vector2i PromotionSquare)
	{
		OnPromotionRequested.Broadcast(PromotionSquare);
	}

	void StartGame::SpawnBoard()
	{
		ChessBoard = GetWorld()->SpawnActor<Board>();
	}
}