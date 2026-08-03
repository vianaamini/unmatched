// ==================== tui_controller.cpp ====================
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
#include <sstream>

using namespace std;
using namespace ftxui;

TuiController::TuiController()
{
    gamelogs.push_back("System: TUI initialized");
    gamelogs.push_back("System: Ready for Unmatched game");

    std::cout << "=== Choosing who picks their fighter first ===" << std::endl;
    std::cout << "Enter age of Player 1 (between 12 and 70): ";
    std::cin >> age1;
    while (age1 < 12 || age1 > 70)
    {
        std::cout << "Invalid age! Age must be between 12 and 70.\n"
                  << "Enter age of Player 1 again: ";
        std::cin >> age1;
    }

    std::cout << "Enter age of Player 2 (between 12 and 70): ";
    std::cin >> age2;
    while (age2 < 12 || age2 > 70)
    {
        std::cout << "Invalid age! Age must be between 12 and 70. Enter age of Player 2 again: ";
        std::cin >> age2;
    }
    if (age1 < age2)
    {
        cout << "Player 1 is younger and chooses first!" << endl;
        firstPlayer = 1;
    }
    else if (age2 < age1)
    {
        cout << "Player 2 is younger and chooses first!" << endl;
        firstPlayer = 2;
    }
    else
    {
        cout << "Both players have the same age. Player 1 chooses first by default." << endl;
        firstPlayer = 1;
    }

    cout << "\nPlayer " << firstPlayer << ", choose your fighter:" << endl;
    cout << "1. Sherlock Holmes" << endl;
    cout << "2. Dracula" << endl;
    cout << "Enter your choice (1 or 2): ";
    cin >> choice;
    while (choice != 1 && choice != 2)
    {
        cout << "Invalid choice. Enter 1 or 2: ";
        cin >> choice;
    }

    string firstFighterName = (choice == 1) ? "Sherlock Holmes" : "Dracula";
    string secondFighterName = (choice == 1) ? "Dracula" : "Sherlock Holmes";
    cout << "\nPlayer " << firstPlayer << " picked " << firstFighterName << "." << endl;
    cout << "The other player automatically gets " << secondFighterName << "." << endl;

    cout << "\nPress Enter to continue to the game screen...";
    cin.ignore();
    cin.get();

    setupCharacters();
    gamelogs.push_back("Player " + to_string(firstPlayer) + " starts the game!");
}

void TuiController::setupCharacters()
{
    sherlock *sherlockChar = new sherlock();
    watson *watsonChar = new watson();
    dracula *draculaChar = new dracula();
    sister *sister1 = new sister(1);
    sister *sister2 = new sister(2);
    sister *sister3 = new sister(3);

    Deployment deployment(&gamemanager.getBoard());

    vector<character *> sherlockSidekicks;
    sherlockSidekicks.push_back(watsonChar);
    auto sherlockResult = deployment.placeHeroWithSidekicks(sherlockChar, sherlockSidekicks, 4);

    vector<character *> draculaSidekicks;
    draculaSidekicks.push_back(sister1);
    draculaSidekicks.push_back(sister2);
    draculaSidekicks.push_back(sister3);
    auto draculaResult = deployment.placeHeroWithSidekicks(draculaChar, draculaSidekicks, 18);

    if (choice == 1)
    {
        gamemanager.addCharacter(sherlockChar, 1);
        gamemanager.addCharacter(watsonChar, 1);
        gamemanager.addCharacter(draculaChar, 2);
        gamemanager.addCharacter(sister1, 2);
        gamemanager.addCharacter(sister2, 2);
        gamemanager.addCharacter(sister3, 2);
    }
    else
    {
        gamemanager.addCharacter(draculaChar, 1);
        gamemanager.addCharacter(sister1, 1);
        gamemanager.addCharacter(sister2, 1);
        gamemanager.addCharacter(sister3, 1);
        gamemanager.addCharacter(sherlockChar, 2);
        gamemanager.addCharacter(watsonChar, 2);
    }
    sherlockChar->drawhand();
    draculaChar->drawhand();

    gamelogs.push_back("Characters deployed successfully.");
    if (sherlockResult.success)
    {
        gamelogs.push_back("Sherlock at n" + to_string(sherlockChar->getposition()) +
                           ", Watson at n" + to_string(watsonChar->getposition()));
    }
    else
    {
        gamelogs.push_back("Sherlock team deploy failed: " + sherlockResult.message);
    }
    if (draculaResult.success)
    {
        gamelogs.push_back("Dracula at n" + to_string(draculaChar->getposition()) +
                           ", Sisters at n" + to_string(sister1->getposition()) +
                           ", n" + to_string(sister2->getposition()) +
                           ", n" + to_string(sister3->getposition()));
    }
    else
    {
        gamelogs.push_back("Dracula team deploy failed: " + draculaResult.message);
    }
}

