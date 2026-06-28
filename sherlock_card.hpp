#ifndef SHERLOCK_CARD_HPP
#define SHERLOCK_CARD_HPP

#include <string>
#include "character.hpp"
#include "hero.hpp"
#include "card.hpp"

using namespace std;

class sherlock_card_base {
protected:
    string card_name;
    cardtype card_type;
    int card_boost;

public:
    sherlock_card_base(string name, cardtype type, int boost) 
        : card_name(name), card_type(type), card_boost(boost) {}

    virtual ~sherlock_card_base() {}

    virtual void execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) = 0;
};

class card_confirming_suspicion : public sherlock_card_base {
public:
    card_confirming_suspicion() : sherlock_card_base("Confirming Suspicion", cardtype::scheme, 0) {}
    void execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) override;
};

class card_administer_aid : public sherlock_card_base {
public:
    card_administer_aid() : sherlock_card_base("Administer Aid", cardtype::scheme, 0) {}
    void execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) override;
};

class card_counterpunch : public sherlock_card_base {
public:
    card_counterpunch() : sherlock_card_base("Counterpunch", cardtype::multipurpose, 2) {}
    void execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) override;
};

class card_deduce_strategy : public sherlock_card_base {
public:
    card_deduce_strategy() : sherlock_card_base("Deduce Strategy", cardtype::multipurpose, 2) {}
    void execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) override;
};

class card_education_never_ends : public sherlock_card_base {
public:
    card_education_never_ends() : sherlock_card_base("Education Never Ends", cardtype::multipurpose, 2) {}
    void execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) override;
};

class card_elementary : public sherlock_card_base {
public:
    card_elementary() : sherlock_card_base("Elementary", cardtype::defense, 1) {}
    void execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) override;
};

class card_eliminate_impossible : public sherlock_card_base {
public:
    card_eliminate_impossible() : sherlock_card_base("Eliminate the Impossible", cardtype::scheme, 0) {}
    void execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) override;
};

class card_feint : public sherlock_card_base {
public:
    card_feint() : sherlock_card_base("Feint", cardtype::multipurpose, 1) {}
    void execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) override;
};

class card_fixed_point : public sherlock_card_base {
public:
    card_fixed_point() : sherlock_card_base("Fixed Point in a Changing Age", cardtype::multipurpose, 2) {}
    void execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) override;
};

class card_master_of_disguise : public sherlock_card_base {
public:
    card_master_of_disguise() : sherlock_card_base("Master of Disguise", cardtype::scheme, 0) {}
    void execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) override;
};

class card_game_is_afoot : public sherlock_card_base {
public:
    card_game_is_afoot() : sherlock_card_base("The Game is Afoot", cardtype::attack, 3) {}
    void execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) override;
};

class card_service_revolver : public sherlock_card_base {
public:
    card_service_revolver() : sherlock_card_base("Service Revolver", cardtype::attack, 1) {}
    void execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) override;
};

class card_study_methods : public sherlock_card_base {
public:
    card_study_methods() : sherlock_card_base("Study Methods", cardtype::multipurpose, 2) {}
    void execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) override;
};

#endif
