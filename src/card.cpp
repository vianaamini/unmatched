#include "../include/card.hpp"

card::card(string name ,cardtype type ,int attack ,int defense ,int boost ,
    cardowner owner, string effect) 
    :name(name) , type(type) , attackvalue(attack) , 
    defensevalue(defense) , boostvalue(boost) , owner(owner) ,
     effect(effect) {}

string card::get_name () const{return name;}

cardtype card::gettype () const{return type;}

int card::getattack () const{return attackvalue;}
int card::getdefense () const{return defensevalue;}
int card::getboost () const{return boostvalue;}
cardowner card::getowner () const{return owner;}
string card::geteffect () const{return effect;}