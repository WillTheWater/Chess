#include "GameFramework/Game.h"
#include "GameFramework/Play.h"
#include "Framework/Assetmanager.h"
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
		weak<Play> PlayChess = LoadWorld<Play>();
	}
}