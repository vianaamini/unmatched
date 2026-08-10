#include "raylib.h"
#include "menu.hpp"
#include "age_screen.hpp"
#include "hero_selection.hpp"
#include "../include/map.hpp"
#include "raylib.hpp"
#include <iostream>
#include <vector>
#include <string>
static Texture2D LoadTextureWithFallbacksForMain(const std::string& category, const std::vector<std::string>& filenames) {
    std::vector<std::string> basePaths = {
        "assets/",               
        "assets/heroes/",        
        "assets/images/",
        "assest/",                
        "assest/heroes/",
        "../assets/",
        "../assets/heroes/",
        "build/assets/",
        "build/assets/heroes/"
    };

    for (const auto& filename : filenames) {
        for (const auto& basePath : basePaths) {
            std::string fullPath = basePath + (category.empty() ? "" : category + "/") + filename;
            
            if (FileExists(fullPath.c_str())) {
                Texture2D tex = LoadTexture(fullPath.c_str());
                if (tex.id > 0) {
                    SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);
                    std::cout << "[SUCCESS] Loaded: " << fullPath << std::endl;
                    return tex;
                }
            }
        }
    }
    std::cout << "[ERROR] Could not find texture for: " << (filenames.empty() ? "" : filenames[0]) << std::endl;
    return Texture2D{ 0 };
}



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
                else // Retreat / Reset
                {
                    currentState = GameState::AgeSelection;
                }
                break;
            }

            case GameState::Playing:
            {
                Board board;
                
                RunGameUI(board); 
                currentState = GameState::MainMenu; 
                break;

            }

            case GameState::Exit:
                break;
        }
    }

    return 0;
}