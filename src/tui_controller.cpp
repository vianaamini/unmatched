#include "tui_controller.hpp"
#include <ftxui/component/screen_interactive.hpp>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
using namespace ftxui;

TuiController::TuiController() {
    gamelogs.push_back("system : tui controller initialized");
    gamelogs.push_back("system : ready for unmatched game");
}

Element TuiController::createDynamicNode(const std::string& nodename, const std::string& dracPos, const std::string& sherlockPos, const std::string& watsonPos, const std::string& s1Pos, const std::string& s2Pos, const std::string& s3Pos)  {
    
    NodeColor colorEnum = gameMap.getNodeColorByName(nodename);
    
    Color nodeZoneColor = Color::None;
    switch (colorEnum) {
        case NodeColor::YELLOW: nodeZoneColor = Color::Yellow; break;
        case NodeColor::GREEN:  nodeZoneColor = Color::Green;  break;
        case NodeColor::BLUE:   nodeZoneColor = Color::Blue;   break;
        case NodeColor::RED:    nodeZoneColor = Color::Red;    break;
        case NodeColor::PURPLE: nodeZoneColor = Color::Magenta;break;
        case NodeColor::BROWN:  nodeZoneColor = Color::Gray;   break; 
        default:                nodeZoneColor = Color::None;   break;
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
        if (c->getName() == "Dracula") dPos = c->getPosition();
        else if (c->getName() == "Sherlock") sPos = c->getPosition();
        else if (c->getName() == "Watson") wPos = c->getPosition();
        else if (c->getName() == "Sister1") s1 = c->getPosition();
        else if (c->getName() == "Sister2") s2 = c->getPosition();
        else if (c->getName() == "Sister3") s3 = c->getPosition();
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
    auto n22 = createDynamicNode("n22", dPos, sPos, wPos, s1, s2, s3);
    auto n23 = createDynamicNode("n23", dPos, sPos, wPos, s1, s2, s3);
    auto n24 = createDynamicNode("n24", dPos, sPos, wPos, s1, s2, s3);
    auto n25 = createDynamicNode("n25", dPos, sPos, wPos, s1, s2, s3);
    auto n26 = createDynamicNode("n26", dPos, sPos, wPos, s1, s2, s3);
    auto n27 = createDynamicNode("n27", dPos, sPos, wPos, s1, s2, s3);
    auto n28 = createDynamicNode("n28", dPos, sPos, wPos, s1, s2, s3);
    auto n29 = createDynamicNode("n29", dPos, sPos, wPos, s1, s2, s3);
    auto n30 = createDynamicNode("n30", dPos, sPos, wPos, s1, s2, s3);
    auto n31 = createDynamicNode("n31", dPos, sPos, wPos, s1, s2, s3);
    auto n32 = createDynamicNode("n32", dPos, sPos, wPos, s1, s2, s3);

    return vbox({
        text("─── MAP (Spider Web Graph) ───") | center | bold | color(Color::Green),
        separator(),
        hbox({ text("               "), n1, text("───────────"), n2, text("───────────"), n3, text("               ") }) | center,
        hbox({ text("                ╲              │              ╱                ") }) | center,
        hbox({ text("          "), n4, text("─── "), n9, text("───────"), n10, text("───────"), n11, text(" ───"), n5 }) | center,
        hbox({ text("           │       ╲        │        ╱       │           ") }) | center,
        hbox({ text("           │   "), n12, text("── "), n17, text("───"), n18, text("───"), n19, text(" ── Kish ── "), n13, text("   │           ") }) | center,
        hbox({ text("           │    │       ╲   │   ╱       │    │           ") }) | center,
        hbox({ text("     "), n6, text("──"), n14, text("──"), n20, text("──"), n25, text("[CORE]"), n26, text("──"), n27, text("──"), n21, text("──"), n15, text("──"), n7 }) | center,
        hbox({ text("           │    │       ╱   │   ╲       │    │           ") }) | center,
        hbox({ text("           │   "), n16, text("── "), n22, text("───"), n23, text("───"), n24, text(" ── "), n28, text("   │           ") }) | center,
        hbox({ text("           │       ╱        │        ╲       │           ") }) | center,
        hbox({ text("          "), n29, text("─── "), n30, text("───────"), n31, text("───────"), n32, text(" ───"), n8 }) | center,
        hbox({ text("                ╱           │           ╲                ") }) | center,
        hbox({ text("               "), n5, text("───────────"), n6, text("───────────"), n7, text("               ") }) | center
    }) | center | flex;
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
                screen.ExitLoop(); 
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

        auto status_dashboard = vbox({
            text(" 3. Status Dashboard ") | bold | color(Color::Cyan),
            separator(),
            text("Turn Number: " + to_string(gamemanager.getTurnNumber())),
            text("Actions Left: " + to_string(gamemanager.getActionsRemaining())),
            separator(),
            text("ACTIVE HERO HP: " + to_string(current_char ? current_char->getHP() : 0)) | color(Color::Red) | bold,
            text("Cards in Hand: " + to_string(current_char ? current_char->getHand().size() : 0)),
            text("Status Log: Running normal workflow"),
        }) | border | size(WIDTH, EQUAL, 32);

        auto action_menu_panel = vbox({
            text(" 4. Action Command Examples ") | bold | color(Color::Green),
            separator(),
            hbox({
                text(" [ Discarding Cards ] ") | border | color(Color::Yellow),
                text(" [ Scheme ] ") | border | color(Color::Magenta),
                text(" [ Attack ] ") | border | color(Color::Red),
                text(" [ maneuver ] ") | border | color(Color::Blue)
            }) | center,
            hbox({
                text(" [ Back to main menu ] ") | border | color(Color::White),
                text(" [ Help ] ") | border | color(Color::Cyan),
                text(" [ Drawing Card ] ") | border | color(Color::Green)
            }) | center
        }) | border;

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

        return vbox({
            hbox({ network_map | flex, status_dashboard }),
            action_menu_panel,
            hbox({ commands_guide_panel | flex, log_panel | flex }),
            input_line_panel
        });
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
                        bool success = gamemanager.moveCharacter(current_char, {target_node_id, 0});
                        
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
                        bool success = gamemanager.playCard(current_char, card_idx);
                        
                        if (success) {
                            gamelogs.push_back("Action: Played card from hand at index " + to_string(card_idx));
                        } else {
                            gamelogs.push_back("Error: Play rejected! Insufficient actions or invalid card index.");
                        }
                    } catch (...) {
                        gamelogs.push_back("Syntax Error: Use 'play <card_index>' (e.g., play 1)");
                    }
                } 
                else if (cmd == "end") {
                    gamemanager.endTurn();
                    gamelogs.push_back("Action: Player ended turn.");
                } 
                else if (cmd == "hand") {
                    character* current_char = gamemanager.getCurrentCharacter();
                    gamelogs.push_back("--- CURRENT HAND ---");
                    auto current_hand = current_char->getHand();
                    for (size_t i = 0; i < current_hand.size(); ++i) {
                        gamelogs.push_back("[" + to_string(i + 1) + "] " + current_hand[i].getName());
                    }
                } 
                else if (cmd == "deck") {
                    character* current_char = gamemanager.getCurrentCharacter();
                    gamelogs.push_back("Deck Info: " + to_string(current_char->getDeckSize()) + " cards left in Draw Pile.");
                    gamelogs.push_back("Discard Pile: " + to_string(current_char->getDiscardSize()) + " cards.");
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
                    gamelogs.push_back("Unknown Command Syntax: '" + cmd + "'. Please refer to Game Commands list.");
                }
                return true;
            }
        }
        return false;
    });

    auto main_container = Container::Tab({
        menu_event_handler, 
        help_event_handler, 
        gameplay_event_handler
    }, &screen_mode);

    screen.Loop(main_container);
}