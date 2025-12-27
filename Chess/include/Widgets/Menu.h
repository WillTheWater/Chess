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
		Delegate<> OnRestartButtonClicked;
		Delegate<> OnQuitButtonClicked;

	private:
		virtual void Initialize(Renderer& GameRenderer) override;
		Button RestartButton;
		TextBlock RestartButtonText;
		Button QuitButton;
		void StartButtonClicked();
		void QuitButtonClicked();
		void InitializeButtons(const sf::Vector2u& ViewportSize);
	};
}