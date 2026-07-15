#include "tui_controller.hpp"
#include <ftxui/component/screen_interactive.hpp>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
using namespace ftxui;

TuiController::TuiController() {
    gamelogs.push_back("System: TUI initialized");
    gamelogs.push_back("System: Ready for Unmatched game");
}

Element TuiController::createDynamicNode(const std::string& nodename, const std::string& dracPos, const std::string& sherlockPos, const std::string& watsonPos, const std::string& s1Pos, const std::string& s2Pos, const std::string& s3Pos) {
    
    NodeColor colorEnum = gameMap.getNodeColorByName(nodename);
    
    Color nodeZoneColor = Color::Default;
    switch (colorEnum) {
        case NodeColor::YELLOW: nodeZoneColor = Color::Yellow; break;
        case NodeColor::GREEN:  nodeZoneColor = Color::Green;  break;
        case NodeColor::BLUE:   nodeZoneColor = Color::Blue;   break;
        case NodeColor::RED:    nodeZoneColor = Color::Red;    break;
        case NodeColor::PURPLE: nodeZoneColor = Color::Magenta;break;
        case NodeColor::BROWN:  nodeZoneColor = Color::White;  break; 
        default:                nodeZoneColor = Color::Default;break;
    }

    vector<Element> indicators;
    indicators.push_back(text(nodename) | bold);

    if (nodename == dracPos) indicators.push_back(text(" [D]") | color(Color::Red) | bold);
    if (nodename == sherlockPos) indicators.push_back(text(" [S]") | color(Color::Blue) | bold);
    if (nodename == watsonPos) indicators.push_back(text(" [W]") | color(Color::Cyan) | bold);
    if (nodename == s1Pos) indicators.push_back(text(" [1]") | color(Color::White) | bold);
    if (nodename == s2Pos) indicators.push_back(text(" [2]") | color(Color::White) | bold);
    if (nodename == s3Pos) indicators.push_back(text(" [3]") | color(Color::White) | bold);

    return hbox(move(indicators)) | center | color(nodeZoneColor) | border;
}

Element TuiController::drawExactGraphMap() {
    std::string dPos = "", sPos = "", wPos = "", s1 = "", s2 = "", s3 = "";
    auto allChars = gamemanager.getAllCharacters(); 
    for (character* c : allChars) {
        if (c->getname() == "Dracula") dPos = c->getPositionString();
        else if (c->getname() == "Sherlock Holmes") sPos = c->getPositionString();
        else if (c->getname() == "Watson") wPos = c->getPositionString();
        else if (c->getname() == "Sister 1") s1 = c->getPositionString();
        else if (c->getname() == "Sister 2") s2 = c->getPositionString();
        else if (c->getname() == "Sister 3") s3 = c->getPositionString();
    } 

    auto n1 = createDynamicNode("n1", dPos, sPos, wPos, s1, s2, s3);
    auto n2 = createDynamicNode("n2", dPos, sPos, wPos, s1, s2, s3);
    auto n3 = createDynamicNode("n3", dPos, sPos, wPos, s1, s2, s3);
    auto n4 = createDynamicNode("n4", dPos, sPos, wPos, s1, s2, s3);
    auto n5 = createDynamicNode("n5", dPos, sPos, wPos, s1, s2, s3);
    auto n6 = createDynamicNode("n6", dPos, sPos, wPos, s1, s2, s3);
    auto n7 = createDynamicNode("n7", dPos, sPos, wPos, s1, s2, s3);
    auto n8 = createDynamicNode("n8", dPos, sPos, wPos, s1, s2, s3);
    auto n9 = createDynamicNode("n9", dPos, sPos, wPos, s1, s2, s3);
    auto n10 = createDynamicNode("n10", dPos, sPos, wPos, s1, s2, s3);
    auto n11 = createDynamicNode("n11", dPos, sPos, wPos, s1, s2, s3);
    auto n12 = createDynamicNode("n12", dPos, sPos, wPos, s1, s2, s3);
    auto n13 = createDynamicNode("n13", dPos, sPos, wPos, s1, s2, s3);
    auto n14 = createDynamicNode("n14", dPos, sPos, wPos, s1, s2, s3);
    auto n15 = createDynamicNode("n15", dPos, sPos, wPos, s1, s2, s3);
    auto n16 = createDynamicNode("n16", dPos, sPos, wPos, s1, s2, s3);
    auto n17 = createDynamicNode("n17", dPos, sPos, wPos, s1, s2, s3);
    auto n18 = createDynamicNode("n18", dPos, sPos, wPos, s1, s2, s3);
    auto n19 = createDynamicNode("n19", dPos, sPos, wPos, s1, s2, s3);
    auto n20 = createDynamicNode("n20", dPos, sPos, wPos, s1, s2, s3);
    auto n21 = createDynamicNode("n21", dPos, sPos, wPos, s1, s2, s3);

    return vbox({
        text("─── MAP (Spider Web Graph) ───") | center | bold | color(Color::Green),
        separator(),
        hbox({ text("         "), n1, text("─────────"), n2, text("─────────"), n3, text("         ") }) | center,
        hbox({ text("          ╱   ╲         │             ╲          ") }) | center,
        hbox({ text("   "), n4, text("────"), n5, text("────"), n6, text("────"), n7, text("────"), n8, text("   ") }) | center,
        hbox({ text("         ╱           ╲   │   ╱           ╲        ") }) | center,
        hbox({ text("   "), n9, text("------"), n10, text("-----"), n11, text("-----"), n12, text("-----"), n13, text("   ") }) | center,
        hbox({ text("         ╲           ╱   │   ╲           ╱        ") }) | center,
        hbox({ text("   "), n14, text("────"), n15, text("────"), n16, text("────"), n17, text("────"), n18, text("   ") }) | center,
        hbox({ text("                     ╲   ╱       │             ╱                   ") }) | center,
        hbox({ text("         "), n19, text("─────────"), n20, text("─────────"), n21, text("         ") }) | center,
    }) | center | flex;
}

