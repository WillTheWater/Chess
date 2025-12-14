#include "Widgets/GameHUD.h"

namespace we
{
	GameHUD::GameHUD()
		: PlayerTurn{"White's Move"}
	{
	}

	void GameHUD::Draw(sf::RenderWindow& Window)
	{
		PlayerTurn.NativeDraw(Window);
	}
}