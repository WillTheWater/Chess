#include "Widgets/HUD.h"

namespace we
{
	HUD::HUD()
		: bIsInitialized{ false }
	{
	}
	void HUD::Initialize(const sf::RenderWindow& Window)
	{
	}
	void HUD::NativeInitialize(const sf::RenderWindow& Window)
	{
		if (!bIsInitialized)
		{
			bIsInitialized = true;
			Initialize(Window);
		}
	}

	void HUD::Tick(float DeltaTime)
	{

	}

	bool HUD::HandleEvent(const optional<sf::Event> Event)
	{
		return false;
	}
}