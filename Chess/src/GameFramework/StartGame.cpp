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
			ChessBoard.lock()->OnCheckmate.Bind(GetObject(), &StartGame::Checkmate);
			ChessBoard.lock()->OnStalemate.Bind(GetObject(), &StartGame::Stalemate);
			ChessBoard.lock()->OnDraw.Bind(GetObject(), &StartGame::Draw);
		}
	}

	void StartGame::TickLevel(float DeltaTime)
	{
	}

	void StartGame::Checkmate()
	{
		OnCheckmate.Broadcast();
	}

	void StartGame::Stalemate()
	{
		OnStalemate.Broadcast();
	}

	void StartGame::Draw()
	{
		OnDraw.Broadcast();
	}

	void StartGame::SpawnBoard()
	{
		ChessBoard = GetWorld()->SpawnActor<Board>();
	}
}