#include "Framework/World.h"
#include "Framework/Core.h"
#include "Framework/Actor.h"
#include "Framework/Application.h"
#include "Widgets/HUD.h"

namespace we
{
	World::World(Application* OwningApp)
		: OwningApp{OwningApp},
		Actors{},
		PendingActors{},
		bHasBegunPlay{false}
	{
	}

	World::~World()
	{
	}

	void World::BeginPlayGlobal()
	{
		if (!bHasBegunPlay)
		{
			bHasBegunPlay = true;
			BeginPlay();
		}
	}

	void World::TickGlobal(float DeltaTime)
	{
		for (auto Actor : PendingActors)
		{
			Actors.push_back(Actor);
			Actor->BeginPlayGlobal();
		}

		PendingActors.clear();

		for (auto i = Actors.begin(); i != Actors.end(); )
		{
			i->get()->Tick(DeltaTime);
			i++;	
		}

		Tick(DeltaTime);

		if (GameHUD)
		{
			if (!GameHUD->IsInitialized())
			{
				GameHUD->NativeInitialize(GetRenderWindow());
			}
			GameHUD->Tick(DeltaTime);
		}
	}

	void World::BeginPlay()
	{
	}

	void World::Tick(float DeltaTime)
	{
	
	}

	void World::RenderHUD(sf::RenderWindow& Window)
	{
		if (GameHUD)
		{
			GameHUD->Draw(Window);
		}
	}

	void World::Render(sf::RenderWindow& Window)
	{
		for (auto& Actor : Actors)
		{
			Actor->Render(Window);
		}

		RenderHUD(Window);
	}

	void World::GarbageCollectionCycle()
	{
		for (auto i = Actors.begin(); i != Actors.end(); )
		{
			if (i->get()->IsPendingDestroy())
			{
				i = Actors.erase(i);
			}
			else
			{
				i++;
			}
		}
	}

	bool World::BroadcastEvent(const optional<sf::Event> Event)
	{
		if (GameHUD)
		{
			return GameHUD->HandleEvent(Event);
		}
		return false;
	}

	sf::RenderWindow& World::GetRenderWindow() const
	{
		return OwningApp->GetRenderWindow();
	}

	sf::Vector2u World::GetWindowSize() const
	{
		return OwningApp->GetWindowSize();
	}
}