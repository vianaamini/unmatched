#pragma once

#include <string>
#include "character.hpp"
#include "hero.hpp"
#include "card.hpp"

class invisible_man_card_base {
protected:
    std::string card_name;
    cardtype card_type;
    int card_boost;

public:
    invisible_man_card_base(std::string name, cardtype type, int boost) 
        : card_name(name), card_type(type), card_boost(boost) {}

    virtual ~invisible_man_card_base() {}

    virtual void execute_effect(character& attacker, character& defender, 
                                const card& enemy_card, 
                                bool is_adjacent, 
                                bool attacker_won) = 0;
};

class card_impossible_to_catch : public invisible_man_card_base {
public:
    card_impossible_to_catch() : invisible_man_card_base("Impossible to Catch", cardtype::multipurpose, 2) {}
    void execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_adjacent, bool attacker_won) override;
};

class card_vanish : public invisible_man_card_base {
public:
    card_vanish() : invisible_man_card_base("Vanish", cardtype::defense, 1) {}
    void execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_adjacent, bool attacker_won) override;
};

class card_emerging_from_mist : public invisible_man_card_base {
public:
    card_emerging_from_mist() : invisible_man_card_base("Emerging from Mist", cardtype::attack, 2) {}
    void execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_adjacent, bool attacker_won) override;
};

class card_covert_preparation : public invisible_man_card_base {
public:
    card_covert_preparation() : invisible_man_card_base("Covert Preparation", cardtype::scheme, 1) {}
    void execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_adjacent, bool attacker_won) override;
};

class card_slip_away : public invisible_man_card_base {
public:
    card_slip_away() : invisible_man_card_base("Slip Away", cardtype::multipurpose, 3) {}
    void execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_adjacent, bool attacker_won) override;
};

class card_fog_vision : public invisible_man_card_base {
public:
    card_fog_vision() : invisible_man_card_base("Fog Vision", cardtype::attack, 1) {}
    void execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_adjacent, bool attacker_won) override;
};

class card_into_thin_air : public invisible_man_card_base {
public:
    card_into_thin_air() : invisible_man_card_base("Into Thin Air", cardtype::defense, 2) {}
    void execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_adjacent, bool attacker_won) override;
};

class card_lurking_in_shadows : public invisible_man_card_base {
public:
    card_lurking_in_shadows() : invisible_man_card_base("Lurking in Shadows", cardtype::scheme, 2) {}
    void execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_adjacent, bool attacker_won) override;
};