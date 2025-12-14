#include "Widgets/TextWidget.h"
#include "Framework/AssetManager.h"

namespace we
{
	TextWidget::TextWidget(const string& Text, const string& FontPath, unsigned int FontSize)
		: WidgetFont{AssetManager::GetAssetManager().LoadFont(FontPath)}
		, WidgetText{ *(WidgetFont.get()), Text, FontSize }
	{

	}

	void TextWidget::SetText(const string& InText)
	{
		WidgetText.setString(InText);
	}

	void TextWidget::SetFontSize(unsigned int InSize)
	{
		WidgetText.setCharacterSize(InSize);
	}

	void TextWidget::LocationUpdate(const sf::Vector2f& NewLocation)
	{
		WidgetText.setPosition(NewLocation);
	}

	void TextWidget::RotationUpdate(const sf::Angle& NewRotation)
	{
		WidgetText.setRotation(NewRotation);
	}

	void TextWidget::Draw(sf::RenderWindow& Window)
	{
		Window.draw(WidgetText);
	}
}