Element TuiController::createDynamicNode(const string &nodename,
                                         const string &dracPos,
                                         const string &sherlockPos,
                                         const string &watsonPos,
                                         const string &s1Pos,
                                         const string &s2Pos,
                                         const string &s3Pos)
{
    NodeColor colorEnum = gameMap.getNodeColorByName(nodename);
    Color nodeZoneColor = Color::Default;
    switch (colorEnum)
    {
    case NodeColor::YELLOW:
        nodeZoneColor = Color::Yellow;
        break;
    case NodeColor::GREEN:
        nodeZoneColor = Color::Green;
        break;
    case NodeColor::BLUE:
        nodeZoneColor = Color::Blue;
        break;
    case NodeColor::RED:
        nodeZoneColor = Color::Red;
        break;
    case NodeColor::PURPLE:
        nodeZoneColor = Color::Magenta;
        break;
    case NodeColor::BROWN:
        nodeZoneColor = Color::White;
        break;
    default:
        nodeZoneColor = Color::Default;
        break;
    }

    vector<Element> indicators;
    indicators.push_back(text(nodename) | bold);

    if (nodename == dracPos)
        indicators.push_back(text(" [D]") | color(Color::Red) | bold);
    if (nodename == sherlockPos)
        indicators.push_back(text(" [S]") | color(Color::Blue) | bold);
    if (nodename == watsonPos)
        indicators.push_back(text(" [W]") | color(Color::Cyan) | bold);
    if (nodename == s1Pos)
        indicators.push_back(text(" [s1]") | color(Color::White) | bold);
    if (nodename == s2Pos)
        indicators.push_back(text(" [s2]") | color(Color::White) | bold);
    if (nodename == s3Pos)
        indicators.push_back(text(" [s3]") | color(Color::White) | bold);

    return hbox(move(indicators)) | center | color(nodeZoneColor);
}

Element TuiController::drawExactGraphMap()
{
    string dPos = "", sPos = "", wPos = "", s1 = "", s2 = "", s3 = "";
    auto allChars = gamemanager.getAllCharacters();
    for (character *c : allChars)
    {
        if (c->getname() == "Dracula")
            dPos = c->getPositionString();
        else if (c->getname() == "Sherlock Holmes")
            sPos = c->getPositionString();
        else if (c->getname() == "Watson")
            wPos = c->getPositionString();
        else if (c->getname() == "Sister 1")
            s1 = c->getPositionString();
        else if (c->getname() == "Sister 2")
            s2 = c->getPositionString();
        else if (c->getname() == "Sister 3")
            s3 = c->getPositionString();
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
               hbox({text("         "), n1, text("─────────"), n2, text("─────────"), n3, text("         ")}) | center,
               hbox({text("         ╱    ╲         │             ╲          ")}) | center,
               hbox({text("  "), n4, text("────"), n5, text("────"), n6, text("────"), n7, text("────"), n8, text("    ")}) | center,
               hbox({text("       ╱             ╲   │   ╱           ╲        ")}) | center,
               hbox({text(" "), n9, text("────"), n10, text("───────"), n11, text("─────"), n12, text("─────"), n13, text("   ")}) | center,
               hbox({text("       ╲              ╱  │ ╲             ╱        ")}) | center,
               hbox({text("   "), n14, text("────"), n15, text("────"), n16, text("────"), n17, text("────"), n18, text("   ")}) | center,
               hbox({text("          ╲         ╱    │      ╲       ╱          ")}) | center,
               hbox({text("         "), n19, text("────────────"), n20, text("───────────"), n21, text("         ")}) | center,
           }) |
           center | flex;
}

Element TuiController::createHealthBar(int current, int max)
{
    int barLength = 10;
    int filled = (current * barLength) / max;
    string bar = "[";
    for (int i = 0; i < barLength; i++)
    {
        if (i < filled)
            bar += "#";
        else
            bar += " ";
    }
    bar += "]";
    return text(bar) | color(Color::Green);
}

