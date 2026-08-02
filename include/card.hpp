#pragma once

#include <string>

using namespace std;

enum class cardtype {
    attack,
    defense,
    scheme,
    multipurpose
};

enum class cardowner {
    sherlock,
    watson,
    dracula,
    sister,
    any
};

enum class timing {
    none,
    immediately,
    during_combat,
    after_combat
};


class card {

private:

    string name;

    cardtype type;

    int attackvalue;
    int defensevalue;
    int boostvalue;

    cardowner owner;

    timing effectTiming;

    string effect;


public:


    card(
        string name = "",
        cardtype type = cardtype::multipurpose,
        int attack = 0,
        int defense = 0,
        int boost = 0,
        cardowner owner = cardowner::any,
        string effect = ""
    );


    card(
        string name,
        cardtype type,
        int attack,
        int defense,
        int boost,
        cardowner owner,
        timing effectTiming,
        string effect
    );


    string get_name() const;

    cardtype gettype() const;

    int getattack() const;

    int getdefense() const;

    int getboost() const;

    cardowner getowner() const;

    timing getTiming() const;

    string geteffect() const;


    void setattack(int value);

    void setdefense(int value);
};