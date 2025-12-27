#include "Widgets/Menu.h"

namespace we
{
	Menu::Menu()
		: RestartButton{ "/button.png" }
		, RestartButtonText{ "Restart" }
		, QuitButton{ "closebutton.png" }
	{
		RestartButton.SetVisibility(false);
		RestartButtonText.SetVisibility(false);
	}

	void Menu::Render(Renderer& GameRenderer)
	{
		RestartButton.NativeRender(GameRenderer);
		RestartButtonText.NativeRender(GameRenderer);
		QuitButton.NativeRender(GameRenderer);
	}

	void Menu::Tick(float DeltaTime)
	{
	}

	bool Menu::HandleEvent(const optional<sf::Event> Event)
	{
		return RestartButton.HandleEvent(Event) || QuitButton.HandleEvent(Event) || HUD::HandleEvent(Event);
	}

	void Menu::Initialize(Renderer& GameRenderer)
	{
		InitializeButtons(GameRenderer.GetViewportSize());
		RestartButton.OnButtonClicked.Bind(GetObject(), &Menu::StartButtonClicked);
		QuitButton.OnButtonClicked.Bind(GetObject(), &Menu::QuitButtonClicked);
	}

	void Menu::StartButtonClicked()
	{
		OnRestartButtonClicked.Broadcast();
	}

	void Menu::QuitButtonClicked()
	{
		OnQuitButtonClicked.Broadcast();
	}

	void Menu::InitializeButtons(const sf::Vector2u& ViewportSize)
	{
		RestartButton.CenterOrigin();
		RestartButtonText.CenterOrigin();
		RestartButtonText.SetColor(sf::Color::Black);
		QuitButton.CenterOrigin();
		RestartButton.SetWidgetPosition({ ViewportSize.x / 2.f, ViewportSize.y / 2.f + 204.f });
		RestartButtonText.SetWidgetPosition(RestartButton.GetWidgetPosition());
		QuitButton.SetWidgetPosition({ ViewportSize.x - 40.f, 40.f });
	}

	void Menu::SetVisibility(bool NewVisibility)
	{
		RestartButton.SetVisibility(NewVisibility);
		RestartButtonText.SetVisibility(NewVisibility);
	}
}