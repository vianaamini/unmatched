#pragma once
#include <string>
#include <vector>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include "game_manager.hpp"
#include "map.hpp"
#include "hero.hpp"
#include "deployment.hpp"
#include "invisible_man.hpp"

class TuiController {
private:
    GameManager gamemanager; 
    Board gameMap;          
    std::vector<std::string> gamelogs;
    std::string currentCommand;
    int menuSelected = 0;
    int screenMode = 0;
    int age1, age2;
    int firstPlayer;
    int choice;
    int p1Choice;
    int p2Choice;
    bool gameStarted = false;
    
    ftxui::Element createDynamicNode(const std::string& nodename, 
                                     const std::string& dracPos, 
                                     const std::string& sherlockPos, 
                                     const std::string& watsonPos, 
                                     const std::string& s1Pos, 
                                     const std::string& s2Pos, 
                                     const std::string& s3Pos);
    ftxui::Element drawExactGraphMap();
    
    ftxui::Element createHealthBar(int current, int max);
    ftxui::Element createCardDisplay(const card& c, const std::string& type, 
                                     const std::string& range, const std::string& effect, int index);
    ftxui::Element createHeroPanel(character* c, const std::string& title, ftxui::Color titleColor);
    ftxui::Element createHandPanel(hero* h, const std::string& title, ftxui::Color titleColor);
    ftxui::Element createActionLog();
    ftxui::Element createCommandsPanel();
    ftxui::Element createLocationInfo();
    ftxui::Element createLegend();
    ftxui::Element createStatusDashboard();
    
    ftxui::Element createActionMenu();
    
    void processCommand(const std::string& cmd);
    void setupCharacters();
    int firstFighterChoice;

public:
    TuiController();
    void run(); 
};