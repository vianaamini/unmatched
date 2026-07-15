#pragma once
#include <string>
#include <vector>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include "game_manager.hpp"
#include "map.hpp"
#include "hero.hpp"

class TuiController {
private:
    GameManager gamemanager; 
    Board gameMap;          
    std::vector<std::string> gamelogs;
    std::string currentCommand;
    int menuSelected = 0;
    int screenMode = 0;
    
    // توابع نقشه
    ftxui::Element createDynamicNode(const std::string& nodename, const std::string& dracPos, const std::string& sherlockPos, const std::string& watsonPos, const std::string& s1Pos, const std::string& s2Pos, const std::string& s3Pos);
    ftxui::Element drawExactGraphMap();
    
    // توابع جدید (اضافه شده)
    ftxui::Element createHealthBar(int current, int max);
    ftxui::Element createCardDisplay(const card& c, const std::string& type, const std::string& range, const std::string& effect, int index);
    ftxui::Element createHeroPanel(character* c, const std::string& title, ftxui::Color titleColor);
    ftxui::Element createHandPanel(hero* h, const std::string& title, ftxui::Color titleColor);
    ftxui::Element createActionLog();
    ftxui::Element createMapDisplay();
    ftxui::Element createCommandsPanel();
    ftxui::Element createLocationInfo();
    ftxui::Element createLegend();
    ftxui::Element createStatusDashboard();
    ftxui::Element createActionMenu();
    
    void processCommand(const std::string& cmd);

public:
    TuiController();
    void run(); 
};