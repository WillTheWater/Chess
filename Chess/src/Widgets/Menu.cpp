#include "Widgets/Menu.h"

namespace we
{
	Menu::Menu()
		: RestartButton{ "button.png" }
		, QuitButton{ "closebutton.png" }
		, FullScreenButton{"fullscreenbutton.png"}
		, MinimizeButton{"minimizebutton.png"}
		, RestartButtonText{ "Restart" }
		, CheckmateText{"Checkmate"}
		, StalemateText{"Stalemate"}
		, DrawnText{"Draw"}
		, FlavorText{"Your deeds of valor will be forgotten"}
		, WinnerText{"Winner"}
		, PromotionMenu{}
	{
		RestartButton.SetVisibility(false);
		RestartButtonText.SetVisibility(false);
		CheckmateText.SetVisibility(false);
		StalemateText.SetVisibility(false);
		DrawnText.SetVisibility(false);
		FlavorText.SetVisibility(false);
		WinnerText.SetVisibility(false);
		PromotionMenu.SetVisibility(false);
	}

	void Menu::Render(Renderer& GameRenderer)
	{
		RestartButton.NativeRender(GameRenderer);
		RestartButtonText.NativeRender(GameRenderer);
		QuitButton.NativeRender(GameRenderer);
		FullScreenButton.NativeRender(GameRenderer);
		MinimizeButton.NativeRender(GameRenderer);
		CheckmateText.NativeRender(GameRenderer);
		StalemateText.NativeRender(GameRenderer);
		DrawnText.NativeRender(GameRenderer);
		FlavorText.NativeRender(GameRenderer);
		WinnerText.NativeRender(GameRenderer);

		PromotionMenu.NativeRender(GameRenderer);
		PromotionMenu.DrawChoices(GameRenderer);
	}

	void Menu::Tick(float DeltaTime)
	{
	}

	bool Menu::HandleEvent(const optional<sf::Event> Event, Renderer& GameRenderer)
	{
		return RestartButton.HandleEvent(Event, GameRenderer)
			|| QuitButton.HandleEvent(Event, GameRenderer)
			|| FullScreenButton.HandleEvent(Event, GameRenderer)
			|| MinimizeButton.HandleEvent(Event, GameRenderer)
			|| PromotionMenu.QueenSelected.HandleEvent(Event, GameRenderer)
			|| PromotionMenu.RookSelected.HandleEvent(Event, GameRenderer)
			|| PromotionMenu.BishopSelected.HandleEvent(Event, GameRenderer)
			|| PromotionMenu.KnightSelected.HandleEvent(Event, GameRenderer)
			|| HUD::HandleEvent(Event, GameRenderer);
	}

	void Menu::Initialize(Renderer& GameRenderer)
	{
		const auto& Viewport = GameRenderer.GetViewportSize();
		InitializeButtons(Viewport);
		InitializeText(Viewport);
		PromotionMenu.Init();
		RestartButton.OnButtonClicked.Bind(GetWeakObject(), &Menu::RestartButtonClicked);
		QuitButton.OnButtonClicked.Bind(GetWeakObject(), &Menu::QuitButtonClicked);
		FullScreenButton.OnButtonClicked.Bind(GetWeakObject(), &Menu::FullScreenButtonClicked);
		MinimizeButton.OnButtonClicked.Bind(GetWeakObject(), &Menu::MinimizeButtonClicked);
		PromotionMenu.QueenSelected.OnButtonClicked.Bind(GetWeakObject(), &Menu::QueenButtonClicked);
		PromotionMenu.RookSelected.OnButtonClicked.Bind(GetWeakObject(), &Menu::RookButtonClicked);
		PromotionMenu.BishopSelected.OnButtonClicked.Bind(GetWeakObject(), &Menu::BishopButtonClicked);
		PromotionMenu.KnightSelected.OnButtonClicked.Bind(GetWeakObject(), &Menu::KnightButtonClicked);
	}

	void Menu::RestartButtonClicked()
	{
		OnRestartButtonClicked.Broadcast();
	}

	void Menu::QuitButtonClicked()
	{
		OnQuitButtonClicked.Broadcast();
	}

	void Menu::FullScreenButtonClicked()
	{
		OnFullScreenButtonClicked.Broadcast();
	}

	void Menu::MinimizeButtonClicked()
	{
		OnMinimizeButtonClicked.Broadcast();
	}