Element TuiController::createHeroPanel(character *c, const string &title, Color titleColor)
{
    if (!c)
        return text("") | border;

    Elements info;
    info.push_back(text(" " + title + " ") | bold | color(titleColor) | center);
    info.push_back(separator());

    if (!c->isalive())
    {
        info.push_back(text(" [ DEAD ] ") | bold | color(Color::Red) | center);
    }
    else
    {
        info.push_back(text("HP: " + to_string(c->gethealth()) + " / " + to_string(c->getMaxHp())) | center);
        info.push_back(createHealthBar(c->gethealth(), c->getMaxHp()) | center);
        info.push_back(text("Pos: " + c->getPositionString()) | center);
    }

    hero *h = dynamic_cast<hero *>(c);
    if (h)
    {
        info.push_back(text("Deck: " + to_string(h->getdeck().getsize())) | center);
        info.push_back(text("Actions: " + to_string(h->get_actions())) | center);
    }

    return vbox(move(info)) | border | size(WIDTH, EQUAL, 20);
}

Element TuiController::createHandPanel(hero *h, const string &title, Color titleColor)
{
    if (!h)
        return text("") | border;

    Elements headerElements;
    headerElements.push_back(text(" " + title + " - HAND (" + to_string(h->handsize()) + "/5) ") | bold | color(titleColor) | center);
    headerElements.push_back(separator());

    Elements cardBoxes;
    auto hand = h->gethand();
    for (size_t i = 0; i < hand.size() && i < 5; i++)
    {
        string typeStr;
        switch (hand[i].gettype())
        {
        case cardtype::attack:
            typeStr = "ATTACK";
            break;
        case cardtype::defense:
            typeStr = "DEFENSE";
            break;
        case cardtype::scheme:
            typeStr = "SCHEME";
            break;
        default:
            typeStr = "MULTI";
            break;
        }

        string valueLine;
        if (hand[i].gettype() == cardtype::attack)
        {
            valueLine = "ATK: " + to_string(hand[i].getattack());
        }
        else if (hand[i].gettype() == cardtype::defense)
        {
            valueLine = "DEF: " + to_string(hand[i].getdefense());
        }
        else
        {
            valueLine = "BST: " + to_string(hand[i].getboost());
        }

        Elements cardInfo;
        cardInfo.push_back(text("[" + to_string(i + 1) + "]") | bold | center);
        cardInfo.push_back(text(hand[i].get_name()) | bold | center);
        cardInfo.push_back(text(typeStr) | color(Color::Yellow) | center);
        cardInfo.push_back(text(valueLine) | center);
        string effect = hand[i].geteffect();
        if (effect.length() > 20)
            effect = effect.substr(0, 18) + "...";
        cardInfo.push_back(text(effect) | dim | center);

        cardBoxes.push_back(vbox(cardInfo) | border | size(WIDTH, EQUAL, 18));
    }

    Elements panelElements = headerElements;
    panelElements.push_back(hbox(move(cardBoxes)));

    return vbox(move(panelElements)) | border;
}

Element TuiController::createActionLog()
{
    Elements logElements;
    logElements.push_back(text(" ACTION LOG ") | bold | color(Color::Yellow) | center);
    logElements.push_back(separator());

    int startIdx = max(0, (int)gamelogs.size() - 8);
    for (size_t i = startIdx; i < gamelogs.size(); i++)
    {
        string prefix = (i == gamelogs.size() - 1) ? "► " : "  ";
        logElements.push_back(text(prefix + gamelogs[i]) | dim);
    }

    return vbox(logElements) | border | size(HEIGHT, EQUAL, 10);
}

Element TuiController::createCommandsPanel()
{
    return vbox({text(" COMMANDS ") | bold | color(Color::Yellow) | center,
                 separator(),
                 text("move <n> - Move active hero"),
                 text("move <who> <n> (e.g. move watson n1)"),
                 text("play <idx> - Play card"),
                 text("end - End turn"),
                 text("hand - Show hand"),
                 text("deck - Show deck size"),
                 text("log - Show full log"),
                 text("help - Help menu"),
                 text("quit - Exit game")}) |
           border | size(WIDTH, EQUAL, 32);
}

