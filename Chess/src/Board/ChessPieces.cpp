#include "Board/ChessPieces.h"

namespace we
{
	ChessPiece::ChessPiece(World* OwningWorld, EChessPieceType Type, EChessColor Color, int GridX, int GridY)
		: Actor{OwningWorld}
	{
	}
}