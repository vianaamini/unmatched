#include "raylib.h"
#include "menu.hpp"
#include "age_screen.hpp"
#include "hero_selection.hpp"
#include <iostream>

enum class GameState {
    MainMenu,
    AgeSelection,
    HeroSelection,
    Playing,
    Exit
};

int main()
{
    GameState currentState = GameState::MainMenu;
    int firstPlayer = 1;
    std::string p1Hero = "";
    std::string p2Hero = "";

    while (currentState != GameState::Exit)
    {
        switch (currentState)
        {
            case GameState::MainMenu:
            {
                Menu menu;
                MenuResult menuResult = menu.show();

                if (menuResult == MenuResult::Start)
                {
                    currentState = GameState::AgeSelection;
                }
                else if (menuResult == MenuResult::Load)
                {
                    // قابلیت لود بازی در صورت نیاز در اینجا قرار می‌گیرد
                    currentState = GameState::Playing; 
                }
                else
                {
                    currentState = GameState::Exit;
                }
                break;
            }

            case GameState::AgeSelection:
            {
                AgeScreen ageScreen;
                AgeScreenResult ageResult = ageScreen.show();

                if (ageResult == AgeScreenResult::EqualAge)
                {
                    // اگر مساوی بودند پیش‌فرض بازیکن اول شروع کند
                    firstPlayer = 1;
                    currentState = GameState::HeroSelection;
                }
                else if (ageResult == AgeScreenResult::Player1First)
                {
                    firstPlayer = 1;
                    currentState = GameState::HeroSelection;
                }
                else if (ageResult == AgeScreenResult::Player2First)
                {
                    firstPlayer = 2;
                    currentState = GameState::HeroSelection;
                }
                break;
            }

            case GameState::HeroSelection:
            {
                HeroSelection heroSelection;
                HeroSelectionResult heroResult = heroSelection.show(firstPlayer);

                if (heroResult.type == HeroSelectionResultType::EnterGame)
                {
                    p1Hero = heroResult.player1Hero;
                    p2Hero = heroResult.player2Hero;
                    currentState = GameState::Playing;
                }
                else // Retreat
                {
                    // بازگشت به صفحه تعیین سن یا انتخاب مجدد
                    currentState = GameState::AgeSelection;
                }
                break;
            }

            case GameState::Playing:
            {
                // حلقه اصلی و رندرینگ محیط بازی (نبرد)
                const int WIDTH = 1600;
                const int HEIGHT = 900;
                InitWindow(WIDTH, HEIGHT, "Unmatched - In Game");
                SetTargetFPS(60);

                Font gameFont = LoadFontEx("E:/assassins/new/unmatched/assets/fonts/Cinzel-SemiBold.ttf", 48, nullptr, 0);
                if (gameFont.texture.id != 0) SetTextureFilter(gameFont.texture, TEXTURE_FILTER_BILINEAR);

                while (!WindowShouldClose())
                {
                    // خروج اضطراری با کلید ESC به منو
                    if (IsKeyPressed(KEY_ESCAPE))
                    {
                        break;
                    }

                    BeginDrawing();
                    ClearBackground({ 15, 15, 22, 255 });

                    DrawTextEx(gameFont, "BATTLE HAS BEGUN", { 550.f, 350.f }, 32, 1.0f, { 212, 175, 55, 255 });
                    
                    std::string info1 = "Player 1 Hero: " + p1Hero;
                    std::string info2 = "Player 2 Hero: " + p2Hero;
                    DrawTextEx(gameFont, info1.c_str(), { 550.f, 430.f }, 22, 0.8f, { 235, 231, 220, 255 });
                    DrawTextEx(gameFont, info2.c_str(), { 550.f, 470.f }, 22, 0.8f, { 235, 231, 220, 255 });

                    DrawText("Press ESC to return to Menu", 600, 800, 16, { 145, 150, 165, 255 });

                    EndDrawing();
                }

                if (gameFont.texture.id != 0) UnloadFont(gameFont);
                CloseWindow();
                currentState = GameState::Exit;
                break;
            }

            case GameState::Exit:
                break;
        }
    }

    return 0;
}