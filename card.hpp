#pragma once
#include<iostream>

using namespace std;

enum class cardtype {
    Attack,
    Defense,
    Scheme,
};

class card {
    string name;
    cardtype type;
    int value;

    public:
    card(string name ,cardtype type,int value);

    string get_name() const;
    cardtype gettype() const;
    int getvalue() const;
};