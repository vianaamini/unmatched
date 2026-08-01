#include "../include/deck-builder.hpp"

deck sherlockdeck() {
    deck d;
    
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Administer Aid", cardtype::scheme, 0, 0, 2, cardowner::watson, 
                       "Place Watson adjacent to Holmes, heal Holmes 1, draw a card"));
    }
    
    for (int i = 0; i < 3; i++) {
        d.addcard(card("Confirm Suspicion", cardtype::scheme, 0, 0, 1, cardowner::sherlock,
                       "Name a value, opponent must discard a card with that value or reveal hand"));
    }
    
    for (int i = 0; i < 3; i++) {
        d.addcard(card("Counter Punch", cardtype::multipurpose, 3, 3, 1, cardowner::sherlock,
                       "If Holmes is adjacent to enemy, deal 2 damage"));
    }
    
    for (int i = 0; i < 3; i++) {
        d.addcard(card("Deduce Strategy", cardtype::multipurpose, 3, 3, 1, cardowner::sherlock,
                       "Change opponent's card value to its boost value"));
    }
    
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Education Never Ends", cardtype::multipurpose, 3, 3, 1, cardowner::any,
                       "If you win, opponent draws 1; if you lose, you draw 2"));
    }
    
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Elementary", cardtype::defense, 0, 3, 3, cardowner::sherlock,
                       "Predict opponent's attack. If correct, cancel all effects"));
    }
    
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Eliminate the Impossible", cardtype::scheme, 0, 0, 2, cardowner::sherlock,
                       "Look at opponent's hand and burn 1 card"));
    }
    
    // Feint - consistent boost value = 1 (as per Sherlock deck)
    for (int i = 0; i < 3; i++) {
        d.addcard(card("Feint", cardtype::multipurpose, 2, 2, 1, cardowner::any,
                       "Cancel all effects on opponent's card"));
    }
    
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Fixed Point", cardtype::multipurpose, 3, 3, 1, cardowner::watson,
                       "If Watson is adjacent to Holmes, heal both for 1"));
    }
    
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Master of Disguise", cardtype::scheme, 0, 0, 2, cardowner::sherlock,
                       "Swap positions with opponent and deal 1 damage"));
    }
    
    for (int i = 0; i < 2; i++) {
        d.addcard(card("The Game is Afoot", cardtype::attack, 5, 0, 2, cardowner::sherlock,
                       "Move Holmes 3 spaces"));
    }
    
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Service Revolver", cardtype::attack, 5, 0, 3, cardowner::watson,
                       "No effect"));
    }
    
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Study Methods", cardtype::multipurpose, 3, 3, 2, cardowner::any,
                       "If you win, you may look at opponent's hand"));
    }
    
    return d;
}

deck draculadeck() {
    deck d;
    
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Feeding Frenzy", cardtype::attack, 2, 0, 3, cardowner::dracula,
                       "+1 attack for each Sister adjacent to opponent"));
    }
    
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Mistform", cardtype::scheme, 0, 0, 2, cardowner::dracula,
                       "Place Dracula anywhere and gain 1 action"));
    }
    
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Ambush", cardtype::attack, 2, 0, 3, cardowner::any,
                       "Opponent discards 1 random card, add its boost to attack"));
    }
    
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Baptism of Blood", cardtype::scheme, 0, 0, 2, cardowner::dracula,
                       "Heal 2. If a Sister is defeated, return her to Dracula's zone"));
    }
    
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Beastform", cardtype::attack, 6, 0, 4, cardowner::dracula,
                       "Discard any number of cards, +1 attack per card discarded"));
    }
    
    for (int i = 0; i < 3; i++) {
        d.addcard(card("Dash", cardtype::multipurpose, 3, 3, 1, cardowner::any,
                       "Move your fighter up to 3 spaces"));
    }
    
    for (int i = 0; i < 3; i++) {
        d.addcard(card("Exploit", cardtype::multipurpose, 4, 4, 1, cardowner::any,
                       "Draw 1 card"));
    }
    
    for (int i = 0; i < 3; i++) {
        d.addcard(card("Look Into My Eyes", cardtype::defense, 0, 1, 2, cardowner::dracula,
                       "Add opponent's attack boost to this defense"));
    }
    
    for (int i = 0; i < 2; i++) {
        d.addcard(card("Prey Upon", cardtype::scheme, 0, 0, 4, cardowner::dracula,
                       "Deal 1 damage to all enemies adjacent to Dracula, heal 1 per damage"));
    }
    
    for (int i = 0; i < 3; i++) {
        d.addcard(card("Ravening Seduction", cardtype::scheme, 0, 0, 2, cardowner::sister,
                       "Move any fighter 2 spaces, deal 1 damage per adjacent Sister"));
    }
    
    for (int i = 0; i < 3; i++) {
        d.addcard(card("Thirst for Sustenance", cardtype::attack, 3, 0, 3, cardowner::sister,
                       "If you win, place Dracula adjacent to opponent"));
    }

    for (int i = 0; i < 3; i++) {
        d.addcard(card("Feint", cardtype::multipurpose, 2, 2, 1, cardowner::any,
                       "Cancel all effects on opponent's card"));
    }
    
    return d;
}