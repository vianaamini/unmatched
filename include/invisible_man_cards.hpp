#ifndef INVISIBLE_MAN_CARDS_HPP
#define INVISIBLE_MAN_CARDS_HPP

#include "character.hpp"
#include "card.hpp"

class card_vanish {
public:
    static void execute_effect(character& attacker, character& defender);
};

class card_reign_of_terror {
public:
    static void execute_effect(character& attacker, character& defender, bool is_on_fog);
};

class card_rolling_fog {
public:
    static void execute_effect(character& attacker);
};

class card_step_lightly {
public:
    static void execute_effect(character& attacker, character& defender, bool is_on_fog);
};

class card_confound {
public:
    static void execute_effect(character& attacker, character& defender);
};

class card_covert_preparation {
public:
    static void execute_effect(character& attacker);
};

class card_dreaming_of_revenge {
public:
    static void execute_effect(character& attacker, character& defender, bool is_on_fog, bool enemy_on_fog);
};

class card_impossible_to_see {
public:
    static void execute_effect(const card& enemy_card);
};

class card_coded_notes {
public:
    static void execute_effect(character& attacker);
};

class card_into_thin_air {
public:
    static void execute_effect(character& attacker);
};

class card_lurking {
public:
    static void execute_effect(character& attacker);
};

class card_emerge_from_mist {
public:
    static void execute_effect(card& my_card, bool started_on_fog);
};

class card_slip_away {
public:
    static void execute_effect(character& attacker);
};

#endif