	void Menu::QueenButtonClicked()
	{
		LOG("Queen Selected");
		OnQueenSelected.Broadcast();
		PromotionVisibility(false);
	}

	void Menu::RookButtonClicked()
	{
		LOG("Rook Selected");
		OnRookSelected.Broadcast();
		PromotionVisibility(false);
	}

	void Menu::BishopButtonClicked()
	{
		LOG("Bishop Selected");
		OnBishopSelected.Broadcast();
		PromotionVisibility(false);
	}

	void Menu::KnightButtonClicked()
	{
		LOG("Knight Selected");
		OnKnightSelected.Broadcast();
		PromotionVisibility(false);
	}

	void Menu::InitializeButtons(const sf::Vector2u& ViewportSize)
	{
		RestartButton.CenterOrigin();
		RestartButtonText.CenterOrigin();
		RestartButtonText.SetColor(sf::Color::Black);
		RestartButtonText.SetOutline(sf::Color::Black, 1.f);
		RestartButton.SetWidgetPosition({ ViewportSize.x / 2.f, ViewportSize.y / 2.f + 204.f });
		RestartButtonText.SetWidgetPosition(RestartButton.GetWidgetPosition());
		QuitButton.CenterOrigin();
		FullScreenButton.CenterOrigin();
		MinimizeButton.CenterOrigin();
		QuitButton.SetWidgetPosition({ ViewportSize.x - 40.f, 70.f });
		FullScreenButton.SetWidgetPosition({ ViewportSize.x - 114.f, 70.f });
		MinimizeButton.SetWidgetPosition({ ViewportSize.x - 188.f, 70.f });
		PromotionMenu.SetWidgetPosition({ ViewportSize.x - 124.f, ViewportSize.y / 2.f });
	}

	void Menu::InitializeText(const sf::Vector2u& ViewportSize)
	{
		CheckmateText.SetFontSize(80);
		StalemateText.SetFontSize(80);
		DrawnText.SetFontSize(80);
		CheckmateText.CenterOrigin();
		StalemateText.CenterOrigin();
		DrawnText.CenterOrigin();
		CheckmateText.SetColor(TextColor);
		StalemateText.SetColor(TextColor);
		DrawnText.SetColor(TextColor);
		CheckmateText.SetOutline(OutlineColor, 3.f);
		StalemateText.SetOutline(OutlineColor, 3.f);
		DrawnText.SetOutline(OutlineColor, 3.f);
		CheckmateText.SetWidgetPosition({ ViewportSize.x / 2.f, 400.f });
		StalemateText.SetWidgetPosition({ ViewportSize.x / 2.f, 200.f });
		DrawnText.SetWidgetPosition({ ViewportSize.x / 2.f, 200.f });

		FlavorText.CenterOrigin();
		FlavorText.SetColor(TextColor);
		FlavorText.SetOutline(OutlineColor, 2.f);
		FlavorText.SetWidgetPosition({ ViewportSize.x / 2.f, 500.f });

		WinnerText.SetFontSize(80);
		WinnerText.CenterOrigin();
		WinnerText.SetColor(TextColor);
		WinnerText.SetOutline(OutlineColor, 3.f);
		WinnerText.SetWidgetPosition({ ViewportSize.x / 2.f, 300.f });
	}

	void Menu::SetWinnerText(EPlayerTurn Winner)
	{
		switch (Winner)
		{
			case EPlayerTurn::White:
			{
				WinnerText.SetText("White won by");
				break;
			}
			case EPlayerTurn::Black:
			{
				WinnerText.SetText("Black won by");
				break;
			}
		}
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
		WinnerText.CenterOrigin();
		WinnerText.SetVisibility(true);
	}

	void Menu::Stalemated()
	{
		StalemateText.SetVisibility(true);
	}

	void Menu::Drawn()
	{
		DrawnText.SetVisibility(true);
	}

	void Menu::PromotionVisibility(bool Visibility)
	{
		PromotionMenu.SetVisibility(Visibility);
		PromotionMenu.QueenSelected.SetVisibility(Visibility);
		PromotionMenu.RookSelected.SetVisibility(Visibility);
		PromotionMenu.BishopSelected.SetVisibility(Visibility);
		PromotionMenu.KnightSelected.SetVisibility(Visibility);
	}
}