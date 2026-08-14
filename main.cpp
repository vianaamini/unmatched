#include "raylib.h"
#include "menu.hpp"
#include "age_screen.hpp"
#include "hero_selection.hpp"
#include "../include/map.hpp"
#include "character.hpp"
#include "hero.hpp"
#include "dracula.hpp"
#include "sherlock.hpp"
#include "sister.hpp"
#include "watson.hpp"
#include "raylib.hpp"
#include <iostream>
#include <vector>
#include <string>

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
                    
                    // تنظیم دقیق نوبت‌دهی بر اساس خروجی
                    firstPlayer = heroResult.firstPlayer; 

                    currentState = GameState::Playing;
                }
                else 
                {
                    currentState = GameState::AgeSelection;
                }
                break;
            }

            case GameState::Playing:
            {
                Board board;

                dracula* draculaPtr = new dracula();
                sister* sister1Ptr = new sister(1);
                sister* sister2Ptr = new sister(2);
                sister* sister3Ptr = new sister(3);
                sherlock* sherlockPtr = new sherlock();
                watson* watsonPtr = new watson();

                // موقعیت اولیه‌ی هر کاراکتر روی نقشه (نودهای n1 تا n32 طبق map.cpp)
                // بدون این خط‌ها هیچ کاراکتری روی نقشه نمایش داده نمی‌شه.
                draculaPtr->setposition(10);
                sister1Ptr->setposition(1);
                sister2Ptr->setposition(2);
                sister3Ptr->setposition(3);
                sherlockPtr->setposition(23);
                watsonPtr->setposition(22);

                // پاس دادن درست متغیر firstPlayer به محیط بازی
                RunGameUI(board, draculaPtr, sister1Ptr, sister2Ptr, sister3Ptr, sherlockPtr, watsonPtr, firstPlayer, draculaPtr, sherlockPtr);

                delete draculaPtr;
                delete sister1Ptr;
                delete sister2Ptr;
                delete sister3Ptr;
                delete sherlockPtr;
                delete watsonPtr;

                currentState = GameState::MainMenu; 
                break;
            }
            
            case GameState::Exit:
                break;
        }
    }

    return 0;
}