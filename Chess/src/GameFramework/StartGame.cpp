#include "GameFramework/StartGame.h"
#include "Framework/World.h"
#include "Board/Board.h"

namespace we
{
	StartGame::StartGame(World* World)
		: Level{World}
	{
	}

	void StartGame::BeginLevel()
	{
		SpawnBoard();
	}

	void StartGame::TickLevel(float DeltaTime)
	{
	}

	void StartGame::EndLevel()
	{
	}

	void StartGame::SpawnBoard()
	{
		weak<Board> NewFighter = GetWorld()->SpawnActor<Board>();
	}
}