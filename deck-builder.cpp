#include "deck-builder.hpp"

// ==================== دک شرلوک ====================
deck sherlockdeck() {
    deck d;
    
    // 1. Administer Aid (2)
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Administer Aid", cardtype::scheme, 0, 0, 2, cardowner::watson, 
                       "Place Watson adjacent to Holmes, heal Holmes 1, draw a card"));
    }
    
    // 2. Confirm Suspicion (3)
    for (int i = 0; i < 3; i++) {
        d.addcard(card("Confirm Suspicion", cardtype::scheme, 0, 0, 1, cardowner::sherlock,
                       "Name a value, opponent must discard a card with that value or reveal hand"));
    }
    
    // 3. Counter Punch (3)
    for (int i = 0; i < 3; i++) {
        d.addcard(card("Counter Punch", cardtype::multipurpose, 3, 3, 1, cardowner::sherlock,
                       "If Holmes is adjacent to enemy, deal 2 damage"));
    }
    
    // 4. Deduce Strategy (3)
    for (int i = 0; i < 3; i++) {
        d.addcard(card("Deduce Strategy", cardtype::multipurpose, 3, 3, 1, cardowner::sherlock,
                       "Change opponent's card value to its boost value"));
    }
    
    // 5. Education Never Ends (2)
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Education Never Ends", cardtype::multipurpose, 3, 3, 1, cardowner::any,
                       "If you win, opponent draws 1; if you lose, you draw 2"));
    }
    
    // 6. Elementary (2)
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Elementary", cardtype::defense, 0, 3, 3, cardowner::sherlock,
                       "Predict opponent's attack. If correct, cancel all effects"));
    }
    
    // 7. Eliminate the Impossible (2)
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Eliminate the Impossible", cardtype::scheme, 0, 0, 2, cardowner::sherlock,
                       "Look at opponent's hand and burn 1 card"));
    }
    
    // 8. Feint (3)
    for (int i = 0; i < 3; i++) {
        d.addcard(card("Feint", cardtype::multipurpose, 2, 2, 1, cardowner::any,
                       "Cancel all effects on opponent's card"));
    }
    
    // 9. Fixed Point (2)
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Fixed Point", cardtype::multipurpose, 3, 3, 1, cardowner::watson,
                       "If Watson is adjacent to Holmes, heal both for 1"));
    }
    
    // 10. Master of Disguise (2)
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Master of Disguise", cardtype::scheme, 0, 0, 2, cardowner::sherlock,
                       "Swap positions with opponent and deal 1 damage"));
    }
    
    // 11. The Game is Afoot (2)
    for (int i = 0; i < 2; i++) {
        d.addcard(card("The Game is Afoot", cardtype::attack, 5, 0, 2, cardowner::sherlock,
                       "Move Holmes 3 spaces"));
    }
    
    // 12. Service Revolver (2)
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Service Revolver", cardtype::attack, 5, 0, 3, cardowner::watson,
                       "No effect"));
    }
    
    // 13. Study Methods (2)
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Study Methods", cardtype::multipurpose, 3, 3, 2, cardowner::any,
                       "If you win, you may look at opponent's hand"));
    }
    
    return d;
}

// ==================== دک دراکولا ====================
deck draculadeck() {
    deck d;
    
    // 1. Feeding Frenzy (2)
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Feeding Frenzy", cardtype::attack, 2, 0, 3, cardowner::dracula,
                       "+1 attack for each Sister adjacent to opponent"));
    }
    
    // 2. Mistform (2)
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Mistform", cardtype::scheme, 0, 0, 2, cardowner::dracula,
                       "Place Dracula anywhere and gain 1 action"));
    }
    
    // 3. Ambush (2)
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Ambush", cardtype::attack, 2, 0, 3, cardowner::any,
                       "Opponent discards 1 random card, add its boost to attack"));
    }
    
    // 4. Baptism of Blood (2)
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Baptism of Blood", cardtype::scheme, 0, 0, 2, cardowner::dracula,
                       "Heal 2. If a Sister is defeated, return her to Dracula's zone"));
    }
    
    // 5. Beastform (2)
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Beastform", cardtype::attack, 6, 0, 4, cardowner::dracula,
                       "Discard any number of cards, +1 attack per card discarded"));
    }
    
    // 6. Dash (3)
    for (int i = 0; i < 3; i++) {
        d.addcard(card("Dash", cardtype::multipurpose, 3, 3, 1, cardowner::any,
                       "Move your fighter up to 3 spaces"));
    }
    
    // 7. Exploit (3)
    for (int i = 0; i < 3; i++) {
        d.addcard(card("Exploit", cardtype::multipurpose, 4, 4, 1, cardowner::any,
                       "Draw 1 card"));
    }
    
    // 8. Look Into My Eyes (3)
    for (int i = 0; i < 3; i++) {
        d.addcard(card("Look Into My Eyes", cardtype::defense, 0, 1, 2, cardowner::dracula,
                       "Add opponent's attack boost to this defense"));
    }
    
    // 9. Prey Upon (2)
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Prey Upon", cardtype::scheme, 0, 0, 4, cardowner::dracula,
                       "Deal 1 damage to all enemies adjacent to Dracula, heal 1 per damage"));
    }
    
    // 10. Ravening Seduction (3)
    for (int i = 0; i < 3; i++) {
        d.addcard(card("Ravening Seduction", cardtype::scheme, 0, 0, 2, cardowner::sister,
                       "Move any fighter 2 spaces, deal 1 damage per adjacent Sister"));
    }
    
    // 11. Thirst for Sustenance (3)
    for (int i = 0; i < 3; i++) {
        d.addcard(card("Thirst for Sustenance", cardtype::attack, 3, 0, 3, cardowner::sister,
                       "If you win, place Dracula adjacent to opponent"));
    }
    
    // 12. Feint (3)
    for (int i = 0; i < 3; i++) {
        d.addcard(card("Feint", cardtype::multipurpose, 2, 2, 2, cardowner::any,
                       "Cancel all effects on opponent's card"));
    }
    
    return d;
}