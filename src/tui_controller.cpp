#include "tui_controller.hpp"
#include "sherlock.hpp"
#include "dracula.hpp"
#include "watson.hpp"
#include "sister.hpp"
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
    setupCharacters();
}

void TuiController::setupCharacters() {
    sherlock* sherlockChar = new sherlock();
    watson* watsonChar = new watson();
    dracula* draculaChar = new dracula();
    sister* sister1 = new sister(1);
    sister* sister2 = new sister(2);
    sister* sister3 = new sister(3);

    Deployment deployment(&gamemanager.getBoard());
    deployment.placeHeroWithSidekicks(sherlockChar, {watsonChar}, 4, 0);
    deployment.placeHeroWithSidekicks(draculaChar, {sister1, sister2, sister3}, 18, 0);

    gamemanager.addCharacter(sherlockChar, 1);
    gamemanager.addCharacter(watsonChar, 1);
    gamemanager.addCharacter(draculaChar, 2);
    gamemanager.addCharacter(sister1, 2);
    gamemanager.addCharacter(sister2, 2);
    gamemanager.addCharacter(sister3, 2);

    sherlockChar->drawhand();
    draculaChar->drawhand();

    gamelogs.push_back("Characters deployed: Sherlock, Watson vs Dracula, Sisters");
}

// ============================================================
// توابع نقشه
// ============================================================

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

// ============================================================
// توابع جدید
// ============================================================

Element TuiController::createHealthBar(int current, int max) {
    int barLength = 10;
    int filled = (current * barLength) / max;
    string bar = "[";
    for (int i = 0; i < barLength; i++) {
        if (i < filled) bar += "#";
        else bar += " ";
    }
    bar += "]";
    return text(bar) | color(Color::Green);
}

Element TuiController::createCardDisplay(const card& c, const string& type, const string& range, const string& effect, int index) {
    Color typeColor;
    if (type == "ATTACK") typeColor = Color::Red;
    else if (type == "DEFENSE") typeColor = Color::Blue;
    else if (type == "SCHEME") typeColor = Color::Magenta;
    else typeColor = Color::Yellow;

    return vbox({
        text("  [" + to_string(index) + "] " + c.get_name()) | bold | color(Color::Cyan),
        text("    " + type) | color(typeColor),
        text("    Range " + range),
        text("    " + effect) | dim
    }) | border | size(WIDTH, EQUAL, 28);
}

Element TuiController::createHeroPanel(character* c, const string& title, Color titleColor) {
    if (!c) return text("") | border;

    Elements info;
    info.push_back(text(" " + title + " ") | bold | color(titleColor) | center);
    info.push_back(separator());
    info.push_back(text("Health: " + to_string(c->gethealth()) + " / " + to_string(c->gethealth())) | center);
    info.push_back(createHealthBar(c->gethealth(), c->gethealth()) | center);
    info.push_back(text("Range: 1") | center);
    info.push_back(text("Move: " + to_string(c->getmovement())) | center);
    
    hero* h = dynamic_cast<hero*>(c);
    if (h) {
        info.push_back(text("Deck (Draw Pile): " + to_string(h->getdeck().getsize())) | center);
        info.push_back(text("Discard Pile: 0") | center);
    }

    return vbox(move(info)) | border | size(WIDTH, EQUAL, 25);
}


 Element TuiController::createHandPanel(hero* h, const string& title, Color titleColor) {
    if (!h) return text("") | border;

    Elements headerElements;
    headerElements.push_back(text(" " + title + " - HAND (" + to_string(h->handsize()) + "/5) ") | bold | color(titleColor) | center);
    headerElements.push_back(separator());

    Elements cardBoxes;
    auto hand = h->gethand();
    for (size_t i = 0; i < hand.size() && i < 5; i++) {
        string typeStr;
        string effect = hand[i].geteffect();

        switch (hand[i].gettype()) {
            case cardtype::attack: typeStr = "ATTACK"; break;
            case cardtype::defense: typeStr = "DEFENSE"; break;
            case cardtype::scheme: typeStr = "SCHEME"; break;
            default: typeStr = "MULTIPURPOSE"; break;
        }

        string valueLine;
        if (hand[i].gettype() == cardtype::attack) {
            valueLine = "Deal " + to_string(hand[i].getattack()) + " dmg.";
        } else if (hand[i].gettype() == cardtype::defense) {
            valueLine = "Block " + to_string(hand[i].getdefense()) + " dmg.";
        } else {
            valueLine = "Boost " + to_string(hand[i].getboost());
        }

        Elements cardInfo;
        cardInfo.push_back(text("[" + typeStr + "]") | color(Color::Yellow) | bold | center);
        cardInfo.push_back(text(hand[i].get_name()) | bold | center);
        cardInfo.push_back(text(valueLine) | center);
        cardInfo.push_back(text(effect) | dim | center);

        cardBoxes.push_back(vbox(cardInfo) | border | size(WIDTH, EQUAL, 20));
    }

    Elements panelElements = headerElements;
    panelElements.push_back(hbox(std::move(cardBoxes)));

    return vbox(std::move(panelElements)) | border;
}       

