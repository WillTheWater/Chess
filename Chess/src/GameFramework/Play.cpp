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
		, PromotionSquare{ sf::Vector2i{0,0}}
	{
		GameMenu = CreateHUD<Menu>();
	}

	void Play::BeginPlay()
	{
		GameMenu.lock()->OnRestartButtonClicked.Bind(GetWeakObject(), &Play::RestartGame);
		GameMenu.lock()->OnQuitButtonClicked.Bind(GetWeakObject(), &Play::QuitGame);
		GameMenu.lock()->OnFullScreenButtonClicked.Bind(GetWeakObject(), &Play::ToggleFullScreen);
		GameMenu.lock()->OnMinimizeButtonClicked.Bind(GetWeakObject(), &Play::Minimize);
		GameMenu.lock()->OnQueenSelected.Bind(GetWeakObject(), &Play::ChooseQueen);
		GameMenu.lock()->OnRookSelected.Bind(GetWeakObject(), &Play::ChooseRook);
		GameMenu.lock()->OnBishopSelected.Bind(GetWeakObject(), &Play::ChooseBishop);
		GameMenu.lock()->OnKnightSelected.Bind(GetWeakObject(), &Play::ChooseKnight);
		NewChessGame->OnCheckmate.Bind(GetWeakObject(), &Play::Checkmate);
		NewChessGame->OnStalemate.Bind(GetWeakObject(), &Play::Stalemate);
		NewChessGame->OnDraw.Bind(GetWeakObject(), &Play::Draw);
		NewChessGame->OnPromotionRequested.Bind(GetWeakObject(), &Play::Promotion);
		sf::RenderWindow& Win = GetApplication()->GetRenderer()->GetRenderWindow();
		sf::Vector2u GameResolution = { 1920, 1080 };
		ApplyAspectRatio(GetApplication()->IsFullscreen(), Win.getSize(), GameResolution);
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
		GameMenu.lock()->SetWinnerText(Winner);
		GameMenu.lock()->SetVisibility(true);
		GameMenu.lock()->Checkmated();
		Overlay();
	}

	void Play::Stalemate()
	{
		GameMenu.lock()->SetVisibility(true);
		GameMenu.lock()->Stalemated();
		Overlay();
	}

	void Play::Draw()
	{
		GameMenu.lock()->SetVisibility(true);
		GameMenu.lock()->Drawn();
		Overlay();
	}

	void Play::Promotion(EPlayerTurn Color, sf::Vector2i NewPromotionSquare)
	{
		GameMenu.lock()->PromotionVisibility(Color, true);
		PromotionSquare = NewPromotionSquare;
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
		Application* App = GetApplication();
		sf::RenderWindow& Win = App->GetRenderer()->GetRenderWindow();

		bool bIsNowFullscreen = !App->IsFullscreen();
		App->SetFullscreen(bIsNowFullscreen);

		sf::Vector2u GameResolution = { 1920, 1080 };
		ApplyAspectRatio(bIsNowFullscreen, Win.getSize(), GameResolution);
	}

	void Play::Minimize()
	{
		Application* App = GetApplication();
		sf::RenderWindow& Win = App->GetRenderer()->GetRenderWindow();
		sf::WindowHandle handle = Win.getNativeHandle();
		ShowWindow(static_cast<HWND>(handle), SW_MINIMIZE);
	}

	void Play::ChooseQueen()
	{
		NewChessGame->PromoteTo(EChessPieceType::Queen, PromotionSquare);
	}

	void Play::ChooseRook()
	{
		NewChessGame->PromoteTo(EChessPieceType::Rook, PromotionSquare);
	}

	void Play::ChooseBishop()
	{
		NewChessGame->PromoteTo(EChessPieceType::Bishop, PromotionSquare);
	}

	void Play::ChooseKnight()
	{
		NewChessGame->PromoteTo(EChessPieceType::Knight, PromotionSquare);
	}

	void Play::Overlay()
	{
		Application* App = GetApplication();
		sf::RenderWindow& Win = App->GetRenderer()->GetRenderWindow();
		auto OverlayBG = SpawnActor<Prop>("overlay.png");
		auto Overlay = OverlayBG.lock();
		sf::Vector2f ViewSize = Win.getView().getSize();
		Overlay->SetSpriteScale({ 4.f,4.f });
		Overlay->CenterPivot();
		Overlay->SetActorLocation({ ViewSize.x / 2.f, ViewSize.y / 2.f });
		Overlay->GetSprite().setColor({ 0, 0, 0, 140 });
	}

	void Play::ApplyAspectRatio(bool bIsFullscreenMode, const sf::Vector2u& WindowSize, const sf::Vector2u& GameResolution)
	{
		Application* App = GetApplication();
		sf::RenderWindow& Win = App->GetRenderer()->GetRenderWindow();
		float ScreenAspect = static_cast<float>(WindowSize.x) / static_cast<float>(WindowSize.y);
		float GameAspect = static_cast<float>(GameResolution.x) / static_cast<float>(GameResolution.y);

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

		sf::View CurrentView(sf::FloatRect({ 0, 0 }, { static_cast<float>(GameResolution.x), static_cast<float>(GameResolution.y) }));
		CurrentView.setViewport(Viewport);
		Win.setView(CurrentView);
	}
}