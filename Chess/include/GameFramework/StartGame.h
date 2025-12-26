
#pragma once
#include "GameMode/Level.h"

namespace we
{
	class StartGame : public Level
	{
	public:
		StartGame(World* World);

		virtual void BeginLevel() override;
		virtual void TickLevel(float DeltaTime) override;

	private:
		virtual void EndLevel() override;
		void SpawnBoard();
	};
}