#include "GameFramework/Game.h"
#include "Framework/World.h"
#include "Framework/Actor.h"
#include "Framework/Assetmanager.h"
#include "Board/Board.h"
#include "Widgets/GameHUD.h"
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
		ChessGameHUD = ChessWorld.lock()->SpawnHUD<GameHUD>();
	}
	void Game::Tick(float DeltaTime)
	{
	}
}