#include "Framework/Application.h"
#include "Framework/PropActor.h"
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
		NewChessGame->OnDraw.Bind(GetObject(), &Play::Draw);
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
		GameMenu.lock()->Checkmated();
		Overlay();
	}

	void Play::Stalemate()
	{
		GameMenu.lock()->SetVisibility(true);
		GameMenu.lock()->Stalemated();
	}

	void Play::Draw()
	{
		GameMenu.lock()->SetVisibility(true);
		GameMenu.lock()->Drawn();
	}

	void Play::RestartGame()
	{
		GetApplication()->LoadWorld<Play>();
	}

	void Play::QuitGame()
	{
		GetApplication()->QuitApplication();
	}

	void Play::Overlay()
	{
		auto OverlayBG = SpawnActor<Prop>("overlay.png");
		auto Overlay = OverlayBG.lock();
		Overlay->SetActorLocation({ GetWindowSize().x / 2.f, GetWindowSize().y / 2.f });
		Overlay->GetSprite().setColor({ 0, 0, 0, 140 });
	}
}