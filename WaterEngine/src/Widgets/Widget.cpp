#include "Widgets/Widget.h"

namespace we
{
	Widget::Widget()
		: bIsVisable{ true }
		, WidgetTransform{}
	{
	}

	void Widget::Draw(sf::RenderWindow& Window)
	{
	}

	void Widget::LocationUpdate(const sf::Vector2f& NewLocation)
	{
	}

	void Widget::RotationUpdate(const sf::Angle& NewRotation)
	{
	}

	bool Widget::HandleEvent(const optional<sf::Event> Event)
	{
		return false;
	}

	void Widget::SetWidgetLocation(const sf::Vector2f& NewLocation)
	{
		WidgetTransform.setPosition(NewLocation);
		LocationUpdate(NewLocation);
	}

	void Widget::SetWidgetRotation(const sf::Angle& NewRotation)
	{
		WidgetTransform.setRotation(NewRotation);
		RotationUpdate(NewRotation);
	}

	void Widget::SetVisability(bool NewVisability)
	{
		bIsVisable = NewVisability;
	}

	void Widget::NativeDraw(sf::RenderWindow& Window)
	{
		if (bIsVisable)
		{
			Draw(Window);
		}
	}
}