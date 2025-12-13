#pragma once

#include "SFML/Graphics.hpp"
#include "Framework/Object.h"

namespace we
{
	class HUD : public Object
	{
	public:
		virtual void Render(sf::RenderWindow& Window) = 0;
		void NativeInitialize(sf::RenderWindow& Window);

		bool IsInitialized() const { return bIsInitialized; }
		virtual bool HandleEvent(const sf::Event& Event);

	protected:
		HUD();

	private:
		virtual void Initialize(sf::RenderWindow& Window);
		bool bIsInitialized;
	};
}