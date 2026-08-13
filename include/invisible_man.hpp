#ifndef INVISIBLE_MAN_HPP
#define INVISIBLE_MAN_HPP

#include "hero.hpp"
#include "invisible_man_cards.hpp"
#include <vector>
#include <string>

class InvisibleMan : public hero {
private:
    std::vector<int> fogPositions;
    std::vector<character*>* allCharacters = nullptr;

public:
    InvisibleMan(std::string name, int startNode);
    ~InvisibleMan() override = default;

    const std::vector<int>& getFogPositions() const;
    void setFogPosition(int index, int nodeID);
    bool isOnFog() const;
    int getDefenseBonus() const;
    bool isFogOnNode(int nodeID) const;

    void setAllCharacters(std::vector<character*>* chars) {
        allCharacters = chars;
    }

    void initializeDeck();
    void useability() override;
};

#endif