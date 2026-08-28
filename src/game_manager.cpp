#include "../include/game_manager.hpp"
#include "../include/hero.hpp"
#include "../include/dracula.hpp"
#include "../include/invisible_man.hpp"
#include "../include/card.hpp"
#include <algorithm>
#include <iostream>
#include <cctype>
#include <fstream>
#include <string>
#include <vector>

GameManager::GameManager() : board(), movement(&board) {}

void GameManager::addCharacter(character *c, int team)
{
    if (!c)
        return;
    allCharacters.push_back(c);
    turnManager.addCharacter(c, team);

    hero *h = dynamic_cast<hero *>(c);
    if (h)
    {
        h->setBoard(&board);
        h->setGameManager(this);
    }

    dracula *d = dynamic_cast<dracula *>(c);
    if (d)
    {
        d->setAllCharacters(&allCharacters);
    }

    InvisibleMan *inv = dynamic_cast<InvisibleMan *>(c);
    if (inv)
    {
        inv->setAllCharacters(&allCharacters);
    }

    if (team == 1)
        team1.push_back(c);
    else
        team2.push_back(c);
}

void GameManager::removeCharacter(character *c)
{
    if (!c)
        return;
    bool isHero = (dynamic_cast<hero *>(c) != nullptr);

    auto it = std::find(allCharacters.begin(), allCharacters.end(), c);
    if (it != allCharacters.end())
        allCharacters.erase(it);

    auto it1 = std::find(team1.begin(), team1.end(), c);
    if (it1 != team1.end())
        team1.erase(it1);

    auto it2 = std::find(team2.begin(), team2.end(), c);
    if (it2 != team2.end())
        team2.erase(it2);

    turnManager.removeCharacter(c);

    if (isHero)
    {
        std::cout << c->getname() << " has been defeated! Game Over!" << std::endl;
    }
    else
    {
        std::cout << c->getname() << " has been defeated and removed." << std::endl;
    }
}

std::vector<character *> GameManager::getAllies(character *c) const
{
    std::vector<character *> allies;
    if (!c)
        return allies;

    int team = -1;
    if (std::find(team1.begin(), team1.end(), c) != team1.end())
        team = 1;
    else if (std::find(team2.begin(), team2.end(), c) != team2.end())
        team = 2;
    if (team == -1)
        return allies;

    const auto &teammates = (team == 1) ? team1 : team2;
    for (const auto &cc : teammates)
    {
        if (cc != c && cc->isalive())
            allies.push_back(cc);
    }
    return allies;
}

std::vector<character *> GameManager::getEnemies(character *c) const
{
    std::vector<character *> enemies;
    if (!c)
        return enemies;

    int team = -1;
    if (std::find(team1.begin(), team1.end(), c) != team1.end())
        team = 1;
    else if (std::find(team2.begin(), team2.end(), c) != team2.end())
        team = 2;
    if (team == -1)
        return enemies;

    const auto &enemyTeam = (team == 1) ? team2 : team1;
    for (const auto &cc : enemyTeam)
    {
        if (cc->isalive())
            enemies.push_back(cc);
    }
    return enemies;
}

std::vector<character *> GameManager::getAllCharacters() const
{
    return allCharacters;
}

std::vector<std::string> GameManager::getValidMoves(character *c)
{
    if (!c)
        return {};
    auto allies = getAllies(c);
    auto enemies = getEnemies(c);
    auto moves = movement.getPossibleMoves(c, c->getmovement(), allies, enemies);

    if (InvisibleMan *inv = dynamic_cast<InvisibleMan *>(c))
    {
        auto fogPositions = inv->getFogPositions();
        bool onFog = std::find(fogPositions.begin(), fogPositions.end(), c->getx()) != fogPositions.end();
        if (onFog)
        {
            for (int fogNode : fogPositions)
            {
                if (fogNode == c->getx())
                    continue;

                bool occupied = false;
                for (character *other : allCharacters)
                {
                    if (other != c && other->isalive() && other->getx() == fogNode)
                    {
                        occupied = true;
                        break;
                    }
                }
                if (occupied)
                    continue;

                std::string nodeName = "n" + std::to_string(fogNode);
                if (std::find(moves.begin(), moves.end(), nodeName) == moves.end())
                {
                    moves.push_back(nodeName);
                }
            }
        }
    }

    return moves;
}

