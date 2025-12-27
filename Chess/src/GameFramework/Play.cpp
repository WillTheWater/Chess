#include "Framework/Application.h"
#include "GameFramework/Play.h"
#include "GameFramework/StartGame.h"

namespace we
{
	Play::Play(Application* OwningApp)
		: World{OwningApp}
		, NewChessGame{ new StartGame{ this } }
	{
		GameMenu = CreateHUD<Menu>();
	}

	void Play::BeginPlay()
	{
		GameMenu.lock()->OnRestartButtonClicked.Bind(GetObject(), &Play::RestartGame);
		GameMenu.lock()->OnQuitButtonClicked.Bind(GetObject(), &Play::QuitGame);
		NewChessGame->OnCheckmate.Bind(GetObject(), &Play::Checkmate);
		NewChessGame->OnStalemate.Bind(GetObject(), &Play::Stalemate);
	}

	void Play::Tick(float DeltaTime)
	{
	}

	void Play::InitLevels()
	{
		AddLevel(NewChessGame);
	}

	void Play::Checkmate()
	{
		GameMenu.lock()->SetVisibility(true);
	}

	void Play::Stalemate()
	{
		GameMenu.lock()->SetVisibility(true);
	}

	void Play::RestartGame()
	{
		GetApplication()->LoadWorld<Play>();
	}

	void Play::QuitGame()
	{
		GetApplication()->QuitApplication();
	}
}