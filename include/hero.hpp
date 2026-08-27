#pragma once

#include <vector>
#include <string>
#include "card.hpp"
#include "deck.hpp"
#include "map.hpp"
#include "character.hpp"

class GameManager;

class hero : public character {
private:
    deck dk;
    std::vector<card> hand;
    int actions;
    Board* board;
    GameManager* gameManager;

    int predictedAttackValue;
    int beastformDiscardCount;
    std::string mistformTarget;
    std::string raveningTargetFighter;
    std::string raveningTargetSpace;
    int eliminateCardIndex;
    int confirmSuspicionGuess;
    int dashTargetNode;
    int gameIsAfootTargetNode;

public:
    hero(const std::string& name, int maxhp, int movement);
    virtual ~hero() = default;

    void setBoard(Board* b);
    Board* getBoard();
    const Board* getBoard() const;
    void setGameManager(GameManager* gm) { gameManager = gm; }

    void setPredictedAttackValue(int val) { predictedAttackValue = val; }
    int getPredictedAttackValue() const { return predictedAttackValue; }
    void setBeastformDiscardCount(int val) { beastformDiscardCount = val; }
    void setMistformTarget(const std::string& target) { mistformTarget = target; }

    void setRaveningTargets(
        const std::string& fighter,
        const std::string& space
    ) {
        raveningTargetFighter = fighter;
        raveningTargetSpace = space;
    }

    void setEliminateCardIndex(int idx) { eliminateCardIndex = idx; }
    void setConfirmSuspicionGuess(int guess) { confirmSuspicionGuess = guess; }
    void setDashTargetNode(int node) { dashTargetNode = node; }
    void setGameIsAfootTargetNode(int node) { gameIsAfootTargetNode = node; }

    void set_actions(int new_actions);
    void reset_actions();
    void useAction();
    int get_actions() const;
    bool canact() const;

    deck& getdeck();
    const deck& getdeck() const;

    std::vector<card>& gethand();
    const std::vector<card>& gethand() const;

    int handsize() const;

    void drawcard();
    void drawhand();
    void discardToLimit(int limit = 7);
    bool removeCardFromHand(const std::string& cardName);

    virtual card chooseDefense();

    bool maneuver(
        int targetNode,
        Board& board,
        const card* boostCard = nullptr
    );

    bool scheme(card& schemeCard, hero& target);

    bool canAttack(
        const hero& target,
        const Board& board,
        bool ranged = false
    ) const;

    bool attack(
        hero& target,
        card& attackCard,
        Board& board,
        const card* defenseCard = nullptr
    );

    bool canAttack(
        const character& target,
        const Board& board,
        bool ranged = false
    ) const;

    bool attack(
        character& target,
        card& attackCard,
        Board& board
    );

    virtual void useability() = 0;

private:
    bool moveWithRules(
        int targetNode,
        int steps,
        Board& board
    );
};