Element TuiController::createLocationInfo()
{
    character *current = gamemanager.getCurrentCharacter();
    string loc = current ? current->getPositionString() : "Unknown";

    string connected = "";
    if (current)
    {
        auto neighbors = gamemanager.getBoard().getNeighborIds(current->getx());
        for (int n : neighbors)
        {
            if (!connected.empty())
                connected += ", ";
            connected += "n" + to_string(n);
        }
        if (connected.empty())
            connected = "None";

        string currentName = "n" + to_string(current->getx());
        if (gamemanager.getBoard().isTeleport(currentName))
        {
            string dest = gamemanager.getBoard().getTeleportDestination(currentName);
            if (!connected.empty())
                connected += ", ";
            connected += "~" + dest + " (Teleport)";
        }
    }

    TurnPhase phase = gamemanager.getCurrentPhase();
    string phaseName;
    switch (phase)
    {
    case TurnPhase::MANEUVER:
        phaseName = "MANEUVER";
        break;
    case TurnPhase::SCHEME:
        phaseName = "SCHEME";
        break;
    case TurnPhase::ATTACK:
        phaseName = "ATTACK";
        break;
    case TurnPhase::END:
        phaseName = "END";
        break;
    default:
        phaseName = "DRAW";
        break;
    }

    return vbox({text(" INFO ") | bold | color(Color::Cyan) | center,
                 separator(),
                 text("Location: " + loc),
                 text("Connected: " + connected),
                 text("Phase: " + phaseName),
                 text("Actions: " + to_string(gamemanager.getActionsRemaining()))}) |
           border | size(WIDTH, EQUAL, 25);
}

Element TuiController::createLegend()
{
    return vbox({text(" LEGEND ") | bold | color(Color::Yellow) | center,
                 separator(),
                 text("[D] Dracula") | color(Color::Red),
                 text("[S] Sherlock") | color(Color::Blue),
                 text("[W] Watson") | color(Color::Cyan),
                 text("[s1-3] Sisters") | color(Color::White),
                 text("--- Normal edge"),
                 text("~~~ Teleport")}) |
           border | size(WIDTH, EQUAL, 20);
}

Element TuiController::createStatusDashboard()
{
    auto allChars = gamemanager.getAllCharacters();
    Elements heroPanels;

    for (character *c : allChars)
    {
        if (!c)
            continue;
        hero *h = dynamic_cast<hero *>(c);

        Elements info;
        info.push_back(text(c->getname()) | bold | color(Color::Yellow));
        info.push_back(separator());
        info.push_back(text("HP: " + to_string(c->gethealth()) + "/" + to_string(c->getMaxHp())) | color(c->isalive() ? Color::Green : Color::Red));
        info.push_back(text("Pos: " + c->getPositionString()));

        if (h)
        {
            info.push_back(text("Hand: " + to_string(h->handsize()) + "/5"));
            info.push_back(text("Deck: " + to_string(h->getdeck().getsize())));
            info.push_back(text("Actions: " + to_string(h->get_actions())));
        }

        heroPanels.push_back(vbox(move(info)) | border | size(WIDTH, EQUAL, 18));
    }

    character *current = gamemanager.getCurrentCharacter();

    Elements logElements;
    int start_idx = max(0, (int)gamelogs.size() - 4);
    for (size_t i = start_idx; i < gamelogs.size(); ++i)
    {
        logElements.push_back(text("> " + gamelogs[i]));
    }

    return vbox({text(" STATUS ") | bold | color(Color::Cyan) | center,
                 separator(),
                 text("Turn: " + to_string(gamemanager.getTurnNumber()) +
                      "  |  Team: " + to_string(gamemanager.getCurrentTeam())) |
                     center,
                 separator(),
                 hbox(move(heroPanels)) | center,
                 separator(),
                 text("Current: " + (current ? current->getname() : "N/A")) | bold | color(Color::Magenta) | center,
                 separator(),
                 text(" LOG ") | bold | color(Color::Yellow),
                 vbox(move(logElements)) | border | size(HEIGHT, EQUAL, 5)}) |
           border;
}

