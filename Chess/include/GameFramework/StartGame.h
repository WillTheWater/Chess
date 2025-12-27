
#pragma once
#include "GameMode/Level.h"
#include "Board/Board.h"

namespace we
{
	class StartGame : public Level
	{
	public:
		StartGame(World* World);

		virtual void BeginLevel() override;
		virtual void TickLevel(float DeltaTime) override;

		Delegate<> OnCheckmate;
		void Checkmate();

	private:
		virtual void EndLevel() override;
		void SpawnBoard();
		weak<Board> ChessBoard;
	};
}