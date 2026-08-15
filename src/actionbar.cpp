#include "actionbar.hpp"
#include <string>
#include <iostream>

Vector2 ActionBar_NodeScreenPos(Board& board, const std::string& nodeName, Rectangle mapDest, Texture2D boardTex) {
    auto p = board.getCoordinates(nodeName);
    float sX = mapDest.width  / (float)boardTex.width;
    float sY = mapDest.height / (float)boardTex.height;
    return Vector2{ mapDest.x + p.first * sX, mapDest.y + p.second * sY };
}

float ActionBar_NodeClickRadius(Rectangle mapDest, Texture2D boardTex) {
    return (20.0f * (mapDest.width / (float)boardTex.width)) * 2.4f;
}

ActionBarLayout ActionBar_ComputeLayout(Rectangle p1Panel, float endTurnHeight, float turnOrderHeight) {
    ActionBarLayout layout;
    layout.panel = { p1Panel.x, p1Panel.y + p1Panel.height + 12, p1Panel.width, endTurnHeight + 12 + turnOrderHeight };

    float gap = 8.0f;
    float btnW = (layout.panel.width - gap) / 2.0f;
    float btnH = (layout.panel.height - 26 - gap) / 2.0f;

    layout.moveBtn      = { layout.panel.x,             layout.panel.y + 26,             btnW, btnH };
    layout.attackBtn    = { layout.panel.x + btnW + gap, layout.panel.y + 26,             btnW, btnH };
    layout.playCardBtn  = { layout.panel.x,             layout.panel.y + 26 + btnH + gap, btnW, btnH };
    layout.boostCardBtn = { layout.panel.x + btnW + gap, layout.panel.y + 26 + btnH + gap, btnW, btnH };
    return layout;
}

