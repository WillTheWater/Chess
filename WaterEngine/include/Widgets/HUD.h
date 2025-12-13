#pragma once

#include "SFML/Graphics.hpp"
#include "Framework/Object.h"
#include "Framework/Core.h"

namespace we
{
	class HUD : public Object
	{
	public:
		virtual void Draw(sf::RenderWindow& Window) = 0;
		void NativeInitialize(const sf::RenderWindow& Window);

		bool IsInitialized() const { return bIsInitialized; }
		virtual bool HandleEvent(const optional<sf::Event> Event);

	protected:
		HUD();

	private:
		virtual void Initialize(const sf::RenderWindow& Window);
		bool bIsInitialized;
	};
}