void TuiController::processCommand(const std::string &cmd)
{
    if (cmd == "help")
    {
        screenMode = 1;
        gamelogs.push_back("Help menu opened");
    }
    else if (cmd == "quit")
    {
        screenMode = 0;
        gamelogs.push_back("Returned to main menu");
    }
    else if (cmd.rfind("move ", 0) == 0)
    {
        try
        {
            stringstream ss(cmd);
            string moveCmd, p1, p2;
            ss >> moveCmd >> p1;

            character *charToMove = nullptr;
            int targetNodeId = -1;

            if (ss >> p2)
            {
                string targetStr = p2;
                if (!targetStr.empty() && targetStr[0] == 'n')
                    targetStr = targetStr.substr(1);
                targetNodeId = stoi(targetStr);

                string nameLower = p1;
                transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);

                for (character *c : gamemanager.getAllCharacters())
                {
                    string cName = c->getname();
                    transform(cName.begin(), cName.end(), cName.begin(), ::tolower);

                    if ((nameLower == "watson" || nameLower == "w") && cName.find("watson") != string::npos)
                    {
                        charToMove = c;
                        break;
                    }
                    if ((nameLower == "s1" || nameLower == "sister1") && cName == "sister 1")
                    {
                        charToMove = c;
                        break;
                    }
                    if ((nameLower == "s2" || nameLower == "sister2") && cName == "sister 2")
                    {
                        charToMove = c;
                        break;
                    }
                    if ((nameLower == "s3" || nameLower == "sister3") && cName == "sister 3")
                    {
                        charToMove = c;
                        break;
                    }
                    if ((nameLower == "sherlock" || nameLower == "s") && cName.find("sherlock") != string::npos)
                    {
                        charToMove = c;
                        break;
                    }
                    if ((nameLower == "dracula" || nameLower == "d") && cName.find("dracula") != string::npos)
                    {
                        charToMove = c;
                        break;
                    }
                }
            }
            else
            {
                string targetStr = p1;
                if (!targetStr.empty() && targetStr[0] == 'n')
                    targetStr = targetStr.substr(1);
                targetNodeId = stoi(targetStr);
                charToMove = gamemanager.getCurrentCharacter();
            }

            if (!charToMove || !charToMove->isalive())
            {
                gamelogs.push_back("Character not found or dead!");
                return;
            }

            string targetName = "n" + to_string(targetNodeId);
            bool success = gamemanager.moveCharacter(charToMove, targetName, nullptr);
            if (success)
            {
                gamelogs.push_back(charToMove->getname() + " moved to node " + targetName);
            }
            else
            {
                gamelogs.push_back("Move failed! Check adjacency or actions.");
            }
        }
        catch (...)
        {
            gamelogs.push_back("Syntax Error: Use 'move <node>' or 'move <target> <node>'");
        }
    }
    else if (cmd.rfind("play ", 0) == 0)
    {
        try
        {
            int card_idx = stoi(cmd.substr(5));
            character *current_char = gamemanager.getCurrentCharacter();
            hero *h = dynamic_cast<hero *>(current_char);

            if (h && card_idx >= 1 && card_idx <= h->handsize())
            {
                card playedCard = h->gethand()[card_idx - 1];

                hero *opponent = nullptr;
                for (character *c : gamemanager.getAllCharacters())
                {
                    hero *otherHero = dynamic_cast<hero *>(c);
                    if (otherHero && otherHero != h)
                    {
                        opponent = otherHero;
                        break;
                    }
                }

                bool actionSuccess = false;
                Board &board = gamemanager.getBoard();

                if (playedCard.gettype() == cardtype::attack && opponent)
                {
                    actionSuccess = h->attack(*opponent, playedCard, board);
                    if (actionSuccess)
                    {
                        gamelogs.push_back(h->getname() + " attacked " + opponent->getname() +
                                           " with " + playedCard.get_name());
                        if (!opponent->isalive())
                        {
                            gamelogs.push_back(opponent->getname() + " has been defeated!");
                            gamemanager.removeCharacter(opponent);
                        }
                        gamemanager.getTurnManager().endTurn();
                    }
                    else
                    {
                        gamelogs.push_back("Attack failed! (not adjacent)");
                    }
                }
                else if (playedCard.gettype() == cardtype::scheme && opponent)
                {
                    actionSuccess = h->scheme(playedCard, *opponent);
                    if (actionSuccess)
                    {
                        gamelogs.push_back(h->getname() + " played scheme: " + playedCard.get_name());
                        gamemanager.getTurnManager().endTurn();
                    }
                    else
                    {
                        gamelogs.push_back("Scheme failed!");
                    }
                }
                else if (playedCard.gettype() == cardtype::defense ||
                         playedCard.gettype() == cardtype::multipurpose)
                {
                    actionSuccess = true;
                    gamelogs.push_back(h->getname() + " played: " + playedCard.get_name());
                    h->useAction();
                    gamemanager.getTurnManager().endTurn();
                }

                if (actionSuccess)
                {
                    h->gethand().erase(h->gethand().begin() + card_idx - 1);
                }
            }
            else
            {
                gamelogs.push_back("Invalid card index!");
            }
        }
        catch (...)
        {
            gamelogs.push_back("Syntax Error: Use 'play <card_index>'");
        }
    }
    else if (cmd == "end")
    {
        character *current_char = gamemanager.getCurrentCharacter();
        hero *h = dynamic_cast<hero *>(current_char);
        if (h)
        {
            h->reset_actions();
        }
        gamemanager.nextTurn();
        gamelogs.push_back("Turn ended");
    }
    else if (cmd == "hand")
    {
        character *current_char = gamemanager.getCurrentCharacter();
        hero *h = dynamic_cast<hero *>(current_char);
        if (h)
        {
            gamelogs.push_back("--- CURRENT HAND ---");
            auto current_hand = h->gethand();
            for (size_t i = 0; i < current_hand.size(); ++i)
            {
                gamelogs.push_back("[" + to_string(i + 1) + "] " + current_hand[i].get_name());
            }
        }
    }
    else if (cmd == "deck")
    {
        character *current_char = gamemanager.getCurrentCharacter();
        hero *h = dynamic_cast<hero *>(current_char);
        if (h)
        {
            gamelogs.push_back("Deck: " + to_string(h->getdeck().getsize()) + " cards left");
        }
    }
    else if (cmd == "log")
    {
        for (const auto &log : gamelogs)
        {
            cout << log << endl;
        }
    }
    else if (!cmd.empty())
    {
        gamelogs.push_back("Unknown command: " + cmd);
    }

    if (gamemanager.isGameOver())
    {
        screenMode = 3;
        character *winner = gamemanager.getWinner();
        if (winner)
        {
            gamelogs.push_back("GAME OVER! " + winner->getname() + " wins!");
        }
    }
}

