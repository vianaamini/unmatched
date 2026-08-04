#include "../include/game_manager.hpp"
#include "../include/hero.hpp"
#include "../include/dracula.hpp"
#include <algorithm>
#include <iostream>
#include <algorithm>
#include <cctype>

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
    return movement.getPossibleMoves(c, c->getmovement(), allies, enemies);
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

    int moveSteps = c->getmovement();
    if (boostCard)
    {
        moveSteps += boostCard->getboost();
    }

    auto allies = getAllies(c);
    auto enemies = getEnemies(c);
    std::string startSpace = "n" + std::to_string(c->getx());

    bool reachable = movement.canReach(startSpace, targetSpace, moveSteps, allies, enemies);
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
    }

    std::cout << c->getname() << " successfully moved to " << targetSpace << std::endl;

    turnManager.endTurn();
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
character* GameManager::findCharacterByName(const std::string& name) {
    if (name.empty()) return nullptr;

    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

    for (auto c : allCharacters) {
        if (!c) continue;
        std::string cName = c->getname();
        std::transform(cName.begin(), cName.end(), cName.begin(), ::tolower);

        if (cName == lowerName) return c;

        if ((lowerName == "sh" || lowerName == "sherlock") && cName.find("sherlock") != std::string::npos) return c;
        if ((lowerName == "w" || lowerName == "watson") && cName.find("watson") != std::string::npos) return c;
        if ((lowerName == "d" || lowerName == "dracula") && cName.find("dracula") != std::string::npos) return c;
        if ((lowerName == "s1" || lowerName == "sister1" || lowerName == "sister 1") && cName.find("sister") != std::string::npos && cName.find("1") != std::string::npos) return c;
        if ((lowerName == "s2" || lowerName == "sister2" || lowerName == "sister 2") && cName.find("sister") != std::string::npos && cName.find("2") != std::string::npos) return c;
    }

    return nullptr;
}

bool GameManager::handleMove(const std::string& charIdentifier, const std::string& targetNodeStr) {
    character* actor = nullptr;

    if (charIdentifier.empty()) {
        actor = getCurrentCharacter();
    } else {
        actor = findCharacterByName(charIdentifier);
    }

    if (!actor) {
        return false;
    }

    return handleMove(actor, targetNodeStr);
}

bool GameManager::handleMove(character* actor, const std::string& targetNodeStr) {
    if (!actor || !actor->isalive()) {
        return false;
    }

    int activeTeam = getCurrentTeam();
    const auto& currentTeamChars = (activeTeam == 1) ? team1 : team2;
    
    bool belongsToCurrentTeam = false;
    for (auto c : currentTeamChars) {
        if (c == actor) {
            belongsToCurrentTeam = true;
            break;
        }
    }

    if (!belongsToCurrentTeam) {
        return false;
    }

    if (getActionsRemaining() <= 0) {
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
    if (!formattedNode.empty()) {
        if (formattedNode[0] == 'N' || formattedNode[0] == 'n') {
            formattedNode[0] = 'n';
        } else {
            formattedNode = "n" + formattedNode;
        }
    }

    bool isHero = (dynamic_cast<hero*>(actor) != nullptr);
    int moveSteps = isHero ? 3 : actor->getmovement();

    auto allies = getAllies(actor);
    auto enemies = getEnemies(actor);
    std::string startSpace = "n" + std::to_string(actor->getx());

    bool reachable = movement.canReach(startSpace, formattedNode, moveSteps, allies, enemies);
    if (!reachable) {
        std::cout << "Cannot reach " << formattedNode << " within " << moveSteps << " steps!" << std::endl;
        return false;
    }

    int targetNode = board.getNodeId(formattedNode);
    for (character *other : allCharacters) {
        if (other != actor && other->isalive() && other->getx() == targetNode) {
            std::cout << "Node " << formattedNode << " is occupied!" << std::endl;
            return false;
        }
    }

    actor->setposition(targetNode);

    character* currentTurnChar = turnManager.getCurrentCharacter();
    hero* turnHero = dynamic_cast<hero*>(currentTurnChar);
    if (turnHero) {
        turnHero->useAction();
    }

    std::cout << actor->getname() << " successfully moved to " << formattedNode << std::endl;
    return true;
}