bool GameManager::moveCharacter(character *c, const std::string &targetSpace, const card *boostCard)
{
    if (!c || !c->isalive())
        return false;

    character *activeChar = turnManager.getCurrentCharacter();
    if (!activeChar)
        return false;

    int activeTeam = turnManager.getCurrentTeam();
    bool isSameTeam = false;
    const auto &currentTeamChars = turnManager.getTeamCharacters(activeTeam);
    for (character *teamMember : currentTeamChars)
    {
        if (teamMember == c)
        {
            isSameTeam = true;
            break;
        }
    }
    if (!isSameTeam)
    {
        std::cout << "Not your team's turn!" << std::endl;
        return false;
    }

    if (turnManager.getActionsRemaining() <= 0)
    {
        std::cout << "No actions remaining!" << std::endl;
        return false;
    }

    if (!board.hasSpace(targetSpace))
    {
        std::cout << "Invalid node!" << std::endl;
        return false;
    }
    int targetNode = board.getNodeId(targetSpace);

    bool reachable = false;
    if (InvisibleMan* invC = dynamic_cast<InvisibleMan*>(c))
    {
        auto fogPositions = invC->getFogPositions();
        bool startOnFog = std::find(fogPositions.begin(), fogPositions.end(), c->getx()) != fogPositions.end();
        bool targetOnFog = std::find(fogPositions.begin(), fogPositions.end(), targetNode) != fogPositions.end();
        if (startOnFog && targetOnFog)
        {
            reachable = true;
        }
    }

    if (!reachable)
    {
        int moveSteps = c->getmovement();
        if (boostCard)
        {
            moveSteps += boostCard->getboost();
        }

        auto allies = getAllies(c);
        auto enemies = getEnemies(c);
        std::string startSpace = "n" + std::to_string(c->getx());

        reachable = movement.canReach(startSpace, targetSpace, moveSteps, allies, enemies);
    }

    if (!reachable)
    {
        std::cout << "Cannot reach " << targetSpace << std::endl;
        return false;
    }

    for (character *other : allCharacters)
    {
        if (other != c && other->isalive() && other->getx() == targetNode)
        {
            std::cout << "Node " << targetSpace << " is occupied!" << std::endl;
            return false;
        }
    }

    c->setposition(targetNode);

    hero *activeHero = dynamic_cast<hero *>(activeChar);
    if (activeHero)
    {
        activeHero->useAction();
        activeHero->drawcard();
        // NOTE: do NOT auto-discard down to 7 here. If the hand goes over
        // 7 cards, the UI (mustDiscard in raylib.cpp) blocks further
        // actions and forces the player to manually pick which card(s)
        // to discard via DrawDiscardModal. Silently popping a card here
        // would delete it before the player ever sees it.
    }

    std::cout << c->getname() << " successfully moved to " << targetSpace << std::endl;
    return true;
}

bool GameManager::resurrectSister(const std::string &sisterName, int heroNode)
{
    character *sister = nullptr;
    for (character *c : allCharacters)
    {
        if (c->getname() == sisterName)
        {
            sister = c;
            break;
        }
    }
    if (!sister || sister->isalive())
        return false;

    auto zones = board.getZonesAt(heroNode, 0);
    for (const auto &zone : zones)
    {
        auto spaces = board.getSpacesInZone(zone);
        for (const auto &space : spaces)
        {
            int node = space.first;
            bool occupied = false;
            for (character *c : allCharacters)
            {
                if (c->isalive() && c->getx() == node)
                {
                    occupied = true;
                    break;
                }
            }
            if (!occupied && node != heroNode)
            {
                sister->sethealth(1);
                sister->setposition(node);
                return true;
            }
        }
    }
    return false;
}

void GameManager::startGame(int firstTeam)
{
    turnManager.startGame(firstTeam);
}

void GameManager::nextTurn()
{
    turnManager.nextTurn();
}

void GameManager::nextPhase()
{
    turnManager.nextPhase();
}

character *GameManager::getCurrentCharacter() const
{
    return turnManager.getCurrentCharacter();
}

TurnPhase GameManager::getCurrentPhase() const
{
    return turnManager.getCurrentPhase();
}

int GameManager::getTurnNumber() const
{
    return turnManager.getTurnNumber();
}

