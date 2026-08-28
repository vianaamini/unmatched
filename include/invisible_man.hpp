#ifndef INVISIBLE_MAN_HPP
#define INVISIBLE_MAN_HPP

#include "hero.hpp"
#include <vector>
#include <string>

class InvisibleMan : public hero {
private:
    std::vector<int> fogPositions;
    std::vector<character*>* allCharacters = nullptr;
    bool vanished = false;
    bool startedTurnOnFog = false;
    std::string confoundFogTarget = "";

public:
    InvisibleMan();

    void initializeFogTokens(int startNode);
    void setAllCharacters(std::vector<character*>* chars);
    bool isOnFog() const;
    void onTurnStart();
    void resolveVanish(int node);
    bool isVanished() const { return vanished; }
    void setVanished(bool v) { vanished = v; }

    std::vector<int> getFogPositions() const;
    void setFogPosition(int index, int node);

    void takedamage(int amount) override;
    void useability() override;

    bool executeSchemeCard(card& schemeCard, hero& target);
    void executeAttackCardEffects(card& attackCard, character& target, int& attackValue, int& defenseValue,
                                  bool& attackerWon, bool& effectsCanceled, const card& defenseCard,
                                  bool defenseCardProtected);
    void executeDefenseCardEffects(card& defenseCard, const card& attackCard, int& defenseValue,
                                   int& attackValue, bool& effectsCanceled, bool attackCardProtected);

    void setConfoundFogTarget(const std::string& target) { confoundFogTarget = target; }
    std::string getConfoundFogTarget() const { return confoundFogTarget; }
};

#endif