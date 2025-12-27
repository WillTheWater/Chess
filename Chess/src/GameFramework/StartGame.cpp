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
		}
	}

	void StartGame::TickLevel(float DeltaTime)
	{
	}

	void StartGame::Checkmate()
	{
		OnCheckmate.Broadcast();
		LOG("Chackmate Delegate Passed")
	}

	void StartGame::EndLevel()
	{
	}

	void StartGame::SpawnBoard()
	{
		ChessBoard = GetWorld()->SpawnActor<Board>();
	}
}