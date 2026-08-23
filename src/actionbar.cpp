    #include "actionbar.hpp"

    #include "game_fonts.hpp"

    #include <string>

    #include <iostream>

    #include <algorithm>

    Vector2 ActionBar_NodeScreenPos(Board& board, const std::string& nodeName, Rectangle mapDest, Texture2D boardTex)

    {

        if (boardTex.width <= 0 || boardTex.height <= 0)

            return {0, 0};

        auto p = board.getCoordinates(nodeName);

        float sx = mapDest.width / static_cast<float>(boardTex.width);

        float sy = mapDest.height / static_cast<float>(boardTex.height);

        return { mapDest.x + p.first * sx, mapDest.y + p.second * sy };

    }

    float ActionBar_NodeClickRadius(Rectangle mapDest, Texture2D boardTex)

    {

        if (boardTex.width <= 0)

            return 24.0f;

        return (20.0f * (mapDest.width / static_cast<float>(boardTex.width))) * 2.4f;

    }

    ActionBarLayout ActionBar_ComputeLayout(Rectangle p1Panel, float endTurnHeight, float turnOrderHeight)

    {

        ActionBarLayout layout{};

        float bottomY = p1Panel.y + p1Panel.height + 12.0f;

        float bottomH = endTurnHeight + 12.0f + turnOrderHeight;

        layout.panel = { p1Panel.x, bottomY, p1Panel.width, bottomH };

        float gap = 8.0f;

        float titleH = 26.0f;

        float btnW = (layout.panel.width - gap) / 2.0f;

        float btnH = (layout.panel.height - titleH - gap) / 2.0f;

        layout.moveBtn = {

            layout.panel.x,

            layout.panel.y + titleH,

            btnW,

            btnH

        };

        layout.attackBtn = {

            layout.panel.x + btnW + gap,

            layout.panel.y + titleH,

            btnW,

            btnH

        };

        layout.playCardBtn = {

            layout.panel.x,

            layout.panel.y + titleH + btnH + gap,

            btnW,

            btnH

        };

        layout.boostCardBtn = {

            layout.panel.x + btnW + gap,

            layout.panel.y + titleH + btnH + gap,

            btnW,

            btnH

        };

        return layout;

    }

    static void DrawTextCenteredAB(Font font, const char* text, float centerX, float y, float size, float spacing, Color color) {

        Vector2 textSize = MeasureTextEx(font, text, size, spacing);

        DrawTextEx(font, text, { centerX - textSize.x / 2.0f, y }, size, spacing, color);

    }

    static void ToggleAction(ActionBarState& state, ActionMode mode)

    {

        if (state.currentAction == mode) {

            state.currentAction = ActionMode::None;

            state.selectedCardIndex = -1;

            state.hasPendingBoost = false;

            return;

        }

        state.currentAction = mode;

        state.selectedCardIndex = -1;

        if (mode != ActionMode::Move)

            state.hasPendingBoost = false;

    }

    static void ActionBar_CompleteAction(ActionBarState& state, GameManager& gm)

    {

        state.currentAction = ActionMode::None;

        state.selectedCardIndex = -1;

        state.hasPendingBoost = false;

        gm.getTurnManager().endTurn();

    }

    static bool CardGrantsBonusAction(const std::string& name)

    {

        return name == "Mistform" || name == "Rolling Fog";

    }

    static void ActionBar_CompleteFreeAction(ActionBarState& state)

    {

        state.currentAction = ActionMode::None;

        state.selectedCardIndex = -1;

        state.hasPendingBoost = false;

    }

    static void ActionBar_ProceedToDefense(ActionBarState& state)

    {

        state.targetPrompt = TargetPrompt::None;

        state.combatTargetHero = nullptr;

        state.combatTargetIsDefender = false;

        state.awaitingDefense = true;

        state.defenseJustOpened = true;

        state.currentAction = ActionMode::None;

        state.selectedCardIndex = -1;

    }

    void ActionBar_RecordPlayedCard(

        ActionBarState& state,

        hero* player,

        hero* draculaHero,

        const card& c

    )

    {

        if (!player)

            return;

        if (player == draculaHero) {

            state.draculaLastCard = c;

            state.draculaHasCard = true;

        } else {

            state.sherlockLastCard = c;

            state.sherlockHasCard = true;

        }

    }

    void ActionBar_ResetRoundCards(ActionBarState& state)

    {

        state.draculaHasCard = false;

        state.sherlockHasCard = false;

    }

    void ActionBar_ResetOnTurnEnd(ActionBarState& state)

    {

        state.currentAction = ActionMode::None;

        state.selectedCardIndex = -1;

        state.hasPendingBoost = false;

        state.awaitingDefense = false;

        state.defenseJustOpened = false;

        state.pendingAttacker = nullptr;

        state.pendingAttackerPosition = nullptr;

        state.pendingDefender = nullptr;

        state.pendingDefenderTarget = nullptr;

        state.pendingAttackCard = card();

        state.selectedActor = nullptr;

        state.validMoveTargets.clear();

        state.targetPrompt = TargetPrompt::None;

        state.pendingSchemeCard = card();

        state.pendingSchemeCardIndex = -1;

        state.pendingSchemeTarget = nullptr;

        state.raveningFighter = nullptr;

    }

    void ActionBar_ResolveDefense(

        ActionBarState& state,

        GameManager& gm,

        Board& board,

        hero* draculaHero,

        const card* chosenDefense

    )

    {

        if (!state.awaitingDefense || !state.pendingAttacker || !state.pendingDefender)

            return;

        hero* attacker = state.pendingAttacker;

        hero* defender = state.pendingDefender;

        card attackCard = state.pendingAttackCard;

        card noDefense("No Defense", cardtype::defense, 0, 0, 0, cardowner::any);

        card defenseCard = chosenDefense ? *chosenDefense : noDefense;

        if (chosenDefense)

            defender->removeCardFromHand(defenseCard.get_name());

        character* damageTarget = state.pendingDefenderTarget;

        character* attackerPos = state.pendingAttackerPosition;

        bool sidekickInvolved =

            (damageTarget && damageTarget != static_cast<character*>(defender)) ||

            (attackerPos && attackerPos != static_cast<character*>(attacker));

        bool resolved;

        if (sidekickInvolved) {

            character* realTarget = damageTarget ? damageTarget : static_cast<character*>(defender);

            bool effectsCanceled =

                (attackCard.get_name() == "Feint") ||

                (defenseCard.get_name() == "Feint");

            int atk = std::max(0, attackCard.getattack());

            int def = effectsCanceled ? 0 : std::max(0, defenseCard.getdefense());

            int damage = std::max(0, atk - def);

            if (damage > 0) {

                realTarget->takedamage(damage);

            }

            attacker->useAction();

            resolved = true;

            if (resolved && chosenDefense)

                ActionBar_RecordPlayedCard(state, defender, draculaHero, defenseCard);

        } else {

            resolved = attacker->attack(

                *defender,

                attackCard,

                board,

                &defenseCard

            );

            if (resolved && chosenDefense)

                ActionBar_RecordPlayedCard(state, defender, draculaHero, defenseCard);

        }

        state.awaitingDefense = false;

        state.defenseJustOpened = false;

        state.pendingAttacker = nullptr;

        state.pendingAttackerPosition = nullptr;

        state.pendingDefender = nullptr;

        state.pendingDefenderTarget = nullptr;

        state.pendingAttackCard = card();

        if (resolved) {

            ActionBar_CompleteAction(state, gm);

        } else {

            state.currentAction = ActionMode::None;

            state.selectedCardIndex = -1;

            state.hasPendingBoost = false;

        }

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

        hero* activeHero,

        hero* draculaHero

    )

    {

        if (state.awaitingDefense || state.targetPrompt != TargetPrompt::None)

            return;

        bool hasActions = gm.getActionsRemaining() > 0;

        state.canMove = hasActions && actingChar != nullptr;

        state.canAttack = hasActions && actingChar != nullptr && activeHero != nullptr;

        state.canScheme = hasActions && activeHero != nullptr;

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

            if (state.canMove && CheckCollisionPointRec(mousePos, layout.moveBtn)) {

                ToggleAction(state, ActionMode::Move);

                return;

            }

            if (state.canAttack && CheckCollisionPointRec(mousePos, layout.attackBtn)) {

                ToggleAction(state, ActionMode::Attack);

                return;

            }

            if (state.canScheme && CheckCollisionPointRec(mousePos, layout.playCardBtn)) {

                ToggleAction(state, ActionMode::PlayCard);

                return;

            }

            if (state.canMove && CheckCollisionPointRec(mousePos, layout.boostCardBtn)) {

                ToggleAction(state, ActionMode::BoostCard);

                return;

            }

        }

        state.validMoveTargets.clear();

        if (state.currentAction == ActionMode::Move && actingChar) {

            int originalMovement = actingChar->getmovement();

            if (state.hasPendingBoost)

                actingChar->setnewmovement(originalMovement + state.pendingBoostCard.getboost());

            state.validMoveTargets = gm.getValidMoves(actingChar);

            if (state.hasPendingBoost)

                actingChar->resetmovement();

        }

        Board& board = gm.getBoard();

        float clickRadius = ActionBar_NodeClickRadius(mapDest, boardTex);

        bool outsideUi =

            !CheckCollisionPointRec(mousePos, layout.panel) &&

            !CheckCollisionPointRec(mousePos, handBox);

        if (state.currentAction == ActionMode::Move &&

            actingChar &&

            outsideUi &&

            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

            for (const std::string& nodeName : state.validMoveTargets) {

                Vector2 pos = ActionBar_NodeScreenPos(board, nodeName, mapDest, boardTex);

                if (!CheckCollisionPointCircle(mousePos, pos, clickRadius))

                    continue;

                bool moved = false;

                if (state.hasPendingBoost) {

                    int originalMovement = actingChar->getmovement();

                    actingChar->setnewmovement(originalMovement + state.pendingBoostCard.getboost());

                    moved = gm.moveCharacter(actingChar, nodeName);

                    actingChar->resetmovement();

                } else {

                    moved = gm.moveCharacter(actingChar, nodeName);

                }

                if (moved) {

                    if (state.hasPendingBoost && activeHero)

                        activeHero->removeCardFromHand(state.pendingBoostCard.get_name());

                    ActionBar_CompleteAction(state, gm);

                }

                return;

            }

        }

        if (state.currentAction == ActionMode::Attack &&

            actingChar &&

            activeHero &&

            state.selectedCardIndex >= 0 &&

            outsideUi &&

            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

            auto enemies = gm.getEnemies(actingChar);

            for (character* enemyChar : enemies) {

                if (!board.isAdjacent(actingChar->getx(), enemyChar->getx()))

                    continue;

                std::string nodeName = "n" + std::to_string(enemyChar->getx());

                Vector2 pos = ActionBar_NodeScreenPos(board, nodeName, mapDest, boardTex);

                if (!CheckCollisionPointCircle(mousePos, pos, clickRadius))

                    continue;

                auto& hand = activeHero->gethand();

                if (state.selectedCardIndex >= 0 &&

                    state.selectedCardIndex < static_cast<int>(hand.size())) {

                    card attackCard = hand[state.selectedCardIndex];

                    if (attackCard.gettype() != cardtype::attack &&

                        attackCard.gettype() != cardtype::multipurpose) {

                        return;

                    }

                    hero* enemyHero = dynamic_cast<hero*>(enemyChar);

                    bool attackerIsSidekick = (actingHero == nullptr);

                    activeHero->removeCardFromHand(attackCard.get_name());

                    ActionBar_RecordPlayedCard(state, activeHero, draculaHero, attackCard);

                    state.pendingAttackCard = attackCard;

                    state.pendingAttacker = activeHero;

                    state.pendingAttackerPosition = attackerIsSidekick ? actingChar : nullptr;

                    state.currentAction = ActionMode::None;

                    state.selectedCardIndex = -1;

                    if (enemyHero) {

                        state.pendingDefender = enemyHero;

                        state.pendingDefenderTarget = nullptr;

                    } else {

                        hero* controllingHero = nullptr;

                        for (character* ally : gm.getAllies(enemyChar)) {

                            controllingHero = dynamic_cast<hero*>(ally);

                            if (controllingHero) break;

                        }

                        if (!controllingHero) {

                            int dmg = std::max(0, attackCard.getattack());

                            if (dmg > 0) enemyChar->takedamage(dmg);

                            activeHero->useAction();

                            state.pendingAttacker = nullptr;

                            state.pendingAttackerPosition = nullptr;

                            ActionBar_CompleteAction(state, gm);

                            return;

                        }

                        state.pendingDefender = controllingHero;

                        state.pendingDefenderTarget = enemyChar;

                    }

                    std::string atkName = attackCard.get_name();

                    if (!attackerIsSidekick && enemyHero && atkName == "Dash") {

                        state.combatTargetHero = actingHero;

                        state.combatTargetIsDefender = false;

                        state.targetPrompt = TargetPrompt::DashNode;

                    } else if (!attackerIsSidekick && enemyHero && atkName == "The Game is Afoot") {

                        state.combatTargetHero = actingHero;

                        state.combatTargetIsDefender = false;

                        state.targetPrompt = TargetPrompt::AfootNode;

                    } else if (!attackerIsSidekick && enemyHero && atkName == "Beastform") {

                        state.combatTargetHero = actingHero;

                        state.combatTargetIsDefender = false;

                        state.targetPrompt = TargetPrompt::BeastformDiscard;

                    } else {

                        state.awaitingDefense = true;

                        state.defenseJustOpened = true;

                    }

                }

                return;

            }

        }

    }

    static std::string ActionBar_FindNodeAtPos(

        Board& board,

        Rectangle mapDest,

        Texture2D boardTex,

        Vector2 mousePos,

        float clickRadius

    )

    {

        for (const std::string& nodeName : board.getAllSpaceIds()) {

            Vector2 pos = ActionBar_NodeScreenPos(board, nodeName, mapDest, boardTex);

            if (CheckCollisionPointCircle(mousePos, pos, clickRadius))

                return nodeName;

        }

        return "";

    }

    static character* ActionBar_FindCharacterAtPos(

        GameManager& gm,

        Board& board,

        Rectangle mapDest,

        Texture2D boardTex,

        Vector2 mousePos,

        float clickRadius

    )

    {

        for (character* c : gm.getAllCharacters()) {

            if (!c || !c->isalive())

                continue;

            std::string nodeName = "n" + std::to_string(c->getx());

            Vector2 pos = ActionBar_NodeScreenPos(board, nodeName, mapDest, boardTex);

            if (CheckCollisionPointCircle(mousePos, pos, clickRadius))

                return c;

        }

        return nullptr;

    }

    static void ActionBar_FinishPendingScheme(

        ActionBarState& state,

        GameManager& gm,

        hero* activeHero,

        hero* draculaHero

    )

    {

        state.targetPrompt = TargetPrompt::None;

        state.raveningFighter = nullptr;

        if (!activeHero || !state.pendingSchemeTarget) {

            state.pendingSchemeCard = card();

            state.pendingSchemeCardIndex = -1;

            state.pendingSchemeTarget = nullptr;

            return;

        }

        std::string cardName = state.pendingSchemeCard.get_name();

        card cardCopy = state.pendingSchemeCard;

        bool ok = activeHero->scheme(cardCopy, *state.pendingSchemeTarget);

        state.pendingSchemeCard = card();

        state.pendingSchemeCardIndex = -1;

        state.pendingSchemeTarget = nullptr;

        if (!ok) {

            return;

        }

        activeHero->removeCardFromHand(cardName);

        ActionBar_RecordPlayedCard(state, activeHero, draculaHero, cardCopy);

        if (CardGrantsBonusAction(cardName))

            ActionBar_CompleteFreeAction(state);

        else

            ActionBar_CompleteAction(state, gm);

    }

    bool ActionBar_IsTargeting(const ActionBarState& state)

    {

        return state.targetPrompt != TargetPrompt::None;

    }

    void ActionBar_UpdateTargeting(

        ActionBarState& state,

        GameManager& gm,

        hero* activeHero,

        hero* draculaHero,

        Board& board,

        Rectangle mapDest,

        Texture2D boardTex,

        Vector2 mousePos

    )

    {

        if (state.targetPrompt == TargetPrompt::None || !activeHero)

            return;

        if (state.targetPrompt == TargetPrompt::ConfirmSuspicionValue ||

            state.targetPrompt == TargetPrompt::EliminateCard ||

            state.targetPrompt == TargetPrompt::BeastformDiscard ||

            state.targetPrompt == TargetPrompt::ElementaryPredict)

            return;

        if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON))

            return;

        float clickRadius = ActionBar_NodeClickRadius(mapDest, boardTex);

        if (state.targetPrompt == TargetPrompt::MistformNode) {

            std::string node = ActionBar_FindNodeAtPos(board, mapDest, boardTex, mousePos, clickRadius);

            if (node.empty())

                return;

            activeHero->setMistformTarget(node);

            ActionBar_FinishPendingScheme(state, gm, activeHero, draculaHero);

            return;

        }

        if (state.targetPrompt == TargetPrompt::RaveningFighter) {

            character* picked = ActionBar_FindCharacterAtPos(gm, board, mapDest, boardTex, mousePos, clickRadius);

            if (!picked)

                return;

            state.raveningFighter = picked;

            state.targetPrompt = TargetPrompt::RaveningNode;

            return;

        }

        if (state.targetPrompt == TargetPrompt::RaveningNode) {

            if (!state.raveningFighter)

                return;

            std::string node = ActionBar_FindNodeAtPos(board, mapDest, boardTex, mousePos, clickRadius);

            if (node.empty())

                return;

            activeHero->setRaveningTargets(state.raveningFighter->getname(), node);

            ActionBar_FinishPendingScheme(state, gm, activeHero, draculaHero);

            return;

        }

        if (state.targetPrompt == TargetPrompt::DashNode ||

            state.targetPrompt == TargetPrompt::AfootNode) {

            if (!state.combatTargetHero)

                return;

            std::string node = ActionBar_FindNodeAtPos(board, mapDest, boardTex, mousePos, clickRadius);

            if (node.empty())

                return;

            int nodeId = board.getNodeId(node);

            for (character* c : gm.getAllCharacters()) {

                if (c && c->isalive() && c->getx() == nodeId)

                    return;

            }

            hero* mover = state.combatTargetHero;

            bool wasDefender = state.combatTargetIsDefender;

            if (state.targetPrompt == TargetPrompt::DashNode)

                mover->setDashTargetNode(nodeId);

            else

                mover->setGameIsAfootTargetNode(nodeId);

            if (wasDefender) {

                state.targetPrompt = TargetPrompt::None;

                state.combatTargetHero = nullptr;

                state.combatTargetIsDefender = false;

                state.awaitingDefense = true;

                state.defenseJustOpened = false;

                ActionBar_ResolveDefense(state, gm, board, draculaHero, &state.combatChosenDefenseCard);

            } else {

                ActionBar_ProceedToDefense(state);

            }

            return;

        }

    }

    void ActionBar_DrawTargetingHighlights(

        const ActionBarState& state,

        GameManager& gm,

        Board& board,

        Rectangle mapDest,

        Texture2D boardTex

    )

    {

        if (state.targetPrompt == TargetPrompt::None)

            return;

        float radius = ActionBar_NodeClickRadius(mapDest, boardTex);

        Color highlight = GetColor(0x9C6ADEFF);

        if (state.targetPrompt == TargetPrompt::MistformNode ||

            state.targetPrompt == TargetPrompt::RaveningNode ||

            state.targetPrompt == TargetPrompt::DashNode ||

            state.targetPrompt == TargetPrompt::AfootNode) {

            for (const std::string& nodeName : board.getAllSpaceIds()) {

                int id = board.getNodeId(nodeName);

                bool occupied = false;

                for (character* c : gm.getAllCharacters()) {

                    if (c && c->isalive() && c->getx() == id) {

                        occupied = true;

                        break;

                    }

                }

                if (occupied)

                    continue;

                Vector2 pos = ActionBar_NodeScreenPos(board, nodeName, mapDest, boardTex);

                DrawCircleLines((int)pos.x, (int)pos.y, radius, highlight);

            }

        }

        if (state.targetPrompt == TargetPrompt::RaveningFighter) {

            for (character* c : gm.getAllCharacters()) {

                if (!c || !c->isalive())

                    continue;

                Vector2 pos = ActionBar_NodeScreenPos(board, "n" + std::to_string(c->getx()), mapDest, boardTex);

                DrawCircleLines((int)pos.x, (int)pos.y, radius, highlight);

                DrawCircleLines((int)pos.x, (int)pos.y, radius - 1.0f, highlight);

            }

        }

    }

    void ActionBar_DrawValuePicker(

        ActionBarState& state,

        GameManager& gm,

        hero* activeHero,

        hero* draculaHero,

        Vector2 mousePos,

        float sw,

        float sh

    )

    {

        if (state.targetPrompt != TargetPrompt::ConfirmSuspicionValue || !activeHero)

            return;

        float modalW = 420.0f;

        float modalH = 220.0f;

        Rectangle modalRect = { (sw - modalW) / 2.0f, (sh - modalH) / 2.0f, modalW, modalH };

        DrawRectangle(0, 0, (int)sw, (int)sh, Fade(BLACK, 0.7f));

        DrawRectangleRec(modalRect, GetColor(0x0B080CFF));

        DrawRectangleLinesEx(modalRect, 3, GetColor(0x9C6ADEFF));

        const char* title = "CONFIRM SUSPICION - NAME A VALUE";

        DrawTextCenteredAB(GetTitleFont(), title, modalRect.x + modalW / 2.0f, modalRect.y + 12, 19, 0.6f, GetColor(0xE5C158FF));

        float btnSize = 46.0f;

        float gap = 10.0f;

        int count = 7;

        float totalW = count * btnSize + (count - 1) * gap;

        float startX = modalRect.x + (modalW - totalW) / 2.0f;

        float y = modalRect.y + 70.0f;

        for (int v = 0; v < count; v++) {

            Rectangle btn = { startX + v * (btnSize + gap), y, btnSize, btnSize };

            bool hover = CheckCollisionPointRec(mousePos, btn);

            DrawRectangleRec(btn, hover ? GetColor(0x9C6ADEFF) : GetColor(0x1B0A0DFF));

            DrawRectangleLinesEx(btn, 2, GetColor(0x9C6ADEFF));

            std::string label = std::to_string(v);

            DrawTextCenteredAB(GetSemiFont(), label.c_str(), btn.x + btnSize / 2.0f, btn.y + 12, 20, 0.5f, WHITE);

            if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

                activeHero->setConfirmSuspicionGuess(v);

                ActionBar_FinishPendingScheme(state, gm, activeHero, draculaHero);

                return;

            }

        }

        Rectangle cancelBtn = { modalRect.x + (modalW - 140.0f) / 2.0f, modalRect.y + modalH - 46.0f, 140.0f, 32.0f };

        bool cancelHover = CheckCollisionPointRec(mousePos, cancelBtn);

        DrawRectangleRec(cancelBtn, cancelHover ? GetColor(0x9E2230FF) : GetColor(0x1B0A0DFF));

        DrawRectangleLinesEx(cancelBtn, 2, GetColor(0x9E2230FF));

        DrawTextCenteredAB(GetSemiFont(), "CANCEL", cancelBtn.x + 70.0f, cancelBtn.y + 7, 15, 0.4f, GetColor(0xE5C158FF));

        if (cancelHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

            state.targetPrompt = TargetPrompt::None;

            state.pendingSchemeCard = card();

            state.pendingSchemeCardIndex = -1;

            state.pendingSchemeTarget = nullptr;

        }

    }

    void ActionBar_DrawEliminatePicker(

        ActionBarState& state,

        GameManager& gm,

        hero* activeHero,

        hero* draculaHero,

        Vector2 mousePos,

        float sw,

        float sh

    )

    {

        if (state.targetPrompt != TargetPrompt::EliminateCard || !activeHero)

            return;

        hero* enemyHero = state.pendingSchemeTarget;

        if (!enemyHero) {

            state.targetPrompt = TargetPrompt::None;

            return;

        }

        int count = enemyHero->handsize();

        if (count <= 0) {

            state.targetPrompt = TargetPrompt::None;

            state.pendingSchemeCard = card();

            state.pendingSchemeCardIndex = -1;

            state.pendingSchemeTarget = nullptr;

            return;

        }

        float modalW = sw * 0.5f;

        float modalH = 260.0f;

        Rectangle modalRect = { (sw - modalW) / 2.0f, (sh - modalH) / 2.0f, modalW, modalH };

        DrawRectangle(0, 0, (int)sw, (int)sh, Fade(BLACK, 0.7f));

        DrawRectangleRec(modalRect, GetColor(0x0B080CFF));

        DrawRectangleLinesEx(modalRect, 3, GetColor(0x9C6ADEFF));

        const char* title = "ELIMINATE THE IMPOSSIBLE - BURN A CARD";

        DrawTextCenteredAB(GetTitleFont(), title, modalRect.x + modalW / 2.0f, modalRect.y + 12, 19, 0.6f, GetColor(0xE5C158FF));

        float cardW = 70.0f, cardH = 100.0f, gap = 14.0f;

        float totalW = count * cardW + (count - 1) * gap;

        float startX = modalRect.x + (modalW - totalW) / 2.0f;

        float y = modalRect.y + 70.0f;

        for (int i = 0; i < count; i++) {

            Rectangle cardRect = { startX + i * (cardW + gap), y, cardW, cardH };

            bool hover = CheckCollisionPointRec(mousePos, cardRect);

            DrawRectangleRec(cardRect, GetColor(0x151218FF));

            DrawRectangleLinesEx(cardRect, hover ? 3 : 1, hover ? GetColor(0x9C6ADEFF) : GetColor(0xE5C158FF));

            DrawTextCenteredAB(GetTitleFont(), "?", cardRect.x + cardW / 2.0f, cardRect.y + cardH / 2.0f - 12, 22, 0.8f, GetColor(0xE5C158FF));

            if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

                activeHero->setEliminateCardIndex(i);

                ActionBar_FinishPendingScheme(state, gm, activeHero, draculaHero);

                return;

            }

        }

    }

    void ActionBar_DrawBeastformPicker(

        ActionBarState& state,

        Vector2 mousePos,

        float sw,

        float sh

    )

    {

        if (state.targetPrompt != TargetPrompt::BeastformDiscard || !state.combatTargetHero)

            return;

        hero* attacker = state.combatTargetHero;

        int maxDiscard = attacker->handsize();

        if (maxDiscard > 8) maxDiscard = 8;

        float modalW = 420.0f;

        float modalH = 220.0f;

        Rectangle modalRect = { (sw - modalW) / 2.0f, (sh - modalH) / 2.0f, modalW, modalH };

        DrawRectangle(0, 0, (int)sw, (int)sh, Fade(BLACK, 0.7f));

        DrawRectangleRec(modalRect, GetColor(0x0B080CFF));

        DrawRectangleLinesEx(modalRect, 3, GetColor(0x9C6ADEFF));

        const char* title = "BEASTFORM - CARDS TO DISCARD";

        DrawTextCenteredAB(GetTitleFont(), title, modalRect.x + modalW / 2.0f, modalRect.y + 12, 19, 0.6f, GetColor(0xE5C158FF));

        int count = maxDiscard + 1;

        float btnSize = 46.0f;

        float gap = 10.0f;

        float totalW = count * btnSize + (count - 1) * gap;

        float startX = modalRect.x + (modalW - totalW) / 2.0f;

        float y = modalRect.y + 70.0f;

        for (int v = 0; v <= maxDiscard; v++) {

            Rectangle btn = { startX + v * (btnSize + gap), y, btnSize, btnSize };

            bool hover = CheckCollisionPointRec(mousePos, btn);

            DrawRectangleRec(btn, hover ? GetColor(0x9C6ADEFF) : GetColor(0x1B0A0DFF));

            DrawRectangleLinesEx(btn, 2, GetColor(0x9C6ADEFF));

            std::string label = std::to_string(v);

            DrawTextCenteredAB(GetSemiFont(), label.c_str(), btn.x + btnSize / 2.0f, btn.y + 12, 20, 0.5f, WHITE);

            if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

                attacker->setBeastformDiscardCount(v);

                ActionBar_ProceedToDefense(state);

                return;

            }

        }

    }

    void ActionBar_DrawElementaryPicker(

        ActionBarState& state,

        GameManager& gm,

        Board& board,

        hero* draculaHero,

        Vector2 mousePos,

        float sw,

        float sh

    )

    {

        if (state.targetPrompt != TargetPrompt::ElementaryPredict || !state.combatTargetHero)

            return;

        hero* defender = state.combatTargetHero;

        float modalW = 460.0f;

        float modalH = 220.0f;

        Rectangle modalRect = { (sw - modalW) / 2.0f, (sh - modalH) / 2.0f, modalW, modalH };

        DrawRectangle(0, 0, (int)sw, (int)sh, Fade(BLACK, 0.7f));

        DrawRectangleRec(modalRect, GetColor(0x0B080CFF));

        DrawRectangleLinesEx(modalRect, 3, GetColor(0x9C6ADEFF));

        const char* title = "ELEMENTARY - PREDICT THE ATTACK VALUE";

        DrawTextCenteredAB(GetTitleFont(), title, modalRect.x + modalW / 2.0f, modalRect.y + 12, 19, 0.6f, GetColor(0xE5C158FF));

        int count = 7;

        float btnSize = 46.0f;

        float gap = 10.0f;

        float totalW = count * btnSize + (count - 1) * gap;

        float startX = modalRect.x + (modalW - totalW) / 2.0f;

        float y = modalRect.y + 70.0f;

        for (int v = 0; v < count; v++) {

            Rectangle btn = { startX + v * (btnSize + gap), y, btnSize, btnSize };

            bool hover = CheckCollisionPointRec(mousePos, btn);

            DrawRectangleRec(btn, hover ? GetColor(0x9C6ADEFF) : GetColor(0x1B0A0DFF));

            DrawRectangleLinesEx(btn, 2, GetColor(0x9C6ADEFF));

            std::string label = std::to_string(v);

            DrawTextCenteredAB(GetSemiFont(), label.c_str(), btn.x + btnSize / 2.0f, btn.y + 12, 20, 0.5f, WHITE);

            if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

                defender->setPredictedAttackValue(v);

                state.targetPrompt = TargetPrompt::None;

                state.combatTargetHero = nullptr;

                state.combatTargetIsDefender = false;

                state.awaitingDefense = true;

                state.defenseJustOpened = false;

                ActionBar_ResolveDefense(state, gm, board, draculaHero, &state.combatChosenDefenseCard);

                return;

            }

        }

    }

    static bool IsCardOwnerAvailable(const card& c, GameManager& gm) {

        auto isAlive = [&](const std::string& namePart) -> bool {

            bool exists = false;

            for (character* ch : gm.getAllCharacters()) {

                if (ch->getname().find(namePart) != std::string::npos) {

                    exists = true;

                    if (ch->isalive()) return true;

                }

            }

            return !exists;

        };

        switch (c.getowner()) {

            case cardowner::dracula:  return isAlive("Dracula");

            case cardowner::sherlock: return isAlive("Sherlock");

            case cardowner::watson:   return isAlive("Watson");

            case cardowner::sister:   return isAlive("Sister");

            case cardowner::any:

            default: return true;

        }

    }

    bool ActionBar_HandleCardClick(

        ActionBarState& state,

        GameManager& gm,

        hero* activeHero,

        const card& clickedCard,

        int cardIndex,

        bool& showHandFlag,

        Board& board,

        hero* draculaHero

    )

    {

        if (state.awaitingDefense || state.targetPrompt != TargetPrompt::None || !activeHero)

            return false;

        if ((state.currentAction == ActionMode::Attack || state.currentAction == ActionMode::PlayCard) &&

            !IsCardOwnerAvailable(clickedCard, gm))

            return false;

        if (state.currentAction == ActionMode::Attack) {

            if (clickedCard.gettype() != cardtype::attack &&

                clickedCard.gettype() != cardtype::multipurpose)

                return false;

            state.selectedCardIndex = cardIndex;

            showHandFlag = false;

            return true;

        }

        if (state.currentAction == ActionMode::BoostCard) {

            if (clickedCard.getboost() <= 0)

                return false;

            state.pendingBoostCard = clickedCard;

            state.hasPendingBoost = true;

            state.currentAction = ActionMode::Move;

            state.selectedCardIndex = -1;

            showHandFlag = false;

            return true;

        }

        if (state.currentAction == ActionMode::PlayCard) {

            if (clickedCard.gettype() != cardtype::scheme &&

                clickedCard.gettype() != cardtype::multipurpose)

                return false;

            hero* targetHero = nullptr;

            for (character* enemy : gm.getEnemies(activeHero)) {

                targetHero = dynamic_cast<hero*>(enemy);

                if (targetHero)

                    break;

            }

            if (!targetHero)

                return false;

            std::string name = clickedCard.get_name();

            if (name == "Mistform") {

                state.pendingSchemeCard = clickedCard;

                state.pendingSchemeCardIndex = cardIndex;

                state.pendingSchemeTarget = targetHero;

                state.targetPrompt = TargetPrompt::MistformNode;

                showHandFlag = false;

                return true;

            }

            if (name == "Ravening Seduction") {

                state.pendingSchemeCard = clickedCard;

                state.pendingSchemeCardIndex = cardIndex;

                state.pendingSchemeTarget = targetHero;

                state.raveningFighter = nullptr;

                state.targetPrompt = TargetPrompt::RaveningFighter;

                showHandFlag = false;

                return true;

            }

            if (name == "Confirm Suspicion") {

                state.pendingSchemeCard = clickedCard;

                state.pendingSchemeCardIndex = cardIndex;

                state.pendingSchemeTarget = targetHero;

                state.targetPrompt = TargetPrompt::ConfirmSuspicionValue;

                showHandFlag = false;

                return true;

            }

            if (name == "Eliminate the Impossible") {

                if (targetHero->gethand().empty())

                    return false;

                state.pendingSchemeCard = clickedCard;

                state.pendingSchemeCardIndex = cardIndex;

                state.pendingSchemeTarget = targetHero;

                state.targetPrompt = TargetPrompt::EliminateCard;

                showHandFlag = false;

                return true;

            }

            card cardCopy = clickedCard;

            bool ok = activeHero->scheme(cardCopy, *targetHero);

            if (!ok)

                return false;

            activeHero->removeCardFromHand(clickedCard.get_name());

            ActionBar_RecordPlayedCard(state, activeHero, draculaHero, clickedCard);

            showHandFlag = false;

            if (CardGrantsBonusAction(clickedCard.get_name()))

                ActionBar_CompleteFreeAction(state);

            else

                ActionBar_CompleteAction(state, gm);

            return true;

        }

        return false;

    }

    bool ActionBar_IsCardSelectedForAttack(const ActionBarState& state, int cardIndex)

    {

        return state.currentAction == ActionMode::Attack &&

            state.selectedCardIndex == cardIndex;

    }

    void ActionBar_DrawMapHighlights(

        const ActionBarState& state,

        GameManager& gm,

        Board& board,

        Rectangle mapDest,

        Texture2D boardTex,

        character* actingChar,

        hero* actingHero

    )

    {

        float radius = ActionBar_NodeClickRadius(mapDest, boardTex);

        if (state.currentAction == ActionMode::Move) {

            for (const std::string& nodeName : state.validMoveTargets) {

                Vector2 pos = ActionBar_NodeScreenPos(board, nodeName, mapDest, boardTex);

                DrawCircleLines((int)pos.x, (int)pos.y, radius, GetColor(0x4CD964FF));

                DrawCircleLines((int)pos.x, (int)pos.y, radius - 1.0f, GetColor(0x4CD964FF));

                DrawCircle((int)pos.x, (int)pos.y, 4.0f, Fade(GetColor(0x4CD964FF), 0.6f));

            }

        }

        if (state.currentAction == ActionMode::Attack &&

            actingChar &&

            state.selectedCardIndex >= 0) {

            for (character* enemyChar : gm.getEnemies(actingChar)) {

                if (!board.isAdjacent(actingChar->getx(), enemyChar->getx()))

                    continue;

                Vector2 pos = ActionBar_NodeScreenPos(

                    board,

                    "n" + std::to_string(enemyChar->getx()),

                    mapDest,

                    boardTex

                );

                DrawCircleLines((int)pos.x, (int)pos.y, radius, GetColor(0xE53935FF));

                DrawCircleLines((int)pos.x, (int)pos.y, radius - 1.0f, GetColor(0xE53935FF));

            }

        }

    }

    static void DrawActionButton(

        Rectangle rect,

        const char* label,

        Color baseColor,

        bool active,

        bool enabled

    )

    {

        Color fill = active ? baseColor : Fade(baseColor, enabled ? 0.18f : 0.06f);

        Color textColor = active ? BLACK : (enabled ? baseColor : Fade(baseColor, 0.35f));

        DrawRectangleRec(rect, fill);

        DrawRectangleLinesEx(rect, active ? 3 : 2, enabled ? baseColor : Fade(baseColor, 0.35f));

        float fontSize = rect.height >= 35 ? 16.0f : 13.0f;

        Vector2 textSize = MeasureTextEx(GetSemiFont(), label, fontSize, 0.4f);

        DrawTextEx(

            GetSemiFont(),

            label,

            { rect.x + (rect.width - textSize.x) / 2.0f, rect.y + (rect.height - fontSize) / 2.0f },

            fontSize,

            0.4f,

            textColor

        );

    }

    void ActionBar_DrawPanel(

        const ActionBarState& state,

        const ActionBarLayout& layout

    )

    {

        DrawRectangleRec(layout.panel, GetColor(0x0B080CFF));

        DrawRectangleLinesEx(layout.panel, 3, GetColor(0x342936FF));

        const char* title = "ACTIONS";

        float titleSize = 13.0f;

        Vector2 titleDims = MeasureTextEx(GetSemiFont(), title, titleSize, 0.5f);

        DrawTextEx(

            GetSemiFont(),

            title,

            { layout.panel.x + (layout.panel.width - titleDims.x) / 2.0f, layout.panel.y + 6 },

            titleSize,

            0.5f,

            GetColor(0xA39BA0FF)

        );

        DrawActionButton(layout.moveBtn, "MOVE", GetColor(0x4CD964FF), state.currentAction == ActionMode::Move, state.canMove);

        DrawActionButton(layout.attackBtn, "ATTACK", GetColor(0xE53935FF), state.currentAction == ActionMode::Attack, state.canAttack);

        DrawActionButton(layout.playCardBtn, "PLAY CARD", GetColor(0x9C6ADEFF), state.currentAction == ActionMode::PlayCard, state.canScheme);

        DrawActionButton(layout.boostCardBtn, "BOOST CARD", GetColor(0xE5A158FF), state.currentAction == ActionMode::BoostCard, state.canMove);

    }

    void ActionBar_SelectActorClick(

        ActionBarState& state,

        GameManager& gm,

        Vector2 mousePos,

        Rectangle mapDest,

        Texture2D boardTex,

        Rectangle actionsPanel,

        Rectangle handBox

    )

    {

        if (state.awaitingDefense || state.targetPrompt != TargetPrompt::None || state.currentAction != ActionMode::None)

            return;

        if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON))

            return;

        if (CheckCollisionPointRec(mousePos, actionsPanel) ||

            CheckCollisionPointRec(mousePos, handBox))

            return;

        Board& board = gm.getBoard();

        float radius = ActionBar_NodeClickRadius(mapDest, boardTex);

        auto teamChars = gm.getTurnManager().getTeamCharacters(gm.getCurrentTeam());

        for (character* c : teamChars) {

            if (!c || !c->isalive())

                continue;

            Vector2 pos = ActionBar_NodeScreenPos(

                board,

                "n" + std::to_string(c->getx()),

                mapDest,

                boardTex

            );

            if (CheckCollisionPointCircle(mousePos, pos, radius)) {

                state.selectedActor = c;

                return;

            }

        }

    }

    character* ActionBar_GetActingCharacter(

        const ActionBarState& state,

        GameManager& gm

    )

    {

        if (state.selectedActor && state.selectedActor->isalive()) {

            auto teamChars = gm.getTurnManager().getTeamCharacters(gm.getCurrentTeam());

            for (character* c : teamChars) {

                if (c == state.selectedActor)

                    return state.selectedActor;

            }

        }

        return gm.getCurrentCharacter();

    }

    void ActionBar_DrawActorSelection(

        const ActionBarState& state,

        GameManager& gm,

        Board& board,

        Rectangle mapDest,

        Texture2D boardTex

    )

    {

        if (state.currentAction != ActionMode::None)

            return;

        float radius = ActionBar_NodeClickRadius(mapDest, boardTex);

        character* effectiveActor = ActionBar_GetActingCharacter(state, gm);

        auto teamChars = gm.getTurnManager().getTeamCharacters(gm.getCurrentTeam());

        for (character* c : teamChars) {

            if (!c || !c->isalive())

                continue;

            Vector2 pos = ActionBar_NodeScreenPos(

                board,

                "n" + std::to_string(c->getx()),

                mapDest,

                boardTex

            );

            bool selected = c == effectiveActor;

            Color ring = selected

                ? GetColor(0xE5C158FF)

                : Fade(GetColor(0xE5C158FF), 0.35f);

            DrawCircleLines((int)pos.x, (int)pos.y, radius + 4.0f, ring);

            if (selected)

                DrawCircleLines((int)pos.x, (int)pos.y, radius + 5.0f, ring);

        }

    }

    static void DrawCardEffectsCell(

        Rectangle cell,

        const char* ownerLabel,

        bool hasCard,

        const card& c,

        Color accent

    )

    {

        DrawRectangleRec(cell, GetColor(0x0B080CFF));

        DrawRectangleLinesEx(cell, 2, accent);

        DrawTextEx(GetSemiFont(), ownerLabel, { cell.x + 8, cell.y + 5 }, 17.6, 0.4f, accent);

        if (!hasCard) {

            DrawTextEx(GetRegularFont(), "No card played yet", { cell.x + 8, cell.y + 26 }, 17.6, 0.3f, GetColor(0xA39BA0FF));

            return;

        }

        DrawTextEx(GetSemiFont(), c.get_name().c_str(), { cell.x + 8, cell.y + 26 }, 20.2, 0.4f, GetColor(0xE5C158FF));

        std::string stats;

        if (c.gettype() == cardtype::attack) {

            stats = "ATK " + std::to_string(c.getattack()) + "  BOOST " + std::to_string(c.getboost());

        } else if (c.gettype() == cardtype::defense) {

            stats = "DEF " + std::to_string(c.getdefense()) + "  BOOST " + std::to_string(c.getboost());

        } else if (c.gettype() == cardtype::multipurpose) {

            stats = "ATK/DEF " + std::to_string(c.getattack()) + "/" + std::to_string(c.getdefense());

        } else {

            stats = "BOOST " + std::to_string(c.getboost());

        }

        DrawTextEx(GetRegularFont(), stats.c_str(), { cell.x + 8, cell.y + 47 }, 17.6, 0.3f, GetColor(0xC2B6B9FF));

        std::string effect = c.geteffect();

        if (!effect.empty()) {

            DrawTextEx(GetRegularFont(), effect.c_str(), { cell.x + 8, cell.y + 65 }, 16.2, 0.2f, GetColor(0xA39BA0FF));

        }

    }

    void ActionBar_DrawCardEffectsBox(

        const ActionBarState& state,

        Rectangle box

    )

    {

        DrawRectangleRec(box, GetColor(0x0B080CFF));

        DrawRectangleLinesEx(box, 3, GetColor(0x342936FF));

        const char* title = "CARD EFFECTS";

        float titleSize = 14.0f;

        Vector2 titleDims = MeasureTextEx(GetSemiFont(), title, titleSize, 0.5f);

        DrawTextEx(

            GetSemiFont(),

            title,

            { box.x + (box.width - titleDims.x) / 2.0f, box.y + 6 },

            titleSize,

            0.5f,

            GetColor(0xA39BA0FF)

        );

        float gap = 8.0f;

        float inner = 8.0f;

        float cellW = (box.width - inner * 2.0f - gap) / 2.0f;

        float cellH = box.height - 34.0f;

        if (cellH < 20.0f)

            cellH = 20.0f;

        Rectangle draculaCell = {

            box.x + inner,

            box.y + 28.0f,

            cellW,

            cellH

        };

        Rectangle sherlockCell = {

            draculaCell.x + cellW + gap,

            draculaCell.y,

            cellW,

            cellH

        };

        DrawCardEffectsCell(

            draculaCell,

            state.team1Label.c_str(),

            state.draculaHasCard,

            state.draculaLastCard,

            GetColor(0x9E2230FF)

        );

        DrawCardEffectsCell(

            sherlockCell,

            state.team2Label.c_str(),

            state.sherlockHasCard,

            state.sherlockLastCard,

            GetColor(0x28558FFF)

        );

    }