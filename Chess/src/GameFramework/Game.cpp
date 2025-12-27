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
		: Application{1920, 1080, "Chess", sf::Style::None}
	{
		AssetManager::Get().SetAssetRootDirctory(GetAssetDirectory());
		weak<Play> PlayChess = LoadWorld<Play>();
	}
}