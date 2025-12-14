#pragma once
#include "SFML/Graphics.hpp"
#include "Framework/Object.h"
#include "Framework/Core.h"

namespace we
{
	class Widget : public Object
	{
	public:
		void NativeDraw(sf::RenderWindow& Window);
		virtual bool HandleEvent(const optional<sf::Event> Event);
		void SetWidgetLocation(const sf::Vector2f& NewLocation);
		void SetWidgetRotation(const sf::Angle& NewRotation);
		sf::Vector2f GetWidgetLocation() const { return WidgetTransform.getPosition(); }
		sf::Angle GetWidgetRotation() const { return WidgetTransform.getRotation(); }
		void SetVisability(bool NewVisability);
		bool GetVisability() const { return bIsVisable; }

	protected:
		Widget();
	private:
		virtual void Draw(sf::RenderWindow& Window);
		virtual void LocationUpdate(const sf::Vector2f& NewLocation);
		virtual void RotationUpdate(const sf::Angle& NewRotation);
		sf::Transformable WidgetTransform;
		bool bIsVisable;
	};
}