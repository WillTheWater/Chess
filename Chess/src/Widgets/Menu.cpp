#include "Widgets/Menu.h"

namespace we
{
	Menu::Menu()
		: RestartButton{ "button.png" }
		, QuitButton{ "closebutton.png" }
		, RestartButtonText{ "Restart" }
		, CheckmateText{"Checkmate"}
		, StalemateText{"Stalemate"}
		, FlavorText{"Your deeds of valor will be forgotten"}
	{
		RestartButton.SetVisibility(false);
		RestartButtonText.SetVisibility(false);
		CheckmateText.SetVisibility(false);
		StalemateText.SetVisibility(false);
		FlavorText.SetVisibility(false);
	}

	void Menu::Render(Renderer& GameRenderer)
	{
		RestartButton.NativeRender(GameRenderer);
		RestartButtonText.NativeRender(GameRenderer);
		QuitButton.NativeRender(GameRenderer);
		CheckmateText.NativeRender(GameRenderer);
		StalemateText.NativeRender(GameRenderer);
		FlavorText.NativeRender(GameRenderer);
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
		const auto& Viewport = GameRenderer.GetViewportSize();
		InitializeButtons(Viewport);
		InitializeText(Viewport);
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

	void Menu::InitializeText(const sf::Vector2u& ViewportSize)
	{
		CheckmateText.SetFontSize(80);
		StalemateText.SetFontSize(80);
		CheckmateText.CenterOrigin();
		StalemateText.CenterOrigin();
		CheckmateText.SetColor(TextColor);
		StalemateText.SetColor(TextColor);
		CheckmateText.SetOutline(OutlineColor, 3.f);
		StalemateText.SetOutline(OutlineColor, 3.f);
		CheckmateText.SetWidgetPosition({ ViewportSize.x / 2.f, 400.f });
		StalemateText.SetWidgetPosition({ ViewportSize.x / 2.f, 200.f });

		FlavorText.CenterOrigin();
		FlavorText.SetColor(TextColor);
		FlavorText.SetOutline(OutlineColor, 2.f);
		FlavorText.SetWidgetPosition({ ViewportSize.x / 2.f, 500.f });
	}

	void Menu::SetVisibility(bool NewVisibility)
	{
		RestartButton.SetVisibility(NewVisibility);
		RestartButtonText.SetVisibility(NewVisibility);
	}

	void Menu::Checkmated()
	{
		CheckmateText.SetVisibility(true);
		FlavorText.SetVisibility(true);
	}

	void Menu::Stalemated()
	{
		StalemateText.SetVisibility(true);
	}
}