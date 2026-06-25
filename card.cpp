#include "card.hpp"

card::card(string name ,cardtype type ,int value) 
    :name(name) , type(type) , value(value) {}

string card::get_name () const{return name;}

cardtype card::gettype () const{return type;}

int card::getvalue () const{return value;}