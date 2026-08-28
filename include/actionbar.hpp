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
    ElementaryPredict,
    // The Invisible Man's Confound, played as a scheme: click any other
    // space to move a fog token there (no distance limit, per the card).
    ConfoundNode,
    // The Invisible Man's Vanish: at the start of the turn after it was
    // played, click any space on the board to place him there (see
    // InvisibleMan::onTurnStart()/resolveVanish() -- previously this
    // auto-picked a fog token instead of letting the player choose).
    VanishNode,

    // NEW: the two per-hero "Special Ability" button flows (see the new
    // ability button drawn under each portrait). Dracula's Blood Drain
    // needs the player to click which adjacent fighter takes the damage;
    // the Invisible Man's fog-to-fog teleport needs the player to click
    // which of his other fog tokens to step onto.
    DraculaAbilityTarget,
    FogTeleportTarget
};

struct ActionBarState {
    ActionMode currentAction = ActionMode::None;
    int selectedCardIndex = -1;
    bool hasPendingBoost = false;
    card pendingBoostCard;
    std::vector<std::string> validMoveTargets;
    bool canMove = false;
    // Dracula's Blood Drain is a free, once-per-turn ability (not one of
    // his 2 actions) -- this tracks whether he's already used it this
    // turn. Reset whenever a new turn begins.
    bool draculaAbilityUsed = false;
    // Sherlock's ability is a passive that's simply always on (see
    // hero::attack()) -- this only tracks whether the small "what does
    // this do" info popup under his portrait is open.
    bool showSherlockAbilityInfo = false;
    bool canAttack = false;
    bool canScheme = false;
    character* selectedActor = nullptr;
    bool awaitingDefense = false;
    // Guards against the SAME mouse click that opened the defense modal
    // (by clicking an enemy on the map to attack) also being read as a
    // click INSIDE the modal that same frame -- IsMouseButtonPressed()
    // stays true for the whole frame it was pressed in, so without this,
    // if the attacked character's board position happened to fall under
    // where the modal draws its cards, the defense would resolve itself
    // instantly instead of waiting for a real, separate click.
    bool defenseJustOpened = false;
    hero* pendingAttacker = nullptr;
    hero* pendingDefender = nullptr;
    // NEW: when the physical attacker is a sidekick (Sister/Watson) rather
    // than a hero, pendingAttacker still holds the CONTROLLING hero (whose
    // hand/actions the attack card is drawn from), while this holds the
    // character whose board position/adjacency actually determines range.
    // nullptr means "same as pendingAttacker" (normal hero attacking).
    character* pendingAttackerPosition = nullptr;
    // Same idea for the defending side: pendingDefender is the CONTROLLING
    // hero, pendingDefenderTarget is who actually takes the damage.
    character* pendingDefenderTarget = nullptr;
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

    // Dash / The Game is Afoot / Beastform, for the case where a Sister or
    // Watson is the one physically fighting. These cards' extra choice
    // (which space to move to, how many cards to discard) can't be stored
    // on the hero object in that case -- Dash might need to move the
    // SIDEKICK, not the hero, and hero::attack()'s rich pipeline (which
    // reads dashTargetNode etc. off itself) never even runs for a sidekick
    // fight. So these three mirror that same choice here instead, and
    // ActionBar_ResolveDefense's sidekick-involved branch applies them
    // directly. -1 / -1 / 0 means "not chosen (yet)".
    int pendingDashTargetNode = -1;
    int pendingAfootTargetNode = -1;
    int pendingBeastformDiscardCount = 0;
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

bool CardOwnerMatchesCharacter(const card& c, character* actor);

bool ActionBar_HandleCardClick(
    ActionBarState& state,
    GameManager& gm,
    hero* activeHero,
    const card& clickedCard,
    int cardIndex,
    bool& showHandFlag,
    Board& board,
    hero* draculaHero,
    character* actingChar
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

// NEW: Special Ability button (drawn under each hero's portrait).
// actingChar is whichever character the player currently has selected
// (matters for the Invisible Man, since the fog-teleport is about HIS
// position specifically). Returns true if the button press started a
// targeting flow or resolved immediately (Dracula's case still needs a
// target click after this returns).
bool ActionBar_CanUseSpecialAbility(
    const ActionBarState& state,
    GameManager& gm,
    hero* activeHero,
    character* actingChar
);

void ActionBar_ActivateSpecialAbility(
    ActionBarState& state,
    GameManager& gm,
    hero* activeHero,
    character* actingChar
);

void ActionBar_UpdateTargeting(
    ActionBarState& state,
    GameManager& gm,
    hero* activeHero,
    hero* draculaHero,
    Board& board,
    Rectangle mapDest,
    Texture2D boardTex,
    Vector2 mousePos,
    character* actingChar = nullptr
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