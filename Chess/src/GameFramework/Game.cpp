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
		AssetManager::Get().SetAssetRootDirctory(GetAssetDirectory());
		weak<MainMenu> NewWorld = LoadWorld<MainMenu>();
	}
}