int GameManager::getCurrentTeam() const
{
    return turnManager.getCurrentTeam();
}

int GameManager::getActionsRemaining() const
{
    return turnManager.getActionsRemaining();
}

bool GameManager::isGameOver() const
{
    return turnManager.isGameOver();
}

character *GameManager::getWinner() const
{
    return turnManager.getWinner();
}

character *GameManager::findCharacterByName(const std::string &name)
{
    if (name.empty())
        return nullptr;

    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

    for (auto c : allCharacters)
    {
        if (!c)
            continue;
        std::string cName = c->getname();
        std::transform(cName.begin(), cName.end(), cName.begin(), ::tolower);

        if (cName == lowerName)
            return c;

        if ((lowerName == "sh" || lowerName == "sherlock") && cName.find("sherlock") != std::string::npos)
            return c;
        if ((lowerName == "w" || lowerName == "watson") && cName.find("watson") != std::string::npos)
            return c;
        if ((lowerName == "d" || lowerName == "dracula") && cName.find("dracula") != std::string::npos)
            return c;
        if ((lowerName == "s1" || lowerName == "sister1" || lowerName == "sister 1") && cName.find("sister") != std::string::npos && cName.find("1") != std::string::npos)
            return c;
        if ((lowerName == "s2" || lowerName == "sister2" || lowerName == "sister 2") && cName.find("sister") != std::string::npos && cName.find("2") != std::string::npos)
            return c;
        if ((lowerName == "inv" || lowerName == "invisible" || lowerName == "im") && cName.find("invisible") != std::string::npos)
            return c;
    }

    return nullptr;
}

bool GameManager::handleMove(const std::string &charIdentifier, const std::string &targetNodeStr)
{
    character *actor = nullptr;

    if (charIdentifier.empty())
    {
        actor = getCurrentCharacter();
    }
    else
    {
        actor = findCharacterByName(charIdentifier);
    }

    if (!actor)
    {
        return false;
    }

    return handleMove(actor, targetNodeStr);
}

bool GameManager::handleMove(character *actor, const std::string &targetNodeStr)
{
    if (!actor || !actor->isalive())
    {
        return false;
    }

    int activeTeam = getCurrentTeam();
    const auto &currentTeamChars = (activeTeam == 1) ? team1 : team2;

    bool belongsToCurrentTeam = false;
    for (auto c : currentTeamChars)
    {
        if (c == actor)
        {
            belongsToCurrentTeam = true;
            break;
        }
    }

    if (!belongsToCurrentTeam)
    {
        return false;
    }

    if (getActionsRemaining() <= 0)
    {
        return false;
    }

    hero* activeHero = nullptr;
    for (auto c : currentTeamChars) {
        hero* h = dynamic_cast<hero*>(c);
        if (h && h->isalive()) {
            activeHero = h;
            break;
        }
    }

    if (activeHero) {
        if (activeHero->getdeck().getsize() > 0) {
            activeHero->drawcard();
            std::cout << activeHero->getname() << " drew a card." << std::endl;
        } else {
            std::cout << activeHero->getname() << "'s deck is empty! Taking 2 exhaustion damage." << std::endl;
            activeHero->sethealth(activeHero->gethealth() - 2);
            if (activeHero->gethealth() <= 0) {
                activeHero->sethealth(0);
                removeCharacter(activeHero);
            }
        }
    }

    std::string formattedNode = targetNodeStr;
    if (!formattedNode.empty())
    {
        if (formattedNode[0] == 'N' || formattedNode[0] == 'n')
        {
            formattedNode[0] = 'n';
        }
        else
        {
            formattedNode = "n" + formattedNode;
        }
    }

    if (!board.hasSpace(formattedNode))
    {
        std::cout << "Invalid node!" << std::endl;
        return false;
    }
    int targetNode = board.getNodeId(formattedNode);

    bool reachable = false;
    if (InvisibleMan* invActor = dynamic_cast<InvisibleMan*>(actor))
    {
        auto fogPositions = invActor->getFogPositions();
        bool startOnFog = std::find(fogPositions.begin(), fogPositions.end(), actor->getx()) != fogPositions.end();
        bool targetOnFog = std::find(fogPositions.begin(), fogPositions.end(), targetNode) != fogPositions.end();
        if (startOnFog && targetOnFog)
        {
            reachable = true;
        }
    }

    if (!reachable)
    {
        int moveSteps = actor->getmovement();
        auto allies = getAllies(actor);
        auto enemies = getEnemies(actor);
        std::string startSpace = "n" + std::to_string(actor->getx());
        reachable = movement.canReach(startSpace, formattedNode, moveSteps, allies, enemies);
    }

    if (!reachable)
    {
        std::cout << "Cannot reach " << formattedNode << std::endl;
        return false;
    }

    for (character *other : allCharacters)
    {
        if (other != actor && other->isalive() && other->getx() == targetNode)
        {
            std::cout << "Node " << formattedNode << " is occupied!" << std::endl;
            return false;
        }
    }

    actor->setposition(targetNode);

    character *currentTurnChar = turnManager.getCurrentCharacter();
    hero *turnHero = dynamic_cast<hero *>(currentTurnChar);
    if (turnHero)
    {
        turnHero->useAction();
    }

    std::cout << actor->getname() << " successfully moved to " << formattedNode << std::endl;

    return true;
}

