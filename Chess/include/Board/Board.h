#pragma once

#include "Framework/Actor.h"

namespace we
{
	class Board : public Actor
	{
	public:
		Board(World* OwningWolrd, const std::string& TexturePath = "/board.png");

	private:
		sf::Vector2i BoardDimensions{};

	};
}