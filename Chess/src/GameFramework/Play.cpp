#include "GameFramework/Play.h"
#include "GameFramework/StartGame.h"

namespace we
{
	Play::Play(Application* OwningApp)
		: World{OwningApp}
	{
	}

	void Play::BeginPlay()
	{
	}

	void Play::Tick(float DeltaTime)
	{
	}

	void Play::EndLevels()
	{
	}

	void Play::InitLevels()
	{
		AddLevel(shared<StartGame>{new StartGame{ this }});
	}

	void Play::GameOver()
	{
	}

	void Play::RestartGame()
	{
	}

	void Play::QuitGame()
	{
	}
}