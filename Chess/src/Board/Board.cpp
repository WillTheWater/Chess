#include "Board/Board.h"

namespace we
{
	Board::Board(World* OwningWorld, const std::string& TexturePath)
		: Actor{OwningWorld, TexturePath},
		BoardDimensions{624,624}
	{
		SetTexture(TexturePath, 720, 720);
	}
}