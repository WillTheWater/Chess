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
		ChessPiece(World* OwningWorld, EChessPieceType Type, EChessColor Color, int GridX, int GridY);

		EChessPieceType GetPieceType() const { return PieceType; }
		EChessColor GetColor() const { return Color; }
		sf::Vector2i GetGridPosition() const { return { GridX, GridY }; }

		void SetGridPosition(int NewX, int NewY);

	private:
		sf::Vector2i PieceSize = { 80,90 };

		EChessPieceType PieceType;
		EChessColor Color;
		int GridX;
		int GridY;

		void SetupPieceTexture(const std::string& TexturePath);
		void UpdateSpritePosition();
	};
}