void TuiController::run()
{
    auto screen = ScreenInteractive::Fullscreen();

    int screen_mode = 0;
    int menu_selected = 0;

    vector<string> menu_entries = {
        "   [ Play Game ]   ",
        "   [ Help & Rules ]",
        "   [ Exit Game ]   "};

    auto menu_components = Menu(&menu_entries, &menu_selected);

    auto menu_renderer = Renderer(menu_components, [&]
                                  { return vbox({text("┌────────────────────────────────────────────┐") | center | bold | color(Color::Green),
                                                 text("│               UNMATCHED MAP                │") | center | bold | color(Color::Green),
                                                 text("└────────────────────────────────────────────┘") | center | bold | color(Color::Green),
                                                 text("1. Main Navigation Menu") | center | bold,
                                                 separator(),
                                                 vbox({menu_components->Render() | center}) | center,
                                                 separator(),
                                                 text("Use UP/DOWN Arrows • Press ENTER to Select") | center | color(Color::Yellow)}) |
                                           center | border; });

    auto menu_event_handler = CatchEvent(menu_renderer, [&](Event event)
                                         {
        if (event == Event::Return) {
            if (menu_selected == 0) {
                screen_mode = 2;
                gamemanager.startGame(1);
                gamelogs.push_back("Game started!");
            } else if (menu_selected == 1) {
                screen_mode = 1;
            } else if (menu_selected == 2) {
                screen.ExitLoopClosure()();
            }
            return true;
        }
        return false; });

    auto help_renderer = Renderer([&]
                                  { return vbox({text("  HOW TO PLAY  ") | center | bold | color(Color::Cyan),
                                                 separator(),
                                                 paragraph("Move your hero across the graph network.") | center,
                                                 paragraph("Each turn you have 2 actions:") | center,
                                                 paragraph("MANEUVER: Move to adjacent nodes") | center | color(Color::Blue),
                                                 paragraph("SCHEME: Play scheme cards") | center | color(Color::Magenta),
                                                 paragraph("ATTACK: Attack adjacent enemies") | center | color(Color::Red),
                                                 separator(),
                                                 vbox({text("Commands:"),
                                                       text("move <node>          - Move active hero (e.g., move n14)"),
                                                       text("move <target> <node> - Move sidekick (e.g., move watson n1)"),
                                                       text("play <idx>           - Play card from hand"),
                                                       text("end                  - End turn"),
                                                       text("hand                 - Show hand"),
                                                       text("deck                 - Show deck size"),
                                                       text("log                  - Show full log"),
                                                       text("help                 - This menu"),
                                                       text("quit                 - Exit to menu")}) |
                                                     border,
                                                 separator(),
                                                 text("Press [ ENTER ] to return to Main Menu") | center | color(Color::Yellow)}) |
                                           center | border; });

    auto help_event_handler = CatchEvent(help_renderer, [&](Event event)
                                         {
        if (event == Event::Return) {
            screen_mode = 0;
            return true;
        }
        return false; });

    auto gameover_renderer = Renderer([&]
                                      {
        character* winner = gamemanager.getWinner();
        string winnerName = winner ? winner->getname() : "Unknown";
        return vbox({
            text("╔══════════════════════════════╗") | center | bold | color(Color::Red),text("║          GAME OVER            ║") | center | bold | color(Color::Red),
            text("╚══════════════════════════════╝") | center | bold | color(Color::Red),
            separator(),
            text(winnerName + " WINS!") | center | bold | color(Color::Yellow),
            separator(),
            text("Press [ ENTER ] to return to Main Menu") | center | color(Color::Cyan)
        }) | center | border; });

    auto gameover_event_handler = CatchEvent(gameover_renderer, [&](Event event)
                                             {
        if (event == Event::Return) {
            screen_mode = 0;
            return true;
        }
        return false; });

    string input_command = "";
    Component input_box = Input(&input_command, "Enter command...");

    input_box |= CatchEvent([&](Event event)
                            {
        if (event == Event::Return && !input_command.empty()) {
            processCommand(input_command);
            input_command = "";
            screen.PostEvent(Event::Custom);
            return true;
        }
        return false; });

    auto gameplay_renderer = Renderer(input_box, [&]
                                      {
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

        auto mapElement = drawExactGraphMap() | flex_grow;

        character* watson = nullptr;
        character* s1 = nullptr;
        character* s2 = nullptr;
        character* s3 = nullptr;

        for (character* c : gamemanager.getAllCharacters()) {
            if (!c) continue;
            if (c->getname() == "Watson") watson = c;
            else if (c->getname() == "Sister 1") s1 = c;
            else if (c->getname() == "Sister 2") s2 = c;
            else if (c->getname() == "Sister 3") s3 = c;
        }

        auto draculaGroup = vbox({
            createHeroPanel(dracula, "DRACULA", Color::Red),
            createHeroPanel(s1, "SISTER 1", Color::RedLight),
            createHeroPanel(s2, "SISTER 2", Color::RedLight),
            createHeroPanel(s3, "SISTER 3", Color::RedLight),
        });

        auto sherlockGroup = vbox({
            createHeroPanel(sherlock, "SHERLOCK", Color::Blue),
            createHeroPanel(watson, "WATSON", Color::Cyan),
        });

        auto topRow = hbox({
            draculaGroup,
            mapElement,
            sherlockGroup
        });

        auto handRow = hbox({
            createHandPanel(draculaHero, "DRACULA", Color::Red) | flex,
            createLegend() | flex,
            createHandPanel(sherlockHero, "SHERLOCK", Color::Blue) | flex
        });

        auto bottomRow = hbox({
            createActionLog() | flex,
            createCommandsPanel() | flex,
            createLocationInfo() | flex
        });

        auto inputLine = hbox({
            text(" Enter command: ") | bold | color(Color::Cyan),
            input_box->Render() | flex
        }) | border;

        auto mainLayout = vbox({
            text(" UNMATCHED TUI - Dracula vs Sherlock Holmes ") | bold | color(Color::Green) | center,
            separator(),
            createStatusDashboard() | size(HEIGHT, EQUAL, 12),
            separator(),
            topRow,
            separator(),
            handRow,
            separator(),
            bottomRow,
            inputLine
        });

        return mainLayout | flex; });

    Components tabs = {menu_event_handler, help_event_handler, gameplay_renderer, gameover_event_handler};
    auto main_container = Container::Tab(tabs, &screen_mode);

    screen.Loop(main_container);
}