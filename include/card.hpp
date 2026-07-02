#pragma once
#include<iostream>

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

class card {
    string name;
    cardtype type;
    int attackvalue;
    int defensevalue;
    int boostvalue;
    cardowner owner;
    string effect;


    public:
    card(string name ,cardtype type,int attack ,int defense ,int boost ,cardowner owner
    , string effect = "");

    string get_name() const;
    cardtype gettype() const;
    int getattack() const;
    int getdefense() const;
    int getboost() const;
    cardowner getowner() const;
    string geteffect() const;
};