#pragma once

#include <vector>
#include <string>

#include "card.hpp"
#include "deck.hpp"
#include "map.hpp"
#include "character.hpp"

class hero : public character {
private:
    deck dk;
    std::vector<card> hand;
    int actions;
    Board* board;

public:
    // Constructor
    hero(const std::string& name, int maxhp, int movement);

    virtual ~hero() = default;

    // Board
    void setBoard(Board* b) { board = b; }
    Board& getBoard() { return *board; }
    const Board& getBoard() const { return *board; }

    // Actions
    void set_actions(int new_actions);
    void reset_actions();
    void useAction();
    int get_actions() const;
    bool canact() const;

    // Deck & Hand
    void drawcard();
    void drawhand();

    deck& getdeck();
    const deck& getdeck() const;

    std::vector<card>& gethand();
    const std::vector<card>& gethand() const;

    int handsize() const;

    // Auto defense selection
    card chooseDefense();

    // Hero special ability
    virtual void useability() = 0;

    // Game actions
    bool maneuver(int targetNode, Board& board,
                  const card* boostCard = nullptr);

    bool scheme(card& schemeCard, hero& target);

    bool attack(hero& target, card& attackcard,
                Board& board);
};