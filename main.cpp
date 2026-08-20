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
#include "../include/game_manager.hpp"
#include "../include/invisible_man.hpp"
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
                GameManager gm;

                
                
                
                
                
                
                
                bool team1IsDracula = (p1Hero == "DRACULA" || p2Hero == "DRACULA");
                bool team2IsSherlock = (p1Hero == "SHERLOCK HOLMES" || p2Hero == "SHERLOCK HOLMES");

                hero* team1Hero = nullptr;
                sister* sister1Ptr = nullptr;
                sister* sister2Ptr = nullptr;
                sister* sister3Ptr = nullptr;

                if (team1IsDracula)
                {
                    dracula* draculaPtr = new dracula();
                    draculaPtr->setposition(10);
                    team1Hero = draculaPtr;

                    sister1Ptr = new sister(1);
                    sister2Ptr = new sister(2);
                    sister3Ptr = new sister(3);
                    sister1Ptr->setposition(1);
                    sister2Ptr->setposition(2);
                    sister3Ptr->setposition(3);
                }
                else
                {
                    InvisibleMan* invPtr = new InvisibleMan();
                    invPtr->setposition(10);
                    team1Hero = invPtr;
                }

                hero* team2Hero = nullptr;
                watson* watsonPtr = nullptr;

                if (team2IsSherlock)
                {
                    sherlock* sherlockPtr = new sherlock();
                    sherlockPtr->setposition(23);
                    team2Hero = sherlockPtr;

                    watsonPtr = new watson();
                    watsonPtr->setposition(22);
                }
                else
                {
                    InvisibleMan* invPtr = new InvisibleMan();
                    invPtr->setposition(23);
                    team2Hero = invPtr;
                }

                gm.addCharacter(team1Hero, 1);
                if (sister1Ptr) gm.addCharacter(sister1Ptr, 1);
                if (sister2Ptr) gm.addCharacter(sister2Ptr, 1);
                if (sister3Ptr) gm.addCharacter(sister3Ptr, 1);
                gm.addCharacter(team2Hero, 2);
                if (watsonPtr) gm.addCharacter(watsonPtr, 2);

                
                
                
                
                
                
                
                if (InvisibleMan* invTeam1 = dynamic_cast<InvisibleMan*>(team1Hero))
                    invTeam1->initializeFogTokens(invTeam1->getposition());
                if (InvisibleMan* invTeam2 = dynamic_cast<InvisibleMan*>(team2Hero))
                    invTeam2->initializeFogTokens(invTeam2->getposition());

                
                
                std::string team1PickName = team1IsDracula ? std::string("DRACULA") : std::string("INVISIBLE MAN");
                int p1Team = (p1Hero == team1PickName) ? 1 : 2;
                int firstTeam = (firstPlayer == 1) ? p1Team : (p1Team == 1 ? 2 : 1);

                gm.startGame(firstTeam);
                RunGameUI(gm, team1Hero, sister1Ptr, sister2Ptr, sister3Ptr, team2Hero, watsonPtr, firstTeam, team1Hero, team2Hero);

                delete team1Hero;
                delete sister1Ptr;
                delete sister2Ptr;
                delete sister3Ptr;
                delete team2Hero;
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