static void ToggleAction(ActionBarState& state, ActionMode m) {
    state.currentAction = (state.currentAction == m) ? ActionMode::None : m;
    state.selectedCardIndex = -1;
    state.hasPendingBoost = (state.currentAction == ActionMode::Move) ? state.hasPendingBoost : false;
}

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
    hero* activeHero
) {
    TurnManager& tm = gm.getTurnManager();
    bool hasActionsLeft = gm.getActionsRemaining() > 0;
    state.canMove   = hasActionsLeft;
    state.canAttack = hasActionsLeft;
    state.canScheme = hasActionsLeft;

    if (state.canMove && CheckCollisionPointRec(mousePos, layout.moveBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        ToggleAction(state, ActionMode::Move);
    }
    if (state.canAttack && CheckCollisionPointRec(mousePos, layout.attackBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        ToggleAction(state, ActionMode::Attack);
    }
    if (state.canScheme && CheckCollisionPointRec(mousePos, layout.playCardBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        ToggleAction(state, ActionMode::PlayCard);
    }
    if (state.canMove && CheckCollisionPointRec(mousePos, layout.boostCardBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        ToggleAction(state, ActionMode::BoostCard);
    }

    state.validMoveTargets.clear();
    if (state.currentAction == ActionMode::Move && actingChar) {
        if (state.hasPendingBoost) {
            int boostedMove = actingChar->getmovement() + state.pendingBoostCard.getboost();
            actingChar->setnewmovement(boostedMove);
            state.validMoveTargets = gm.getValidMoves(actingChar);
            actingChar->resetmovement();
        } else {
            state.validMoveTargets = gm.getValidMoves(actingChar);
        }
    }

    Board& board = gm.getBoard();
    float clickRadius = ActionBar_NodeClickRadius(mapDest, boardTex);

    if (state.currentAction == ActionMode::Move && actingChar && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)
        && !CheckCollisionPointRec(mousePos, layout.panel) && !CheckCollisionPointRec(mousePos, handBox)) {
        for (const auto& nodeName : state.validMoveTargets) {
            Vector2 npos = ActionBar_NodeScreenPos(board, nodeName, mapDest, boardTex);
            if (CheckCollisionPointCircle(mousePos, npos, clickRadius)) {
                std::cout << "[DEBUG] attempting move with actingChar=" << (actingChar ? actingChar->getname() : "NULL") << std::endl;
                const card* boostPtr = state.hasPendingBoost ? &state.pendingBoostCard : nullptr;
                bool moved = gm.moveCharacter(actingChar, nodeName, boostPtr);
                if (moved) {
                    if (state.hasPendingBoost && activeHero) {
                        activeHero->removeCardFromHand(state.pendingBoostCard.get_name());
                    }
                    state.currentAction = ActionMode::None;
                    state.selectedCardIndex = -1;
                    state.hasPendingBoost = false;
                }
                break;
            }
        }
    }

    if (state.currentAction == ActionMode::Attack && actingHero && state.selectedCardIndex >= 0
        && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !CheckCollisionPointRec(mousePos, layout.panel)) {
        for (character* enemyChar : gm.getEnemies(actingChar)) {
            hero* enemyHero = dynamic_cast<hero*>(enemyChar);
            if (!enemyHero) continue; 
            Vector2 epos = ActionBar_NodeScreenPos(board, "n" + std::to_string(enemyChar->getx()), mapDest, boardTex);
            if (CheckCollisionPointCircle(mousePos, epos, clickRadius)) {
                auto& handRef = actingHero->gethand();
                if (state.selectedCardIndex < (int)handRef.size()) {
                    std::string cardName = handRef[state.selectedCardIndex].get_name();
                    actingHero->attack(*enemyHero, handRef[state.selectedCardIndex], board);
                    actingHero->removeCardFromHand(cardName);
                    tm.endTurn(); 
                }
                state.currentAction = ActionMode::None;
                state.selectedCardIndex = -1;
                break;
            }
        }
    }
}

bool ActionBar_HandleCardClick(
    ActionBarState& state,
    GameManager& gm,
    hero* activeHero,
    const card& clickedCard,
    int cardIndex,
    bool& showHandFlag
) {
    if (state.currentAction == ActionMode::Attack) {
        state.selectedCardIndex = cardIndex;
        showHandFlag = false;
        return true;
    }
    else if (state.currentAction == ActionMode::PlayCard && activeHero) {
        activeHero->removeCardFromHand(clickedCard.get_name());
        gm.getTurnManager().endTurn();
        state.currentAction = ActionMode::None;
        state.selectedCardIndex = -1;
        showHandFlag = false;
        return true;
    }
    else if (state.currentAction == ActionMode::BoostCard) {
        state.pendingBoostCard = clickedCard;
        state.hasPendingBoost = true;
        state.currentAction = ActionMode::Move;
        state.selectedCardIndex = -1;
        showHandFlag = false;
        return true;
    }
    return false;
}

bool ActionBar_IsCardSelectedForAttack(const ActionBarState& state, int cardIndex) {
    return state.currentAction == ActionMode::Attack && state.selectedCardIndex == cardIndex;
}

void ActionBar_DrawMapHighlights(
    const ActionBarState& state,
    GameManager& gm,
    Board& board,
    Rectangle mapDest,
    Texture2D boardTex,
    character* actingChar,
    hero* actingHero
) {
    float clickRadius = ActionBar_NodeClickRadius(mapDest, boardTex);

    if (state.currentAction == ActionMode::Move) {
        for (const auto& nodeName : state.validMoveTargets) {
            Vector2 npos = ActionBar_NodeScreenPos(board, nodeName, mapDest, boardTex);
            DrawCircleLines((int)npos.x, (int)npos.y, clickRadius, GetColor(0x4CD964FF));
            DrawCircleLines((int)npos.x, (int)npos.y, clickRadius - 1.0f, GetColor(0x4CD964FF));
            DrawCircle((int)npos.x, (int)npos.y, 4.0f, Fade(GetColor(0x4CD964FF), 0.6f));
        }
    }

    if (state.currentAction == ActionMode::Attack && actingHero && state.selectedCardIndex >= 0) {
        for (character* enemyChar : gm.getEnemies(actingChar)) {
            if (!dynamic_cast<hero*>(enemyChar)) continue;
            Vector2 epos = ActionBar_NodeScreenPos(board, "n" + std::to_string(enemyChar->getx()), mapDest, boardTex);
            DrawCircleLines((int)epos.x, (int)epos.y, clickRadius, GetColor(0xE53935FF));
            DrawCircleLines((int)epos.x, (int)epos.y, clickRadius - 1.0f, GetColor(0xE53935FF));
        }
    }
}

static void DrawActionButton(Rectangle rect, const char* label, Color baseColor, bool active, bool enabled) {
    Color fill = active ? baseColor : Fade(baseColor, enabled ? 0.18f : 0.06f);
    Color textColor = active ? BLACK : (enabled ? baseColor : Fade(baseColor, 0.35f));
    DrawRectangleRec(rect, fill);
    DrawRectangleLinesEx(rect, active ? 3 : 2, enabled ? baseColor : Fade(baseColor, 0.35f));
    int tw = MeasureText(label, 12);
    DrawText(label, (int)(rect.x + (rect.width - tw) / 2), (int)(rect.y + rect.height / 2 - 6), 12, textColor);
}

void ActionBar_DrawPanel(const ActionBarState& state, const ActionBarLayout& layout) {
    DrawRectangleRec(layout.panel, GetColor(0x0B080CFF));
    DrawRectangleLinesEx(layout.panel, 3, GetColor(0x342936FF));
    DrawText("ACTIONS", (int)(layout.panel.x + (layout.panel.width - MeasureText("ACTIONS", 10)) / 2), (int)(layout.panel.y + 8), 10, GetColor(0x8A8085FF));

    DrawActionButton(layout.moveBtn,      "MOVE",       GetColor(0x4CD964FF), state.currentAction == ActionMode::Move,     state.canMove);
    DrawActionButton(layout.attackBtn,    "ATTACK",     GetColor(0xE53935FF), state.currentAction == ActionMode::Attack,   state.canAttack);
    DrawActionButton(layout.playCardBtn,  "PLAY CARD",  GetColor(0x9C6ADEFF), state.currentAction == ActionMode::PlayCard, state.canScheme);
    DrawActionButton(layout.boostCardBtn, "BOOST CARD", GetColor(0xE5A158FF), state.currentAction == ActionMode::BoostCard, state.canMove);

    if (state.hasPendingBoost) {
        std::string boostLbl = "Boost ready: " + state.pendingBoostCard.get_name();
        DrawText(boostLbl.c_str(), (int)layout.panel.x + 4, (int)(layout.panel.y + layout.panel.height - 14), 9, GetColor(0xE5C158FF));
    }
}

void ActionBar_ResetOnTurnEnd(ActionBarState& state) {
    state.currentAction = ActionMode::None;
    state.selectedCardIndex = -1;
    state.hasPendingBoost = false;
    state.selectedActor = nullptr; 
}

void ActionBar_SelectActorClick(
    ActionBarState& state,
    GameManager& gm,
    Vector2 mousePos,
    Rectangle mapDest,
    Texture2D boardTex,
    Rectangle actionsPanel,
    Rectangle handBox
) {
    if (state.currentAction != ActionMode::None) return; 
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;
    if (CheckCollisionPointRec(mousePos, actionsPanel) || CheckCollisionPointRec(mousePos, handBox)) return;

    Board& board = gm.getBoard();
    float clickRadius = ActionBar_NodeClickRadius(mapDest, boardTex);
    auto teamChars = gm.getTurnManager().getTeamCharacters(gm.getCurrentTeam());

    std::cout << "[DEBUG] click at (" << mousePos.x << "," << mousePos.y << "), team=" << gm.getCurrentTeam()
               << ", teamChars.size()=" << teamChars.size() << ", radius=" << clickRadius << std::endl;

    for (character* c : teamChars) {
        if (!c || !c->isalive()) continue;
        Vector2 pos = ActionBar_NodeScreenPos(board, "n" + std::to_string(c->getx()), mapDest, boardTex);
        std::cout << "[DEBUG]   candidate: " << c->getname() << " at n" << c->getx()
                   << " -> screen(" << pos.x << "," << pos.y << ")" << std::endl;
        if (CheckCollisionPointCircle(mousePos, pos, clickRadius)) {
            state.selectedActor = c;
            std::cout << "[DEBUG]   >>> SELECTED: " << c->getname() << std::endl;
            break;
        }
    }
}

character* ActionBar_GetActingCharacter(const ActionBarState& state, GameManager& gm) {
    if (state.selectedActor && state.selectedActor->isalive()) {
        auto teamChars = gm.getTurnManager().getTeamCharacters(gm.getCurrentTeam());
        for (character* c : teamChars) {
            if (c == state.selectedActor) return state.selectedActor;
        }
    }
    return gm.getCurrentCharacter(); 
}

void ActionBar_DrawActorSelection(const ActionBarState& state, GameManager& gm, Board& board, Rectangle mapDest, Texture2D boardTex) {
    if (state.currentAction != ActionMode::None) return; 
    float clickRadius = ActionBar_NodeClickRadius(mapDest, boardTex);
    character* effectiveActor = ActionBar_GetActingCharacter(state, gm);
    auto teamChars = gm.getTurnManager().getTeamCharacters(gm.getCurrentTeam());

    for (character* c : teamChars) {
        if (!c || !c->isalive()) continue;
        Vector2 pos = ActionBar_NodeScreenPos(board, "n" + std::to_string(c->getx()), mapDest, boardTex);
        bool isSelected = (c == effectiveActor);
        Color ringColor = isSelected ? GetColor(0xE5C158FF) : Fade(GetColor(0xE5C158FF), 0.35f);
        DrawCircleLines((int)pos.x, (int)pos.y, clickRadius + 4.0f, ringColor);
        if (isSelected) {
            DrawCircleLines((int)pos.x, (int)pos.y, clickRadius + 5.0f, ringColor);
        }
    }
}