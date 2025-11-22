#pragma once
#include "Framework/Core.h"
#include "SFML/Graphics.hpp"

namespace we
{
	class Application;
	class Actor;

	class World
	{
	public:
		World(Application* OwningApp);
		virtual ~World();

		void BeginPlayGlobal();
		void TickGlobal(float DeltaTime);
		void Render(sf::RenderWindow& Window);
		void GarbageCollectionCycle();

		template<typename ActorType, typename... Args>
		weak<ActorType> SpawnActor(Args&&... args);

		sf::RenderWindow* GetRenderWindow() const;
		sf::Vector2u GetWindowSize() const;

	private:
		void BeginPlay();
		void Tick(float DeltaTime);

		Application* OwningApp;
		bool bHasBegunPlay;

		List<shared<Actor>> Actors;
		List<shared<Actor>> PendingActors;
	};


	template<typename ActorType, typename... Args>
	weak<ActorType> World::SpawnActor(Args&&... args)
	{
		shared<ActorType> NewActor{ new ActorType(this, std::forward<Args>(args)...) };
		PendingActors.push_back(NewActor);
		return NewActor;
	}
}