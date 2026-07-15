#pragma once
#include <string>
#include <vector>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include "game_manager.hpp"
#include "map.hpp"
#include "movement.hpp"
#include "hero.hpp"

class TuiController {
private:
    GameManager gamemanager; 
    Board gameMap;          
    std::vector<std::string> gamelogs;
    ftxui::Element createDynamicNode(const std::string& nodename, const std::string& dracPos, const std::string& sherlockPos, const std::string& watsonPos, const std::string& s1Pos, const std::string& s2Pos, const std::string& s3Pos) ;
    ftxui::Element drawExactGraphMap();
    ftxui::Element createStatusDashboard();

public:
    TuiController();
    void run(); 
};
