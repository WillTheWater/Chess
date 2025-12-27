#pragma once
#include"UI/HUD.h"
#include"UI/TextBlock.h"
#include "UI/ProgressBar.h"
#include "UI/Image.h"
#include "UI/Button.h"

namespace we
{
	class Actor;

	class Menu : public HUD
	{
	public:
		Menu();

		virtual void Render(Renderer& GameRenderer) override;
		virtual void Tick(float DeltaTime) override;
		virtual bool HandleEvent(const optional<sf::Event> Event) override;

		void SetVisibility(bool NewVisibility);
		void Checkmated();
		void Stalemated();
		void Drawn();
		Delegate<> OnRestartButtonClicked;
		Delegate<> OnQuitButtonClicked;

	private:
		virtual void Initialize(Renderer& GameRenderer) override;
		void StartButtonClicked();
		void QuitButtonClicked();
		void InitializeButtons(const sf::Vector2u& ViewportSize);
		void InitializeText(const sf::Vector2u& ViewportSize);
		Button RestartButton;
		Button QuitButton;
		TextBlock RestartButtonText;
		TextBlock CheckmateText;
		TextBlock StalemateText;
		TextBlock DrawText;
		TextBlock FlavorText;
		sf::Color TextColor{ 192, 35, 10, 255 };
		sf::Color OutlineColor{ 0, 0, 0, 255 };
	};
}