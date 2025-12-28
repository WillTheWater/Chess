#pragma once
#include "Framework/World.h"
#include "Framework/Core.h"
#include "Board/Board.h"
#include "Widgets/Menu.h"

namespace we
{
	class StartGame;

	class Play : public World
	{
	public:
		Play(Application* OwningApp);

	private:
		virtual void BeginPlay() override;
		virtual void Tick(float DeltaTime) override;
		virtual void InitLevels() override;
		void Checkmate(EPlayerTurn Winner);
		void Stalemate();
		void Draw();
		void RestartGame();
		void QuitGame();
		void ToggleFullScreen();
		void Minimize();
		void Overlay();
		weak<Menu> GameMenu;
		shared<StartGame> NewChessGame;
		sf::RenderWindow* WindowRef = nullptr;
		bool bIsFullscreen = false;
		sf::Vector2i PreFullscreenPosition;
		sf::Vector2u PreFullscreenSize;
	};
}