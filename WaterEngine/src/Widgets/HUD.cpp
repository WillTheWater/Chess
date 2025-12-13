#include "Widgets/HUD.h"

namespace we
{
	HUD::HUD()
		: bIsInitialized{ false }
	{
	}
	void HUD::Initialize(sf::RenderWindow& Window)
	{
	}
	void HUD::NativeInitialize(sf::RenderWindow& Window)
	{
		if (!bIsInitialized)
		{
			bIsInitialized = true;
			Initialize(Window);
		}
	}
	bool HUD::HandleEvent(const sf::Event& Event)
	{
		return false;
	}
}