#pragma once 
#include "Widgets/HUD.h"
#include "Widgets/TextWidget.h"

namespace we
{
	class GameHUD : public HUD
	{
	public: 
		GameHUD();

		virtual void Draw(sf::RenderWindow& Window) override;

	private:
		TextWidget PlayerTurn;
	};
}