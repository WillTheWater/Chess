#pragma once

#include "Framework/Actor.h"

namespace we
{
	enum class EChessPieceType
	{
		King = 0, 
		Queen = 1, 
		Bishop = 2, 
		Knight = 3, 
		Rook = 4, 
		Pawn = 5
	};

	enum class EChessColor 
	{ 
		White, 
		Black 
	};

	class ChessPiece : public Actor
	{
	public:
		ChessPiece(World* OwningWorld, EChessPieceType Type, EChessColor Color, const std::string& TexturePath = "/pieces.png");

		EChessPieceType GetPieceType() const { return PieceType; }
		EChessColor GetColor() const { return Color; }

		sf::Vector2i GetGridPosition() const { return GridPosition; } // Now returns the Vector2i member

		void SetGridPosition(sf::Vector2i& NewPosition);

	private:
		static constexpr int PIECE_SIZE = 78;
		static constexpr float GRID_ABS_OFFSET_X = 328.0f;
		static constexpr float GRID_ABS_OFFSET_Y = 49.0f;

		EChessPieceType PieceType;
		EChessColor Color;

		sf::Vector2i GridPosition{ 0, 0 };

		void ApplyPieceSubFrame();
		void UpdateSpritePosition();
	};
}