Element TuiController::createStatusDashboard() {
    auto allChars = gamemanager.getAllCharacters();
    Elements heroPanels;

    for (character* c : allChars) {
        if (!c) continue;
        
        hero* h = dynamic_cast<hero*>(c);

        Elements info;
        info.push_back(text(c->getname()) | bold | color(Color::Yellow));
        info.push_back(separator());
        info.push_back(text("HP: " + to_string(c->gethealth()))
                        | color(c->isalive() ? Color::Green : Color::Red));
        info.push_back(text("Position: " + c->getPositionString()));

        if (h) {
            info.push_back(text("Hand: " + to_string(h->handsize()) + " cards"));
            info.push_back(text("Deck: " + to_string(h->getdeck().getsize()) + " cards"));
            info.push_back(text("Actions: " + to_string(h->get_actions())));
        }

        heroPanels.push_back(vbox(move(info)) | border | size(WIDTH, EQUAL, 20));
    }

    character* current = gamemanager.getCurrentCharacter();

    Elements logElements;
    int start_idx = max(0, (int)gamelogs.size() - 6);
    for (size_t i = start_idx; i < gamelogs.size(); ++i) {
        logElements.push_back(text("> " + gamelogs[i]));
    }

    return vbox({
        text(" STATUS DASHBOARD ") | bold | color(Color::Cyan) | center,
        separator(),
        text("Turn: " + to_string(gamemanager.getTurnNumber()) +
             "  |  Actions Left: " + to_string(gamemanager.getActionsRemaining())) | center,
        separator(),
        hbox(move(heroPanels)) | center,
        separator(),
        text("Current Turn: " + (current ? current->getname() : "N/A"))
            | bold | color(Color::Magenta) | center,
        separator(),
        text(" GAME LOG ") | bold | color(Color::Yellow),
        vbox(move(logElements)) | border | size(HEIGHT, EQUAL, 8)
    }) | border;
}

