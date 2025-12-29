#include "Widgets/PromotionSelection.h"

namespace we
{
    PromotionSelector::PromotionSelector(const string& TexturePath)
        : Image{ TexturePath }
        , QueenSelected{ "pieces.png" }
        , RookSelected{ "pieces.png" }
        , BishopSelected{ "pieces.png" }
        , KnightSelected{ "pieces.png" }
    {
        QueenSelected.SetTextureRect(sf::IntRect({ 120, 0 }, { 120, 120 }));
        RookSelected.SetTextureRect(sf::IntRect({ 480, 0 }, { 120, 120 }));
        BishopSelected.SetTextureRect(sf::IntRect({ 240, 0 }, { 120, 120 }));
        KnightSelected.SetTextureRect(sf::IntRect({ 360, 0 }, { 120, 120 }));
    }

    void PromotionSelector::DrawChoices(Renderer& GameRenderer)
    {
        QueenSelected.NativeRender(GameRenderer);
        RookSelected.NativeRender(GameRenderer);
        KnightSelected.NativeRender(GameRenderer);
        BishopSelected.NativeRender(GameRenderer);
    }

    void PromotionSelector::Init()
    {
        sf::Vector2f SelectorPos = GetWidgetPosition();

        float ButtonSize = 120.f;
        float Gap = 120.f;

        auto CenterOriginOnRect = [](Button& Btn)
            {
                sf::IntRect Rect = Btn.GetButtonSprite().getTextureRect();
                Btn.GetButtonSprite().setOrigin({ Rect.size.x * 0.5f, Rect.size.y * 0.5f });
            };

        CenterOriginOnRect(QueenSelected);
        CenterOriginOnRect(RookSelected);
        CenterOriginOnRect(BishopSelected);
        CenterOriginOnRect(KnightSelected);

        QueenSelected.SetWidgetPosition({ SelectorPos.x, SelectorPos.y - 182.f });
        RookSelected.SetWidgetPosition({ SelectorPos.x, SelectorPos.y - 182.f + Gap });
        BishopSelected.SetWidgetPosition({ SelectorPos.x, SelectorPos.y - 182.f + Gap * 2 });
        KnightSelected.SetWidgetPosition({ SelectorPos.x, SelectorPos.y - 182.f + Gap * 3 });
    }
}