static std::vector<std::string> SplitFields(const std::string &line, char delim)
{
    std::vector<std::string> fields;
    std::string field;
    for (char ch : line)
    {
        if (ch == delim)
        {
            fields.push_back(field);
            field.clear();
        }
        else
        {
            field += ch;
        }
    }
    fields.push_back(field);
    return fields;
}

bool GameManager::saveGame(const std::string &filename) const
{
    std::ofstream outFile(filename);
    if (!outFile.is_open())
        return false;

    // Header line: which hero classes are on each team, so that on load
    // (before any character objects exist yet) the caller knows whether to
    // construct a Dracula or an Invisible Man for team 1, and a Sherlock
    // or an Invisible Man for team 2.
    std::string team1Type = "INVISIBLE_MAN";
    std::string team2Type = "INVISIBLE_MAN";
    for (auto c : team1)
    {
        if (dynamic_cast<dracula *>(c))
        {
            team1Type = "DRACULA";
            break;
        }
    }
    for (auto c : team2)
    {
        hero *h = dynamic_cast<hero *>(c);
        if (h && !dynamic_cast<InvisibleMan *>(c))
        {
            team2Type = "SHERLOCK";
            break;
        }
    }
    outFile << team1Type << "|" << team2Type << "\n";

    outFile << turnManager.getTurnNumber() << "\n";
    outFile << turnManager.getCurrentTeam() << "\n";
    outFile << static_cast<int>(turnManager.getCurrentPhase()) << "\n";
    outFile << turnManager.getActionsRemaining() << "\n";

    character *curChar = turnManager.getCurrentCharacter();
    outFile << (curChar ? curChar->getname() : std::string("")) << "\n";

    outFile << allCharacters.size() << "\n";

    for (auto c : allCharacters)
    {
        if (!c)
            continue;

        outFile << c->getname() << "|"
                << c->gethealth() << "|"
                << c->getMaxHp() << "|"
                << c->getposition() << "|";

        InvisibleMan *inv = dynamic_cast<InvisibleMan *>(c);
        if (inv)
        {
            auto fog = inv->getFogPositions();
            outFile << "1|" << fog.size();
            for (int pos : fog)
                outFile << "," << pos;
            outFile << "|";
        }
        else
        {
            outFile << "0||";
        }

        hero *h = dynamic_cast<hero *>(c);
        if (h)
        {
            auto hand = h->gethand();
            outFile << hand.size();
            for (const auto &cd : hand)
            {
                std::string effect = cd.geteffect();
                for (auto &ch : effect)
                {
                    if (ch == '|' || ch == ';' || ch == '\n' || ch == '\r')
                        ch = ' ';
                }
                std::string cardName = cd.get_name();
                for (auto &ch : cardName)
                {
                    if (ch == '|' || ch == ';' || ch == '\n' || ch == '\r')
                        ch = ' ';
                }

                outFile << "|" << cardName << ";"
                        << static_cast<int>(cd.gettype()) << ";"
                        << cd.getattack() << ";"
                        << cd.getdefense() << ";"
                        << cd.getboost() << ";"
                        << static_cast<int>(cd.getowner()) << ";"
                        << effect;
            }
        }
        else
        {
            outFile << "0";
        }

        // Persist the hero's remaining draw pile too -- without this the
        // deck gets rebuilt fresh (and reshuffled) by the hero constructor
        // every time the app restarts, which silently undoes everything
        // that happened to that deck during the saved game (cards drawn,
        // discarded, or removed never stay removed on load).
        if (h)
        {
            const auto &deckCards = h->getdeck().getCards();
            outFile << "|" << deckCards.size();
            for (const auto &cd : deckCards)
            {
                std::string effect = cd.geteffect();
                for (auto &ch : effect)
                {
                    if (ch == '|' || ch == ';' || ch == '\n' || ch == '\r')
                        ch = ' ';
                }
                std::string cardName = cd.get_name();
                for (auto &ch : cardName)
                {
                    if (ch == '|' || ch == ';' || ch == '\n' || ch == '\r')
                        ch = ' ';
                }

                outFile << "|" << cardName << ";"
                        << static_cast<int>(cd.gettype()) << ";"
                        << cd.getattack() << ";"
                        << cd.getdefense() << ";"
                        << cd.getboost() << ";"
                        << static_cast<int>(cd.getowner()) << ";"
                        << effect;
            }
        }
        else
        {
            outFile << "|0";
        }

        outFile << "\n";
    }

    outFile.close();
    return true;
}