Element TuiController::createActionMenu() {
    return vbox({
        text(" ACTION MENU ") | bold | color(Color::Green) | center,
        separator(),
        hbox({
            text(" [Maneuver] ") | border | color(Color::Blue),
            text(" [Scheme] ") | border | color(Color::Magenta),
            text(" [Attack] ") | border | color(Color::Red),
            text(" [Discard] ") | border | color(Color::Yellow),
        }) | center,
        hbox({
            text(" [Draw Card] ") | border | color(Color::Green),
            text(" [Help] ") | border | color(Color::Cyan),
            text(" [Back to Menu] ") | border | color(Color::White),
        }) | center,
        separator(),
        text("Select an action using arrow keys and ENTER") | center | dim
    }) | border | center;
}

void TuiController::run() {
    auto screen = ScreenInteractive::Fullscreen();
    
    int screen_mode = 0; 
    int menu_selected = 0;

    vector<string> menu_entries = {
        "   [ Play Game ]   ", 
        "   [ Help & Rules ]", 
        "   [ Exit Game ]   "
    };

    auto menu_components = Menu(&menu_entries, &menu_selected);

    auto menu_renderer = Renderer(menu_components, [&] {
        return vbox({
            text("┌────────────────────────────────────────────┐") | center | bold | color(Color::Green),
            text("│               UNMATCHED MAP                │") | center | bold | color(Color::Green),
            text("└────────────────────────────────────────────┘") | center | bold | color(Color::Green),
            text("1. Main Navigation Menu") | center | bold,
            separator(),
            vbox({ menu_components->Render() | center }) | center,
            separator(),
            text("Use UP/DOWN Arrows to Navigate • Press ENTER to Select") | center | color(Color::Yellow)
        }) | center | border;
    });

    auto menu_event_handler = CatchEvent(menu_renderer, [&](Event event) {
        if (event == Event::Return) {
            if (menu_selected == 0) {
                screen_mode = 2; 
                gamemanager.startGame(); 
            } else if (menu_selected == 1) {
                screen_mode = 1; 
            } else if (menu_selected == 2) {
                screen.ExitLoopClosure()(); 
            }
            return true;
        }
        return false;
    });

    auto help_renderer = Renderer([&] {
        return vbox({
            text("  HOW TO PLAY & SYSTEM COMMANDS  ") | center | bold | color(Color::Cyan),
            separator(),
            paragraph("Move your hero across the 32-node organic web structure.") | center,
            paragraph("Type commands into the input terminal field located at the bottom panel.") | center,
            separator(),
            vbox({
                text("• move <location>  - Relocate hero to a connected adjacent node (e.g., move n14)"),
                text("• play <card_idx>  - Activate card from active hand (1-based index selection)"),
                text("• end              - Close active phase and pass turn sequence"),
                text("• hand             - Display list of currently held hand cards"),
                text("• deck             - Render deck structural values and discard pile size"),
                text("• log              - Print historical match logs inside telemetry section"),
                text("• help             - Toggle full screen instructions overlay frame"),
                text("• quit             - Terminate match phase and drop back to main menu")
            }) | border,
            separator(),
            text("Press [ ENTER ] to return to Main Menu") | center | color(Color::Yellow)
        }) | center | border;
    });

    auto help_event_handler = CatchEvent(help_renderer, [&](Event event) {
        if (event == Event::Return) {
            screen_mode = 0; 
            return true;
        }
        return false;
    });

    string input_command = "";
    Component input_box = Input(&input_command, "Enter command...");

    auto gameplay_renderer = Renderer(input_box, [&] {
        auto network_map = drawExactGraphMap(); 
        character* current_char = gamemanager.getCurrentCharacter();

        auto status_dashboard = createStatusDashboard();

        auto action_menu_panel = createActionMenu();

        auto commands_guide_panel = vbox({
            text(" GAME COMMANDS ") | bold | color(Color::Yellow),
            separator(),
            vbox({
                text("move <location>     - Move your hero"),
                text("play <card_index>   - Play a card from hand (1-based index)"),
                text("end                 - End your turn"),
                text("hand                - Show your hand"),
                text("deck                - Show deck info"),
                text("log                 - Show action log"),
                text("help                - Show this help"),
                text("quit                - Quit game")
            })
        }) | border;

        Elements log_elements;
        int start_idx = max(0, (int)gamelogs.size() - 8); 
        for (size_t i = start_idx; i < gamelogs.size(); ++i) {
            log_elements.push_back(text("> " + gamelogs[i]));
        }
        auto log_panel = vbox(move(log_elements)) | border | size(HEIGHT, EQUAL, 10);

        auto input_line_panel = hbox({
            text(" Enter command: ") | bold | color(Color::Cyan),
            input_box->Render() | flex
        }) | border;

        Elements mainElements = {
            hbox({ network_map | flex, status_dashboard }),
            action_menu_panel,
            hbox({ commands_guide_panel | flex, log_panel | flex }),
            input_line_panel
        };
        return vbox(mainElements);
    });

    auto gameplay_event_handler = CatchEvent(gameplay_renderer, [&](Event event) {
        if (event == Event::Return) {
            if (!input_command.empty()) {
                string cmd = input_command;
                input_command = ""; 
                
                if (cmd.rfind("move ", 0) == 0) {
                    try {
                        string node_part = cmd.substr(5);
                        if (!node_part.empty() && node_part[0] == 'n') {
                            node_part = node_part.substr(1);
                        }
                        int target_node_id = stoi(node_part);
                        character* current_char = gamemanager.getCurrentCharacter();
                        bool success = gamemanager.moveCharacter(current_char, "n" + to_string(target_node_id));
                        
                        if (success) {
                            gamelogs.push_back("Action: Character moved to node n" + to_string(target_node_id));
                        } else {
                            gamelogs.push_back("Error: Invalid move! Nodes are not adjacent or path is blocked.");
                        }
                    } catch (...) {
                        gamelogs.push_back("Syntax Error: Use 'move <node_id>' (e.g., move 26)");
                    }
                } 
                else if (cmd.rfind("play ", 0) == 0) {
                    try {
                        int card_idx = stoi(cmd.substr(5));
                        character* current_char = gamemanager.getCurrentCharacter();
                        hero* h = dynamic_cast<hero*>(current_char);
                        if (h && card_idx >= 1 && card_idx <= h->handsize()) {
                            card played = h->gethand()[card_idx - 1];
                            h->gethand().erase(h->gethand().begin() + card_idx - 1);
                            gamelogs.push_back("Action: Played card [" + played.get_name() + "]");
                        } else {
                            gamelogs.push_back("Error: Invalid card index!");
                        }
                    } catch (...) {
                        gamelogs.push_back("Syntax Error: Use 'play <card_index>' (e.g., play 1)");
                    }
                } 
                else if (cmd == "end") {
                    gamemanager.nextTurn();
                    gamelogs.push_back("Action: Player ended turn.");
                } 
                else if (cmd == "hand") {
                    character* current_char = gamemanager.getCurrentCharacter();
                    hero* h = dynamic_cast<hero*>(current_char);
                    if (h) {
                        gamelogs.push_back("--- CURRENT HAND ---");
                        auto current_hand = h->gethand();
                        for (size_t i = 0; i < current_hand.size(); ++i) {
                            gamelogs.push_back("[" + to_string(i + 1) + "] " + current_hand[i].get_name());
                        }
                    }
                } 
                else if (cmd == "deck") {
                    character* current_char = gamemanager.getCurrentCharacter();
                    hero* h = dynamic_cast<hero*>(current_char);
                    if (h) {
                        gamelogs.push_back("Deck Info: " + to_string(h->getdeck().getsize()) + " cards left.");
                    }
                } 
                else if (cmd == "log") {
                    gamelogs.push_back("Action Log: Showing active game telemetry records below.");
                } 
                else if (cmd == "help") {
                    screen_mode = 1; 
                } 
                else if (cmd == "quit") {
                    screen_mode = 0; 
                    gamelogs.push_back("System: Match paused. Returned to menu.");
                } 
                else {
                    gamelogs.push_back("Unknown Command Syntax: '" + cmd + "'");
                }
                return true;
            }
        }
        return false;
    });

    Components tabs = {menu_event_handler, help_event_handler, gameplay_event_handler};
    auto main_container = Container::Tab(tabs, &screen_mode);

    screen.Loop(main_container);
}