Element TuiController::createActionLog() {
    Elements logElements;
    logElements.push_back(text(" ACTION LOG ") | bold | color(Color::Yellow) | center);
    logElements.push_back(separator());

    int startIdx = max(0, (int)gamelogs.size() - 8);
    for (size_t i = startIdx; i < gamelogs.size(); i++) {
        string prefix = (i == gamelogs.size() - 1) ? "► " : "  ";
        logElements.push_back(text(prefix + gamelogs[i]) | dim);
    }

    return vbox(logElements) | border | size(HEIGHT, EQUAL, 10);
}

Element TuiController::createMapDisplay() {
    vector<string> mapLines = {
        "       n1 ─── n2 ─── n3",
        "        ╲     │     ╱",
        "   n4 ─── n5 ─── n6 ─── n7 ─── n8",
        "    │     ╲   │   ╱     │",
        "    │  n9 ─── n10 ─── n11  │",
        "    │   │    ╲ │ ╱    │   │",
        "   n12─n13── n14 ── n15─n16",
        "    │   │    ╱ │ ╲    │   │",
        "    │  n17── n18 ── n19  │",
        "    │     ╱   │   ╲     │",
        "   n20── n21 ── n22 ── n23",
        "        ╱     │     ╲",
        "       n24 ─── n25 ─── n26"
    };

    std::string dPos = "", sPos = "", wPos = "";
    auto allChars = gamemanager.getAllCharacters();
    for (character* c : allChars) {
        if (c->getname() == "Dracula") dPos = c->getPositionString();
        else if (c->getname() == "Sherlock Holmes") sPos = c->getPositionString();
        else if (c->getname() == "Watson") wPos = c->getPositionString();
    }

    Elements mapElements;
    mapElements.push_back(text(" MAP (Graph) ") | bold | color(Color::Cyan) | center);
    mapElements.push_back(separator());

    for (const auto& line : mapLines) {
        string displayLine = line;
        size_t pos;
        
        if ((pos = displayLine.find("n1")) != string::npos) {
            if (dPos == "n1") displayLine.replace(pos, 2, "D");
            else if (sPos == "n1") displayLine.replace(pos, 2, "S");
            else if (wPos == "n1") displayLine.replace(pos, 2, "W");
        }
        if ((pos = displayLine.find("n2")) != string::npos) {
            if (dPos == "n2") displayLine.replace(pos, 2, "D");
            else if (sPos == "n2") displayLine.replace(pos, 2, "S");
            else if (wPos == "n2") displayLine.replace(pos, 2, "W");
        }
        // ... ادامه برای سایر گره‌ها (به صورت مشابه)

        mapElements.push_back(text(displayLine));
    }

    return vbox(mapElements) | border | size(WIDTH, EQUAL, 30);
}

Element TuiController::createCommandsPanel() {
    return vbox({
        text(" GAME COMMANDS ") | bold | color(Color::Yellow) | center,
        separator(),
        text("move <location>"),
        text("play <card_index>"),
        text("end"),
        text("hand"),
        text("deck"),
        text("log"),
        text("help"),
        text("quit")
    }) | border | size(WIDTH, EQUAL, 20);
}

Element TuiController::createLocationInfo() {
    character* current = gamemanager.getCurrentCharacter();
    string loc = current ? current->getPositionString() : "Unknown";
    string connected = loc.empty() ? "None" : "n7, n10, n12, n15";

    return vbox({
        text(" LOCATION INFO ") | bold | color(Color::Cyan) | center,
        separator(),
        text("Current Location: " + loc + " (Special - ~)"),
        text("Connected to: " + connected),
        text("No active effects.")
    }) | border | size(WIDTH, EQUAL, 30);
}

Element TuiController::createLegend() {
    return vbox({
        text(" LEGEND: ") | bold | color(Color::Yellow),
        text("(D) Dracula"),
        text("(S) Sherlock Holmes"),
        text("( ) Normal Location"),
        text("(##) Obstacle"),
        text("(~) Special Location")
    }) | border | size(WIDTH, EQUAL, 20);
}

