#include "Widgets/GameHUD.h"

namespace we
{
	GameHUD::GameHUD()
		: PlayerTurn{" 's Move"}
	{
	}
	void GameHUD::Draw(sf::RenderWindow& Window)
	{
		PlayerTurn.NativeDraw(Window);
	}
	void GameHUD::Tick(float DeltaTime)
	{
		int framerate = int(1 / DeltaTime);
		string frtxt = "Framerate: " + std::to_string(framerate);
		PlayerTurn = frtxt;
	}
}