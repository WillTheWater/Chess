
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

		Delegate<EPlayerTurn> OnCheckmate;
		Delegate<> OnStalemate;
		Delegate<> OnDraw;
		Delegate<sf::Vector2i> OnPromotionRequested;
		void Checkmate(EPlayerTurn Winner);
		void Stalemate();
		void Draw();
		void Promotion(sf::Vector2i PromotionSquare);
		void PromoteTo(EChessPieceType Choice, sf::Vector2i PromotionSquare);

	private:
		void SpawnBoard();
		weak<Board> ChessBoard;
	};
}