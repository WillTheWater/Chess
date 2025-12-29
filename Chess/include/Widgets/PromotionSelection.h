#pragma once
#include "UI/Image.h"
#include "UI/Button.h"

namespace we
{
	class PromotionSelector : public Image
	{
	public:
		PromotionSelector(const string& TexturePath = "selection.png");

		Delegate<> OnQueenSelected;
		Delegate<> OnRookSelected;
		Delegate<> OnBishopSelected;
		Delegate<> OnKnightSelected;

	private:
		Button QueenSelected;
		Button RookSelected;
		Button BishopSelected;
		Button KnightSelected;
	};
}