#pragma once
#include"UI/HUD.h"
#include"UI/TextBlock.h"
#include "UI/ProgressBar.h"
#include "UI/Image.h"
#include "UI/Button.h"
#include "Board/Board.h"
#include "Widgets/PromotionSelection.h"

namespace we
{
	class Actor;

	class Menu : public HUD
	{
	public:
		Menu();

		virtual void Render(Renderer& GameRenderer) override;
		virtual void Tick(float DeltaTime) override;
		virtual bool HandleEvent(const optional<sf::Event> Event, Renderer& GameRenderer) override;

		void SetVisibility(bool NewVisibility);
		void SetWinnerText(EPlayerTurn Winner);
		void Checkmated();
		void Stalemated();
		void Drawn();
		void PromotionVisibility(EPlayerTurn Color, bool Visibility);
		void PromotionVisibility(bool Visibility);
		Delegate<> OnRestartButtonClicked;
		Delegate<> OnQuitButtonClicked;
		Delegate<> OnFullScreenButtonClicked;
		Delegate<> OnMinimizeButtonClicked;
		Delegate<> OnQueenSelected;
		Delegate<> OnRookSelected;
		Delegate<> OnBishopSelected;
		Delegate<> OnKnightSelected;

	private:
		virtual void Initialize(Renderer& GameRenderer) override;
		void RestartButtonClicked();
		void QuitButtonClicked();
		void FullScreenButtonClicked();
		void MinimizeButtonClicked();
		void QueenButtonClicked();
		void RookButtonClicked();
		void BishopButtonClicked();
		void KnightButtonClicked();
		void InitializeButtons(const sf::Vector2u& ViewportSize);
		void InitializeText(const sf::Vector2u& ViewportSize);
		Button RestartButton;
		Button QuitButton;
		Button FullScreenButton;
		Button MinimizeButton;
		TextBlock RestartButtonText;
		TextBlock CheckmateText;
		TextBlock StalemateText;
		TextBlock DrawnText;
		TextBlock FlavorText;
		TextBlock WinnerText;
		sf::Color TextColor{ 192, 35, 10, 255 };
		sf::Color OutlineColor{ 0, 0, 0, 255 };
		PromotionSelector PromotionMenu;
	};
}