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
		virtual void Tick(float DeltaTime) override;

	private:
		TextWidget PlayerTurn;
	};
}