#pragma once
#include "Framework/World.h"
#include "Framework/Core.h"
#include "Board/Board.h"
#include "Widgets/Menu.h"

namespace we
{
    class StartGame;

    class Play : public World
    {
    public:
        Play(Application* OwningApp);

    private:
        virtual void BeginPlay() override;
        virtual void Tick(float DeltaTime) override;
        virtual void InitLevels() override;
        void Checkmate(EPlayerTurn Winner);
        void Stalemate();
        void Draw();
        void Promotion(sf::Vector2i PromotionSquare);
        void RestartGame();
        void QuitGame();
        void ToggleFullScreen();
        void Minimize();
        void Overlay();

        void ApplyAspectRatio(bool bIsFullscreenMode, const sf::Vector2u& WindowSize, const sf::Vector2u& GameResolution);

        weak<Menu> GameMenu;
        shared<StartGame> NewChessGame;
    };
}