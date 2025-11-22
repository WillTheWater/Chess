#include "GameFramework/Game.h"
#include "Framework/World.h"
#include "Framework/Actor.h"
#include "Framework/Assetmanager.h"
#include "Board/Board.h"
#include "config.h"

we::Application* GetApplication()
{
	return new we::Game{};
}

namespace we
{
	Game::Game()
		: Application{1280, 720, "Chess", sf::Style::Default}
	{
		AssetManager::GetAssetManager().SetAssetRootDirctory(GetAssetDirectory());
		SetWindowIcon("/icon.png");
		SetCustomCursor();
		weak<World> ChessWorld = LoadWorld<World>();
		ChessBoard = ChessWorld.lock()->SpawnActor<Board>();
		ChessBoard.lock()->SetActorLocation(sf::Vector2f{ 1280 / 2, 720 / 2 });
	}
	void Game::Tick(float DeltaTime)
	{
	}
}