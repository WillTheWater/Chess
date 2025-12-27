
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
		Delegate<> OnStalemate;
		Delegate<> OnDraw;
		void Checkmate();
		void Stalemate();
		void Draw();

	private:
		void SpawnBoard();
		weak<Board> ChessBoard;
	};
}