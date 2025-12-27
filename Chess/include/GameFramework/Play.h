#pragma once
#include "Framework/World.h"
#include "Framework/Core.h"
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
		void Checkmate();
		void Stalemate();
		void Draw();
		void RestartGame();
		void QuitGame();
		void Overlay();
		weak<Menu> GameMenu;
		shared<StartGame> NewChessGame;
	};
}