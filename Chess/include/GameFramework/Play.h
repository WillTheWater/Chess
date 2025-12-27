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
		virtual void EndLevels() override;
		virtual void InitLevels() override;
		void GameOver();
		void RestartGame();
		void QuitGame();
		weak<Menu> GameMenu;
		shared<StartGame> NewChessGame;
	};
}