bool GameManager::peekHeroTypes(const std::string &filename, std::string &team1Type, std::string &team2Type) const
{
    std::ifstream inFile(filename);
    if (!inFile.is_open())
        return false;

    std::string line;
    if (!std::getline(inFile, line))
        return false;

    std::vector<std::string> fields = SplitFields(line, '|');
    if (fields.size() < 2)
        return false;

    team1Type = fields[0];
    team2Type = fields[1];
    return true;
}

bool GameManager::loadGame(const std::string &filename)
{
    std::ifstream inFile(filename);
    if (!inFile.is_open())
        return false;

    std::string line;

    // First line is the "team1Type|team2Type" header written by saveGame;
    // the caller is expected to have already used peekHeroTypes() to build
    // the matching character objects before calling loadGame(), so we just
    // skip over it here.
    if (!std::getline(inFile, line))
        return false;

    if (!std::getline(inFile, line))
        return false;
    int turnNum = 0;
    try
    {
        turnNum = std::stoi(line);
    }
    catch (...)
    {
        inFile.close();
        return false;
    }

    if (!std::getline(inFile, line))
        return false;
    int teamNum = 0;
    try
    {
        teamNum = std::stoi(line);
    }
    catch (...)
    {
        inFile.close();
        return false;
    }

    if (!std::getline(inFile, line))
        return false;
    int phaseVal = 0;
    try
    {
        phaseVal = std::stoi(line);
    }
    catch (...)
    {
        inFile.close();
        return false;
    }

    if (!std::getline(inFile, line))
        return false;
    int actionsRem = 0;
    try
    {
        actionsRem = std::stoi(line);
    }
    catch (...)
    {
        inFile.close();
        return false;
    }

    if (!std::getline(inFile, line))
        return false;
    std::string currentCharName = line;

    if (!std::getline(inFile, line))
        return false;
    int charCount = 0;
    try
    {
        charCount = std::stoi(line);
    }
    catch (...)
    {
        inFile.close();
        return false;
    }

    for (int i = 0; i < charCount; ++i)
    {
        if (!std::getline(inFile, line))
            break;

        std::vector<std::string> fields = SplitFields(line, '|');
        if (fields.size() < 6)
            continue;

        std::string name = fields[0];
        int hp = 0, maxHp = 0, pos = 0, isInv = 0;
        try
        {
            hp = std::stoi(fields[1]);
            maxHp = std::stoi(fields[2]);
            pos = std::stoi(fields[3]);
            isInv = std::stoi(fields[4]);
        }
        catch (...)
        {
            continue;
        }

        std::string fogField = fields[5];
        std::string handField = fields.size() > 6 ? fields[6] : "0";
        int nextIdx = 7;

        character *targetChar = findCharacterByName(name);
        if (!targetChar)
            continue;

        targetChar->sethealth(hp);
        targetChar->setposition(pos);

        if (isInv)
        {
            InvisibleMan *inv = dynamic_cast<InvisibleMan *>(targetChar);
            if (inv && !fogField.empty())
            {
                std::vector<int> fogVals;
                std::string num;
                for (char ch : fogField)
                {
                    if (ch == ',')
                    {
                        if (!num.empty())
                        {
                            fogVals.push_back(std::stoi(num));
                            num.clear();
                        }
                    }
                    else
                    {
                        num += ch;
                    }
                }
                if (!num.empty())
                    fogVals.push_back(std::stoi(num));

                for (size_t k = 1; k < fogVals.size() && k <= 3; ++k)
                {
                    inv->setFogPosition((int)k - 1, fogVals[k]);
                }
            }
        }

        hero *h = dynamic_cast<hero *>(targetChar);
        if (h)
        {
            int handCount = 0;
            try
            {
                handCount = std::stoi(handField);
            }
            catch (...)
            {
                handCount = 0;
            }

            auto &hand = h->gethand();
            hand.clear();

            for (int k = 0; k < handCount && nextIdx < (int)fields.size(); ++k, ++nextIdx)
            {
                std::vector<std::string> cardParts = SplitFields(fields[nextIdx], ';');
                if (cardParts.size() < 7)
                    continue;

                std::string cardName = cardParts[0];
                int atk = 0, def = 0, boost = 0, typeVal = 0, ownerVal = 0;
                try
                {
                    typeVal = std::stoi(cardParts[1]);
                    atk = std::stoi(cardParts[2]);
                    def = std::stoi(cardParts[3]);
                    boost = std::stoi(cardParts[4]);
                    ownerVal = std::stoi(cardParts[5]);
                }
                catch (...)
                {
                    continue;
                }
                std::string effect = cardParts[6];

                cardtype cType = static_cast<cardtype>(typeVal);
                cardowner cOwner = static_cast<cardowner>(ownerVal);

                hand.push_back(card(cardName, cType, atk, def, boost, cOwner, effect));
            }

            // Deck field comes right after the hand cards in the line.
            int deckCount = 0;
            if (nextIdx < (int)fields.size())
            {
                try
                {
                    deckCount = std::stoi(fields[nextIdx]);
                }
                catch (...)
                {
                    deckCount = 0;
                }
                ++nextIdx;
            }

            std::vector<card> restoredDeck;
            for (int k = 0; k < deckCount && nextIdx < (int)fields.size(); ++k, ++nextIdx)
            {
                std::vector<std::string> cardParts = SplitFields(fields[nextIdx], ';');
                if (cardParts.size() < 7)
                    continue;

                std::string cardName = cardParts[0];
                int catk = 0, cdef = 0, cboost = 0, ctypeVal = 0, cownerVal = 0;
                try
                {
                    ctypeVal = std::stoi(cardParts[1]);
                    catk = std::stoi(cardParts[2]);
                    cdef = std::stoi(cardParts[3]);
                    cboost = std::stoi(cardParts[4]);
                    cownerVal = std::stoi(cardParts[5]);
                }
                catch (...)
                {
                    continue;
                }
                std::string ceffect = cardParts[6];

                restoredDeck.push_back(card(cardName,
                                             static_cast<cardtype>(ctypeVal),
                                             catk, cdef, cboost,
                                             static_cast<cardowner>(cownerVal),
                                             ceffect));
            }
            h->getdeck().setCards(restoredDeck);
        }
    }

    character *restoredCurrent = currentCharName.empty() ? nullptr : findCharacterByName(currentCharName);
    turnManager.restoreState(turnNum, teamNum, static_cast<TurnPhase>(phaseVal), actionsRem, restoredCurrent);

    inFile.close();
    return true;
}

std::vector<SaveSlotInfo> GameManager::getRecentSaveSlots() const {
    std::vector<SaveSlotInfo> slots;
    for (int i = 1; i <= 3; ++i) {
        std::string fname = "save_slot_" + std::to_string(i) + ".txt";
        std::ifstream inFile(fname);
        SaveSlotInfo info;
        info.slotNumber = i;
        info.filename = fname;
        
        if (inFile.is_open()) {
            info.exists = true;
            std::string teamHeader, turnLine;
            std::getline(inFile, teamHeader);
            std::getline(inFile, turnLine);
            info.details = "Slot " + std::to_string(i) + " - " + (turnLine.empty() ? "Saved Game" : turnLine);
            inFile.close();
        } else {
            info.exists = false;
            info.details = "Empty Slot " + std::to_string(i);
        }
        slots.push_back(info);
    }
    return slots;
}