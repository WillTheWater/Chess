#pragma once
#include "UI/Image.h"
#include "UI/Button.h"
#include "Board/Types.h"
#include "Framework/Delegate.h"

namespace we
{
	class PromotionSelector : public Image
	{
	public:
		PromotionSelector(const string& TexturePath = "selection.png");

		void DrawChoices(Renderer& GameRenderer);
		void Init();
		void SetPieceColor(EPlayerTurn Color);

		Delegate<> OnQueenSelected;
		Delegate<> OnRookSelected;
		Delegate<> OnBishopSelected;
		Delegate<> OnKnightSelected;

		Button QueenSelected;
		Button RookSelected;
		Button BishopSelected;
		Button KnightSelected;
	};
}