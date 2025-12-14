#pragma once
#include "Framework/Core.h"
#include "Widgets/Widget.h"

namespace we
{
	class TextWidget : public Widget
	{
	public:
		TextWidget(const string& Text, const string& FontPath = "font/diablo.ttf", unsigned int FontSize = 24);

		void SetText(const string& InText);
		void SetFontSize(unsigned int InSize);

	private:
		virtual void LocationUpdate(const sf::Vector2f& NewLocation) override;
		virtual void RotationUpdate(const sf::Angle& NewRotation) override;
		virtual void Draw(sf::RenderWindow& Window) override;
		shared<sf::Font> WidgetFont;
		sf::Text WidgetText;
	};
}