Element TuiController::createStatusDashboard() {
    // ... (همان کد قبلی)
    return text("Status Dashboard") | border;
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

void TuiController::processCommand(const std::string& cmd) {
    // ... (همان کد قبلی)
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
                gamelogs.push_back("Game started!");
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
        auto allChars = gamemanager.getAllCharacters();
        character* sherlock = nullptr;
        character* dracula = nullptr;
        hero* sherlockHero = nullptr;
        hero* draculaHero = nullptr;

        for (character* c : allChars) {
            if (c->getname() == "Sherlock Holmes") {
                sherlock = c;
                sherlockHero = dynamic_cast<hero*>(c);
            } else if (c->getname() == "Dracula") {
                dracula = c;
                draculaHero = dynamic_cast<hero*>(c);
            }
        }

        Elements topRowElements = {
            createHeroPanel(dracula, "DRACULA", Color::Red) | flex,
            createMapDisplay() | flex,
            createHeroPanel(sherlock, "SHERLOCK HOLMES", Color::Blue) | flex
        };auto topRow = hbox(std::move(topRowElements));

        Elements handRowElements = {
            createHandPanel(draculaHero, "DRACULA", Color::Red) | flex,
            createLegend() | flex,
            createHandPanel(sherlockHero, "SHERLOCK", Color::Blue) | flex
        };
        auto handRow = hbox(std::move(handRowElements));

        Elements bottomRowElements = {
            createActionLog() | flex,
            createCommandsPanel() | flex,
            createLocationInfo() | flex
        };
        auto bottomRow = hbox(std::move(bottomRowElements));

        auto inputLine = hbox({
            text(" Enter command: ") | bold | color(Color::Cyan),
            input_box->Render() | flex
        }) | border;

        Elements mainElements = {
            text(" UNMATCHED TUI - Dracula vs Sherlock Holmes ") | bold | color(Color::Green) | center,
            separator(),
            text("Turn: " + to_string(gamemanager.getTurnNumber()) + " - " +
                 (gamemanager.getCurrentCharacter() ? gamemanager.getCurrentCharacter()->getname() : "N/A")) | bold | center,
            separator(),
            topRow,
            separator(),
            handRow,
            separator(),
            bottomRow,
            inputLine
        };

        return vbox(std::move(mainElements)) | flex;
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
                            gamelogs.push_back("Moved to node n" + to_string(target_node_id));
                        } else {
                            gamelogs.push_back("Invalid move!");
                        }
                    } catch (...) {
                        gamelogs.push_back("Syntax Error: Use 'move <node_id>'");
                    }
                }

                else if (cmd.rfind("play ", 0) == 0) {
                    try {
                        int card_idx = stoi(cmd.substr(5));
                        character* current_char = gamemanager.getCurrentCharacter();
                        hero* h = dynamic_cast<hero*>(current_char);

                        if (h && card_idx >= 1 && card_idx <= h->handsize()) {

                            card playedCard = h->gethand()[card_idx - 1];

                            hero* opponent = nullptr;
                            for (character* c : gamemanager.getAllCharacters()) {
                                hero* otherHero = dynamic_cast<hero*>(c);
                                if (otherHero && otherHero != h) {
                                    opponent = otherHero;
                                    break;
                                }
                            }

                            bool actionSuccess = false;

                            if (playedCard.gettype() == cardtype::attack && opponent) {
                                actionSuccess = h->attack(*opponent, playedCard, gamemanager.getBoard());

                                if (actionSuccess) {
                                    gamelogs.push_back(
                                        h->getname() + " attacked " +
                                        opponent->getname() + " with " +
                                        playedCard.get_name() + " for " +
                                        to_string(playedCard.getattack()) +" damage!"
                                    );
                                } else {
                                    gamelogs.push_back("Attack failed! (out of range, no actions left, or target down)");
                                }
                            }

                            else if (playedCard.gettype() == cardtype::scheme) {
                                actionSuccess = h->scheme(playedCard, *h);

                                if (actionSuccess) {
                                    gamelogs.push_back(
                                        h->getname() + " played scheme card: " +
                                        playedCard.get_name()
                                    );
                                } else {
                                    gamelogs.push_back("Scheme failed! (no actions left)");
                                }
                            }

                            else {
                                actionSuccess = true;
                                gamelogs.push_back(
                                    h->getname() + " played defense/support card: " +
                                    playedCard.get_name()
                                );
                            }

                            if (actionSuccess) {
                                h->gethand().erase(h->gethand().begin() + card_idx - 1);
                            }

                        } else {
                            gamelogs.push_back("Invalid card index!");
                        }
                    }
                    catch (...) {
                        gamelogs.push_back("Syntax Error: Use 'play <card_index>'");
                    }
                }

                else if (cmd == "end") {
                    gamemanager.nextTurn();
                    character* newCurrent = gamemanager.getCurrentCharacter();
                    hero* newHero = dynamic_cast<hero*>(newCurrent);
                    if (newHero) {
                        newHero->reset_actions();
                    }
                    gamelogs.push_back("Turn ended");
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
                        gamelogs.push_back("Deck: " + to_string(h->getdeck().getsize()) + " cards left");
                    }
                }

                else if (cmd == "log") {
                    for (const auto& log : gamelogs) {
                        std::cout << log << std::endl;
                    }
                }

                else if (cmd == "help") {
                    screen_mode = 1;
                }

                else if (cmd == "quit") {
                    screen_mode = 0;
                    gamelogs.push_back("Returned to menu");
                }

                else {
                    gamelogs.push_back("Unknown command: " + cmd);
                }

                return true;
            }
        }
        return false;
    });

    Components tabs = { menu_event_handler, help_event_handler, gameplay_event_handler };
    auto main_container = Container::Tab(tabs, &screen_mode);

    screen.Loop(main_container);
}