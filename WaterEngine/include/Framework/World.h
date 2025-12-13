#pragma once
#include "Framework/Core.h"
#include "SFML/Graphics.hpp"

namespace we
{
	class Application;
	class Actor;
	class HUD;

	class World
	{
	public:
		World(Application* OwningApp);
		virtual ~World();

		void BeginPlayGlobal();
		void TickGlobal(float DeltaTime);
		void Render(sf::RenderWindow& Window);
		void GarbageCollectionCycle();
		bool BroadcastEvent(const optional<sf::Event> Event);

		template<typename ActorType, typename... Args>
		weak<ActorType> SpawnActor(Args&&... args);

		template<typename HUDType, typename... Args>
		weak<HUDType> SpawnHUD(Args&&... args);

		sf::RenderWindow& GetRenderWindow() const;
		sf::Vector2u GetWindowSize() const;

	private:
		virtual void BeginPlay();
		virtual void Tick(float DeltaTime);
		void RenderHUD(sf::RenderWindow& Window);

		Application* OwningApp;
		bool bHasBegunPlay;

		List<shared<Actor>> Actors;
		List<shared<Actor>> PendingActors;
		shared<HUD> GameHUD;
	};


	template<typename ActorType, typename... Args>
	weak<ActorType> World::SpawnActor(Args&&... args)
	{
		shared<ActorType> NewActor{ new ActorType(this, std::forward<Args>(args)...) };
		PendingActors.push_back(NewActor);
		return NewActor;
	}

	template<typename HUDType, typename ...Args>
	weak<HUDType> World::SpawnHUD(Args && ...args)
	{
		shared<HUDType> NewHUD{ new HUDType(args...) };
		GameHUD = NewHUD;
		return NewHUD;
	}
}