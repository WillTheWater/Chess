#include "Board/TestBoard.h"

namespace we
{
	TestBoard::TestBoard(World* OwningWorld, const std::string& TexturePath)
		: Actor{OwningWorld, TexturePath}
	{
	}

	void TestBoard::BeginPlay()
	{
		LOG("TestBoard BeginPlay")
	}

	void TestBoard::Render(Renderer& GameRenderer)
	{
		Actor::Render(GameRenderer);

		for (const auto& Piece : Pieces)
		{
			Piece->Render(GameRenderer);
		}
	}
}