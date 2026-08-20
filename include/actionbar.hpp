#pragma once

#include "raylib.h"
#include "../include/game_manager.hpp"
#include "../include/character.hpp"
#include "../include/hero.hpp"
#include "../include/card.hpp"
#include <string>
#include <vector>

enum class ActionMode {
    None,
    Move,
    Attack,
    PlayCard,
    BoostCard
};







enum class TargetPrompt {
    None,
    MistformNode,           
    RaveningFighter,        
    RaveningNode,           
    ConfirmSuspicionValue,  
    EliminateCard,          

    
    
    
    
    
    
    DashNode,                
    AfootNode,               
    BeastformDiscard,        
    ElementaryPredict        
};

struct ActionBarState {
    ActionMode currentAction = ActionMode::None;
    int selectedCardIndex = -1;
    bool hasPendingBoost = false;
    card pendingBoostCard;
    std::vector<std::string> validMoveTargets;
    bool canMove = false;
    bool canAttack = false;
    bool canScheme = false;
    character* selectedActor = nullptr;
    bool awaitingDefense = false;
    hero* pendingAttacker = nullptr;
    hero* pendingDefender = nullptr;
    card pendingAttackCard;
    bool draculaHasCard = false;
    card draculaLastCard;
    bool sherlockHasCard = false;
    card sherlockLastCard;
    
    
    
    std::string team1Label = "DRACULA";
    std::string team2Label = "SHERLOCK";

    
    TargetPrompt targetPrompt = TargetPrompt::None;
    card pendingSchemeCard;
    int pendingSchemeCardIndex = -1;
    hero* pendingSchemeTarget = nullptr;
    character* raveningFighter = nullptr;

    
    
    
    
    
    
    
    
    
    hero* combatTargetHero = nullptr;
    bool combatTargetIsDefender = false;
    card combatChosenDefenseCard;
};

struct ActionBarLayout {
    Rectangle panel;
    Rectangle moveBtn;
    Rectangle attackBtn;
    Rectangle playCardBtn;
    Rectangle boostCardBtn;
};

ActionBarLayout ActionBar_ComputeLayout(
    Rectangle p1Panel,
    float endTurnHeight,
    float turnOrderHeight
);

Vector2 ActionBar_NodeScreenPos(
    Board& board,
    const std::string& nodeName,
    Rectangle mapDest,
    Texture2D boardTex
);

float ActionBar_NodeClickRadius(
    Rectangle mapDest,
    Texture2D boardTex
);

void ActionBar_Update(
    ActionBarState& state,
    GameManager& gm,
    const ActionBarLayout& layout,
    Vector2 mousePos,
    Rectangle mapDest,
    Texture2D boardTex,
    Rectangle handBox,
    character* actingChar,
    hero* actingHero,
    hero* activeHero,
    hero* draculaHero
);

bool ActionBar_HandleCardClick(
    ActionBarState& state,
    GameManager& gm,
    hero* activeHero,
    const card& clickedCard,
    int cardIndex,
    bool& showHandFlag,
    Board& board,
    hero* draculaHero
);

bool ActionBar_IsCardSelectedForAttack(
    const ActionBarState& state,
    int cardIndex
);

void ActionBar_DrawMapHighlights(
    const ActionBarState& state,
    GameManager& gm,
    Board& board,
    Rectangle mapDest,
    Texture2D boardTex,
    character* actingChar,
    hero* actingHero
);

void ActionBar_DrawPanel(
    const ActionBarState& state,
    const ActionBarLayout& layout
);

void ActionBar_ResetOnTurnEnd(
    ActionBarState& state
);

void ActionBar_ResetRoundCards(
    ActionBarState& state
);

void ActionBar_RecordPlayedCard(
    ActionBarState& state,
    hero* player,
    hero* draculaHero,
    const card& c
);

void ActionBar_ResolveDefense(
    ActionBarState& state,
    GameManager& gm,
    Board& board,
    hero* draculaHero,
    const card* chosenDefense
);

void ActionBar_SelectActorClick(
    ActionBarState& state,
    GameManager& gm,
    Vector2 mousePos,
    Rectangle mapDest,
    Texture2D boardTex,
    Rectangle actionsPanel,
    Rectangle handBox
);

character* ActionBar_GetActingCharacter(
    const ActionBarState& state,
    GameManager& gm
);

void ActionBar_DrawActorSelection(
    const ActionBarState& state,
    GameManager& gm,
    Board& board,
    Rectangle mapDest,
    Texture2D boardTex
);

void ActionBar_DrawCardEffectsBox(
    const ActionBarState& state,
    Rectangle box
);



bool ActionBar_IsTargeting(const ActionBarState& state);

void ActionBar_UpdateTargeting(
    ActionBarState& state,
    GameManager& gm,
    hero* activeHero,
    hero* draculaHero,
    Board& board,
    Rectangle mapDest,
    Texture2D boardTex,
    Vector2 mousePos
);

void ActionBar_DrawTargetingHighlights(
    const ActionBarState& state,
    GameManager& gm,
    Board& board,
    Rectangle mapDest,
    Texture2D boardTex
);

void ActionBar_DrawValuePicker(
    ActionBarState& state,
    GameManager& gm,
    hero* activeHero,
    hero* draculaHero,
    Vector2 mousePos,
    float sw,
    float sh
);

void ActionBar_DrawEliminatePicker(
    ActionBarState& state,
    GameManager& gm,
    hero* activeHero,
    hero* draculaHero,
    Vector2 mousePos,
    float sw,
    float sh
);






void ActionBar_DrawBeastformPicker(
    ActionBarState& state,
    Vector2 mousePos,
    float sw,
    float sh
);

void ActionBar_DrawElementaryPicker(
    ActionBarState& state,
    GameManager& gm,
    Board& board,
    hero* draculaHero,
    Vector2 mousePos,
    float sw,
    float sh
);