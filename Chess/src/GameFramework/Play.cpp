#include "Framework/Application.h"
#include <windows.h>
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
		GameMenu.lock()->OnRestartButtonClicked.Bind(GetWeakObject(), &Play::RestartGame);
		GameMenu.lock()->OnQuitButtonClicked.Bind(GetWeakObject(), &Play::QuitGame);
		GameMenu.lock()->OnFullScreenButtonClicked.Bind(GetWeakObject(), &Play::ToggleFullScreen);
		GameMenu.lock()->OnMinimizeButtonClicked.Bind(GetWeakObject(), &Play::Minimize);
		NewChessGame->OnCheckmate.Bind(GetWeakObject(), &Play::Checkmate);
		NewChessGame->OnStalemate.Bind(GetWeakObject(), &Play::Stalemate);
		NewChessGame->OnDraw.Bind(GetWeakObject(), &Play::Draw);
		WindowRef = &(GetApplication()->GetRenderer()->GetRenderWindow());
	}

	void Play::Tick(float DeltaTime)
	{
	}

	void Play::InitLevels()
	{
		AddLevel(NewChessGame);
	}

	void Play::Checkmate(EPlayerTurn Winner)
	{
		switch (Winner)
		{
			case EPlayerTurn::White:
			{
				LOG("White Wins")
				break;
			}
			case EPlayerTurn::Black:
			{
				LOG("Black Wins")
				break;
			}
		}
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

	void Play::ToggleFullScreen()
	{
		if (!WindowRef) return;

		if (!bIsFullscreen)
		{
			PreFullscreenPosition = WindowRef->getPosition();
			PreFullscreenSize = WindowRef->getSize();

			sf::VideoMode DesktopMode = sf::VideoMode::getDesktopMode();
			WindowRef->setSize(DesktopMode.size);
			WindowRef->setPosition(sf::Vector2i(0, 0));

			float ScreenAspect = static_cast<float>(DesktopMode.size.x) / static_cast<float>(DesktopMode.size.y);
			float GameAspect = static_cast<float>(PreFullscreenSize.x) / static_cast<float>(PreFullscreenSize.y);

			sf::FloatRect Viewport({ 0.f, 0.f }, { 1.f, 1.f });

			if (ScreenAspect > GameAspect)
			{
				Viewport.size.x = GameAspect / ScreenAspect;
				Viewport.position.x = (1.f - Viewport.size.x) / 2.f;
			}
			else
			{
				Viewport.size.y = ScreenAspect / GameAspect;
				Viewport.position.y = (1.f - Viewport.size.y) / 2.f;
			}

			sf::View CurrentView(sf::FloatRect({ 0, 0 }, { static_cast<float>(PreFullscreenSize.x), static_cast<float>(PreFullscreenSize.y) }));

			CurrentView.setViewport(Viewport);

			WindowRef->setView(CurrentView);

			bIsFullscreen = true;
		}
		else
		{
			WindowRef->setSize(PreFullscreenSize);
			WindowRef->setPosition(PreFullscreenPosition);

			sf::View DefaultView(sf::FloatRect({ 0, 0 }, { static_cast<float>(PreFullscreenSize.x), static_cast<float>(PreFullscreenSize.y) }));
			WindowRef->setView(DefaultView);

			bIsFullscreen = false;
		}
	}

	void Play::Minimize()
	{
		if (WindowRef)
		{
			sf::WindowHandle handle = WindowRef->getNativeHandle();
			ShowWindow(static_cast<HWND>(handle), SW_MINIMIZE);
		}
	}

	void Play::Overlay()
	{
		auto OverlayBG = SpawnActor<Prop>("overlay.png");
		auto Overlay = OverlayBG.lock();
		Overlay->SetActorLocation({ GetWindowSize().x / 2.f, GetWindowSize().y / 2.f });
		Overlay->GetSprite().setColor({ 0, 0, 0, 140 });
	}
}