#include "raylib.h"
#include "../include/map.hpp"
#include "../include/character.hpp"
#include "../include/hero.hpp"
#include "../include/game_manager.hpp"
#include "../include/invisible_man.hpp"
#include "actionbar.hpp"
#include "game_fonts.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <cmath>

static Font g_titleFont = {0};
static Font g_semiFont = {0};
static Font g_regularFont = {0};
static bool g_fontsLoaded = false;

void LoadGameFonts() {
    if (g_fontsLoaded) return;

    g_titleFont   = LoadFontEx("assets/fonts/Cinzel-Black.ttf", 110, nullptr, 0);
    g_semiFont    = LoadFontEx("assets/fonts/Cinzel-SemiBold.ttf", 56, nullptr, 0);
    g_regularFont = LoadFontEx("assets/fonts/Cinzel-Regular.ttf", 56, nullptr, 0);

    if (g_titleFont.texture.id != 0)   SetTextureFilter(g_titleFont.texture, TEXTURE_FILTER_BILINEAR);
    if (g_semiFont.texture.id != 0)    SetTextureFilter(g_semiFont.texture, TEXTURE_FILTER_BILINEAR);
    if (g_regularFont.texture.id != 0) SetTextureFilter(g_regularFont.texture, TEXTURE_FILTER_BILINEAR);

    g_fontsLoaded = true;
}

void UnloadGameFonts() {
    if (!g_fontsLoaded) return;
    UnloadFont(g_titleFont);
    UnloadFont(g_semiFont);
    UnloadFont(g_regularFont);
    g_fontsLoaded = false;
}

Font GetTitleFont()   { return g_titleFont; }
Font GetSemiFont()    { return g_semiFont; }
Font GetRegularFont() { return g_regularFont; }

static std::string HeroFactionLabel(hero* h) {
    if (!h) return "";
    std::string upper = h->getname();
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    return upper;
}

static void DrawTextCentered(Font font, const char* text, float centerX, float y, float size, float spacing, Color color) {
    Vector2 textSize = MeasureTextEx(font, text, size, spacing);
    DrawTextEx(font, text, { centerX - textSize.x / 2.0f, y }, size, spacing, color);
}

// The "Special Ability" button drawn under each hero's portrait, anchored
// to the very bottom of that panel so it never disturbs the Sisters/Watson
// layout above it. Dracula: click, then click an adjacent fighter (Blood
// Drain). Invisible Man: click, then click one of his other fog tokens
// (fog-to-fog "teleport", counts as his normal move). Sherlock: always
// clickable info popup, since his ability is a permanent passive, not
// something you turn on.
static void DrawSpecialAbilityButton(
    ActionBarState& actionBar,
    GameManager& gm,
    hero* teamHero,
    character* actingChar,
    bool isActiveTeam,
    bool gameOver,
    Rectangle panel,
    Vector2 mousePos
) {
    if (!teamHero) return;

    bool infoOnly = (teamHero->getname() == "Sherlock Holmes");
    bool enabled = !gameOver && (infoOnly || (isActiveTeam && ActionBar_CanUseSpecialAbility(actionBar, gm, teamHero, actingChar)));

    float btnH = 42.0f;
    Rectangle btn = { panel.x + 14.0f, panel.y + panel.height - btnH - 10.0f, panel.width - 28.0f, btnH };

    Color base = GetColor(0xE5C158FF);
    bool hover = enabled && CheckCollisionPointRec(mousePos, btn);

    DrawRectangleRec(btn, enabled ? (hover ? Fade(base, 0.30f) : Fade(base, 0.14f)) : Fade(base, 0.05f));
    DrawRectangleLinesEx(btn, hover ? 3 : 2, enabled ? base : Fade(base, 0.3f));
    DrawTextCentered(GetSemiFont(), "SPECIAL ABILITY", btn.x + btn.width / 2.0f, btn.y + btn.height / 2.0f - 8.0f, 14, 0.5f, enabled ? base : Fade(base, 0.4f));

    if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        ActionBar_ActivateSpecialAbility(actionBar, gm, teamHero, isActiveTeam ? actingChar : nullptr);
    }
}

static void DrawSherlockAbilityInfo(ActionBarState& actionBar, Vector2 mousePos, float sw, float sh) {
    if (!actionBar.showSherlockAbilityInfo) return;

    float modalW = sw * 0.42f;
    float modalH = 190.0f;
    Rectangle modalRect = { (sw - modalW) / 2.0f, (sh - modalH) / 2.0f, modalW, modalH };

    DrawRectangle(0, 0, (int)sw, (int)sh, Fade(BLACK, 0.65f));
    DrawRectangleRec(modalRect, GetColor(0x0B080CFF));
    DrawRectangleLinesEx(modalRect, 3, GetColor(0x28558FFF));

    DrawTextCentered(GetTitleFont(), "SHERLOCK'S ABILITY", modalRect.x + modalW / 2.0f, modalRect.y + 16, 20, 0.8f, GetColor(0xE5C158FF));
    DrawTextCentered(GetRegularFont(), "Always active - no need to trigger it.", modalRect.x + modalW / 2.0f, modalRect.y + 52, 14, 0.4f, GetColor(0xC2B6B9FF));
    DrawTextCentered(GetRegularFont(), "Cards belonging to Holmes or Watson can", modalRect.x + modalW / 2.0f, modalRect.y + 80, 13, 0.3f, GetColor(0xC2B6B9FF));
    DrawTextCentered(GetRegularFont(), "never be canceled by the opponent's Feint", modalRect.x + modalW / 2.0f, modalRect.y + 100, 13, 0.3f, GetColor(0xC2B6B9FF));
    DrawTextCentered(GetRegularFont(), "or similar cancel-effect cards.", modalRect.x + modalW / 2.0f, modalRect.y + 120, 13, 0.3f, GetColor(0xC2B6B9FF));

    Rectangle closeBtn = { modalRect.x + (modalW - 130.0f) / 2.0f, modalRect.y + modalH - 44.0f, 130.0f, 30.0f };
    bool hover = CheckCollisionPointRec(mousePos, closeBtn);
    DrawRectangleRec(closeBtn, hover ? GetColor(0x28558FFF) : Fade(GetColor(0x28558FFF), 0.3f));
    DrawRectangleLinesEx(closeBtn, 2, GetColor(0x28558FFF));
    DrawTextCentered(GetSemiFont(), "GOT IT", closeBtn.x + 65.0f, closeBtn.y + 7, 14, 0.5f, GetColor(0xE5C158FF));

    if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        actionBar.showSherlockAbilityInfo = false;
    }
}

void DrawGothicHealthBar(float x, float y, float width, int currentHp, int maxHp) {
    float percent = (maxHp > 0) ? (float)currentHp / (float)maxHp : 0.0f;
    if (percent < 0.0f) percent = 0.0f;
    if (percent > 1.0f) percent = 1.0f;

    DrawRectangleLines((int)x - 1, (int)y - 1, (int)width + 2, 11, GetColor(0x3A2B32FF));
    DrawRectangle((int)x, (int)y, (int)width, 9, GetColor(0x0E0A0DFF));
    DrawRectangle((int)x, (int)y, (int)(width * percent), 9, GetColor(0x7A0010FF));
    DrawText(TextFormat("HP: %d/%d", currentHp, maxHp), (int)x, (int)y + 12, 10, GetColor(0xC2B6B9FF));
}

struct CharAnimState {
    float currentX = 0.0f, currentY = 0.0f;
    float targetX = 0.0f, targetY = 0.0f;
    float scale = 1.0f;
    bool initialized = false;
    bool moving = false;
};

static std::unordered_map<character*, CharAnimState> g_charAnims;

static const float CHAR_ANIM_SPEED = 8.0f;
static const float CHAR_ANIM_SNAP_DIST = 0.5f;

void UpdateCharacterAnim(character* c, Board& board, Rectangle mapDest, float dt, float refWidth, float refHeight) {
    if (!c) return;

    int nodeId = c->getposition();
    std::string nodeName = "n" + std::to_string(nodeId);
    auto pos = board.getCoordinates(nodeName);
    if (pos.first < 0 || pos.second < 0) return;

    float scaleX = mapDest.width / refWidth;
    float scaleY = mapDest.height / refHeight;

    float targetX = mapDest.x + (pos.first * scaleX);
    float targetY = mapDest.y + (pos.second * scaleY);

    CharAnimState& st = g_charAnims[c];

    if (!st.initialized) {
        st.currentX = targetX;
        st.currentY = targetY;
        st.targetX = targetX;
        st.targetY = targetY;
        st.scale = 1.0f;
        st.initialized = true;
        st.moving = false;
        return;
    }

    if (targetX != st.targetX || targetY != st.targetY) {
        st.targetX = targetX;
        st.targetY = targetY;
        st.moving = true;
    }

    if (st.moving) {
        float dx = st.targetX - st.currentX;
        float dy = st.targetY - st.currentY;
        float dist = sqrtf(dx * dx + dy * dy);

        if (dist < CHAR_ANIM_SNAP_DIST) {
            st.currentX = st.targetX;
            st.currentY = st.targetY;
            st.moving = false;
            st.scale = 1.25f;
        } else {
            float t = 1.0f - expf(-CHAR_ANIM_SPEED * dt);
            st.currentX += dx * t;
            st.currentY += dy * t;
            st.scale = 1.1f;
        }
    } else {
        st.scale += (1.0f - st.scale) * fminf(1.0f, dt * 10.0f);
    }
}

void UpdateCharacterAnimFixed(character* c, float relX, float relY, Rectangle mapDest, float dt) {
    if (!c) return;

    float targetX = mapDest.x + (relX * mapDest.width);
    float targetY = mapDest.y + (relY * mapDest.height);

    CharAnimState& st = g_charAnims[c];

    if (!st.initialized) {
        st.currentX = targetX;
        st.currentY = targetY;
        st.targetX = targetX;
        st.targetY = targetY;
        st.scale = 1.0f;
        st.initialized = true;
        st.moving = false;
        return;
    }

    if (targetX != st.targetX || targetY != st.targetY) {
        st.targetX = targetX;
        st.targetY = targetY;
        st.moving = true;
    }

    if (st.moving) {
        float dx = st.targetX - st.currentX;
        float dy = st.targetY - st.currentY;
        float dist = sqrtf(dx * dx + dy * dy);

        if (dist < CHAR_ANIM_SNAP_DIST) {
            st.currentX = st.targetX;
            st.currentY = st.targetY;
            st.moving = false;
            st.scale = 1.25f;
        } else {
            float t = 1.0f - expf(-CHAR_ANIM_SPEED * dt);
            st.currentX += dx * t;
            st.currentY += dy * t;
            st.scale = 1.1f;
        }
    } else {
        st.scale += (1.0f - st.scale) * fminf(1.0f, dt * 10.0f);
    }
}

void DrawCharacterOnNode(character* c, Texture2D avatarTex, Color borderColor, float mapWidth, float sizeMultiplier, float refWidth) {
    if (!c || !c->isalive()) return;

    auto it = g_charAnims.find(c);
    if (it == g_charAnims.end()) return;

    CharAnimState& st = it->second;

    float scaleX = mapWidth / refWidth;
    float renderX = st.currentX;
    float renderY = st.currentY;
    float radius = (20.0f * scaleX) * st.scale * sizeMultiplier;

    if (st.moving) {
        DrawCircle((int)renderX, (int)renderY + 4, radius * 0.9f, Fade(BLACK, 0.35f));
    }

    DrawCircle((int)renderX, (int)renderY, radius + 2.0f, BLACK);
    DrawCircle((int)renderX, (int)renderY, radius, GetColor(0x1C1C24FF));

    if (avatarTex.id > 0) {
        Rectangle srcRec = { 0, 0, (float)avatarTex.width, (float)avatarTex.height };
        Rectangle destRec = { renderX - radius, renderY - radius, radius * 2.0f, radius * 2.0f };
        DrawTexturePro(avatarTex, srcRec, destRec, Vector2{ 0, 0 }, 0.0f, WHITE);
    } else {
        std::string initial = c->getname().substr(0, 1);
        DrawText(initial.c_str(), (int)renderX - 5, (int)renderY - 7, 14, WHITE);
    }

    Color ring = st.moving ? GetColor(0xE5C158FF) : borderColor;
    DrawCircleLines((int)renderX, (int)renderY, radius, ring);
    DrawCircleLines((int)renderX, (int)renderY, radius + 1.0f, ring);
}

// Fog tokens don't animate/move every frame like fighters do, so they don't
// need the CharAnimState smoothing that DrawCharacterOnNode relies on --
// they're just drawn straight at the node's board coordinates, nudged
// toward the corner of the space so the token stays visible even when a
// fighter is also standing on that node.
void DrawFogTokenOnNode(int nodeId, Board& board, Rectangle mapDest, float refWidth, float refHeight, Texture2D tex) {
    std::string nodeName = "n" + std::to_string(nodeId);
    auto pos = board.getCoordinates(nodeName);
    if (pos.first < 0 || pos.second < 0) return;

    float scaleX = mapDest.width / refWidth;
    float scaleY = mapDest.height / refHeight;

    float centerX = mapDest.x + (pos.first * scaleX);
    float centerY = mapDest.y + (pos.second * scaleY);

    float radius = 12.0f * scaleX;
    float renderX = centerX - radius * 0.9f;
    float renderY = centerY - radius * 0.9f;

    if (tex.id > 0) {
        Rectangle srcRec = { 0, 0, (float)tex.width, (float)tex.height };
        Rectangle destRec = { renderX - radius, renderY - radius, radius * 2.0f, radius * 2.0f };
        DrawTexturePro(tex, srcRec, destRec, Vector2{ 0, 0 }, 0.0f, Fade(WHITE, 0.92f));
    } else {
        DrawCircle((int)renderX, (int)renderY, radius, Fade(GetColor(0xB8C4D0FF), 0.75f));
        DrawCircleLines((int)renderX, (int)renderY, radius, GetColor(0xE5C158FF));
    }
}

Texture2D LoadTextureWithFallbacksForMain(const std::string& category, const std::vector<std::string>& filenames) {
    std::vector<std::string> basePaths = {
        "assets/", "assets/heroes/", "assets/images/",
        "../assets/", "../assets/heroes/", "../assets/images/",
        "build/assets/", "build/assets/heroes/"
    };
    std::vector<std::string> categoryVariants;
    if (!category.empty()) {
        categoryVariants.push_back(category + "/");
        if (category.back() != 's')
            categoryVariants.push_back(category + "s/");
    }
    categoryVariants.push_back("");

    for (const auto& filename : filenames) {
        for (const auto& basePath : basePaths) {
            for (const auto& catVariant : categoryVariants) {
                std::string fullPath = basePath + catVariant + filename;
                if (FileExists(fullPath.c_str())) {
                    Texture2D tex = LoadTexture(fullPath.c_str());
                    if (tex.id > 0) {
                        SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);
                        return tex;
                    }
                }
            }
        }
    }
    return Texture2D{ 0 };
}

static std::string GetCardFilename(const std::string& cardName, const std::string& faction);

static void DrawDiscardModal(hero* activeHero, hero* draculaHero,
                              std::unordered_map<std::string, Texture2D>& cardTextures,
                              std::vector<Texture2D>& loadedCardTextures,
                              Vector2 mousePos, float sw, float sh) {
    if (!activeHero) return;

    auto& hand = activeHero->gethand();

    const int DISCARD_STOP_AT = 6;
    if ((int)hand.size() <= 7) return;

    int overBy = (int)hand.size() - DISCARD_STOP_AT;
    if (overBy <= 0) return;

    std::string faction = HeroFactionLabel(activeHero);

    float modalW = sw * 0.75f;
    float modalH = sh * 0.55f;
    float modalX = (sw - modalW) / 2.0f;
    float modalY = (sh - modalH) / 2.0f;
    Rectangle modalRect = { modalX, modalY, modalW, modalH };

    DrawRectangle(0, 0, (int)sw, (int)sh, Fade(BLACK, 0.8f));
    DrawRectangleRec(modalRect, GetColor(0x0B080CFF));
    DrawRectangleLinesEx(modalRect, 4, GetColor(0xE5C158FF));

    std::string title = "HAND LIMIT EXCEEDED - DISCARD " + std::to_string(overBy) + " CARD(S)";
    DrawTextCentered(GetTitleFont(), title.c_str(), modalX + modalW / 2.0f, modalY + 14, 22, 1.0f, GetColor(0xE53935FF));
    DrawTextCentered(GetRegularFont(), "Click a card to discard it", modalX + modalW / 2.0f, modalY + 46, 14, 0.5f, GetColor(0xC2B6B9FF));

    float cardW = 115.0f, cardH = 165.0f, gap = 16.0f;
    int count = (int)hand.size();
    float totalWidth = count * cardW + (count > 0 ? (count - 1) * gap : 0.0f);
    float startX = modalX + (modalW - totalWidth) / 2.0f;
    if (startX < modalX + 10.0f) startX = modalX + 10.0f;
    float startY = modalY + 80.0f;

    for (int i = 0; i < count; i++) {
        Rectangle cardRect = { startX + i * (cardW + gap), startY, cardW, cardH };
        bool hover = CheckCollisionPointRec(mousePos, cardRect);

        std::string cardName = hand[i].get_name();
        std::string filename = GetCardFilename(cardName, faction);
        Texture2D tex = {0};
        auto it = cardTextures.find(filename);
        if (it != cardTextures.end()) {
            tex = it->second;
        } else {
            tex = LoadTextureWithFallbacksForMain("card", {filename});
            if (tex.id != 0) {
                cardTextures[filename] = tex;
                loadedCardTextures.push_back(tex);
            }
        }

        if (tex.id != 0) {
            DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height}, cardRect, {0, 0}, 0.0f, WHITE);
        } else {
            DrawRectangleRec(cardRect, GetColor(0xE2D6BCFF));
            Vector2 nameSize = MeasureTextEx(GetSemiFont(), cardName.c_str(), 19, 0.5f);
            if (nameSize.x > cardRect.width - 10.0f) {
                DrawTextEx(GetSemiFont(), cardName.c_str(), { cardRect.x + 5, cardRect.y + 12 }, 15, 0.4f, BLACK);
            } else {
                DrawTextEx(GetSemiFont(), cardName.c_str(), { cardRect.x + 5, cardRect.y + 12 }, 19, 0.5f, BLACK);
            }
        }

        DrawRectangleLinesEx(cardRect, hover ? 3 : 2, hover ? GetColor(0xE53935FF) : GetColor(0x5A1A24FF));

        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            hand.erase(hand.begin() + i);
            return;
        }
    }
}

static std::string GetCardFilename(const std::string& cardName, const std::string& faction) {
    std::string normalized = cardName;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
    normalized.erase(std::remove(normalized.begin(), normalized.end(), ' '), normalized.end());
    normalized.erase(std::remove(normalized.begin(), normalized.end(), '-'), normalized.end());

    static std::unordered_map<std::string, std::string> nameToFile = {
        {"ambush", "ambush.png"},
        {"baptismofblood", "baptism-of-blood.png"},
        {"beastform", "beastform.png"},
        {"confirmsuspicion", "confirm-suspicion.png"},
        {"counterpunch", "counterpunch.png"},
        {"dash", "dash.png"},
        {"deducestrategy", "deduce-strategy.png"},
        {"domybidding", "do-my-bidding.png"},
        {"educationneverends", "education-never-ends.png"},
        {"elementary", "elementary.png"},
        {"eliminatetheimpossible", "eliminate-the-impossible.png"},
        {"exploit", "exploit.png"},
        {"feedingfrenzy", "feeding-frenzy.png"},
        {"fixedpoint", "fixed-point-in-a-changing-age.png"},
        {"fixedpointinachangingage", "fixed-point-in-a-changing-age.png"},
        {"lookintomyeyes", "look-into-my-eyes.png"},
        {"masterofdisguise", "master-of-disguise.png"},
        {"mistform", "mistform.png"},
        {"preyupon", "prey-upon.png"},
        {"raveningseduction", "ravening-seduction.png"},
        {"servicerevolver", "service-revolver.png"},
        {"studymethods", "study-methods.png"},
        {"thegameisafoot", "the-game-is-afoot.png"},
        {"thirstforsustenance", "thirst-for-sustenance.png"},
        {"administeraid", "administer-aid.png"},

        {"codednotes", "coded-notes.png"},
        {"confound", "confound.png"},
        {"covertpreparation", "covert-preparation.png"},
        {"dreamingofrevenge", "dreaming-of-revenge.png"},
        {"emergefrommist", "emerge-from-mist.png"},
        {"impossibletosee", "impossible-to-see.png"},
        {"intothinair", "into-thin-air.png"},
        {"lurking", "lurking.png"},
        {"reignofterror", "reign-of-terror.png"},
        {"rollingfog", "rolling-fog.png"},
        {"slipaway", "slip-away.png"},
        {"steplightly", "step-lightly.png"},
        {"vanish", "vanish.png"}
    };

    if (normalized == "feint") {
        if (faction == "DRACULA")
            return "feint (1).png";
        else if (faction == "SHERLOCK HOLMES")
            return "feint (2).png";
        else
            return "feint.png";
    }

    auto it = nameToFile.find(normalized);
    if (it != nameToFile.end())
        return it->second;

    return normalized + ".png";
}

static void DrawDefenseModal(GameManager& gm, Board& board, ActionBarState& actionBar, hero* draculaHero,
                              std::unordered_map<std::string, Texture2D>& cardTextures,
                              std::vector<Texture2D>& loadedCardTextures,
                              Vector2 mousePos, float sw, float sh) {
    hero* defender = actionBar.pendingDefender;
    if (!defender) return;

    // See the comment on ActionBarState::defenseJustOpened -- without this,
    // the same click that opened this modal (by clicking an enemy on the
    // map) could double as a click on a card/button here in the same
    // frame, resolving the defense before the player ever saw a choice.
    bool suppressClicks = actionBar.defenseJustOpened;
    actionBar.defenseJustOpened = false;

    std::string faction = HeroFactionLabel(defender);
    auto& hand = defender->gethand();

    // Only defense/multipurpose cards are legal as a defense play -- an
    // attack-only card was previously clickable here too, which let a
    // pure "attack" card get used to defend (its .getdefense() value,
    // usually 0, silently went into the damage math instead of being
    // rejected outright). Also filter by card ownership: whoever is
    // physically defending (a Sister/Watson if pendingDefenderTarget is
    // set, otherwise the hero itself) can only use cards owned by them or
    // marked "any" -- not their teammate's cards, even though the hand is
    // shared.
    character* defenderActor = actionBar.pendingDefenderTarget
        ? actionBar.pendingDefenderTarget
        : static_cast<character*>(defender);

    std::vector<int> defendable;
    for (int i = 0; i < (int)hand.size(); i++) {
        if ((hand[i].gettype() == cardtype::defense || hand[i].gettype() == cardtype::multipurpose) &&
            CardOwnerMatchesCharacter(hand[i], defenderActor)) {
            defendable.push_back(i);
        }
    }

    float modalW = sw * 0.65f;
    float modalH = sh * 0.55f;
    float modalX = (sw - modalW) / 2.0f;
    float modalY = (sh - modalH) / 2.0f;
    Rectangle modalRect = { modalX, modalY, modalW, modalH };

    DrawRectangle(0, 0, (int)sw, (int)sh, Fade(BLACK, 0.75f));
    DrawRectangleRec(modalRect, GetColor(0x0B080CFF));
    DrawRectangleLinesEx(modalRect, 4, GetColor(0xE53935FF));

    std::string title = faction + " - CHOOSE YOUR DEFENSE";
    DrawTextCentered(GetTitleFont(), title.c_str(), modalX + modalW / 2.0f, modalY + 14, 22, 1.0f, GetColor(0xE5C158FF));

    std::string atkLine = "Incoming: " + actionBar.pendingAttackCard.get_name();
    if (actionBar.pendingDefenderTarget && actionBar.pendingDefenderTarget != static_cast<character*>(defender)) {
        atkLine += "  (defending " + actionBar.pendingDefenderTarget->getname() + ")";
    }
    DrawTextCentered(GetRegularFont(), atkLine.c_str(), modalX + modalW / 2.0f, modalY + 46, 15, 0.5f, GetColor(0xC2B6B9FF));

    Rectangle noDefenseBtn = { modalX + modalW - 170.0f, modalY + 12.0f, 150.0f, 30.0f };
    DrawRectangleRec(noDefenseBtn, GetColor(0x1B0A0DFF));
    DrawRectangleLinesEx(noDefenseBtn, 2, GetColor(0x9E2230FF));
    DrawTextCentered(GetSemiFont(), "NO DEFENSE", noDefenseBtn.x + noDefenseBtn.width / 2.0f, noDefenseBtn.y + 7, 13, 0.7f, GetColor(0xE5C158FF));

    if (!suppressClicks && CheckCollisionPointRec(mousePos, noDefenseBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        ActionBar_ResolveDefense(actionBar, gm, board, draculaHero, nullptr);
        return;
    }

    int cardCount = (int)defendable.size();
    if (cardCount > 7) cardCount = 7;

    float cardW = 108.0f;
    float cardH = 172.0f;
    float spacing = 18.0f;
    float totalWidth = (cardCount * cardW) + ((cardCount > 0 ? cardCount - 1 : 0) * spacing);
    float startX = modalX + (modalW - totalWidth) / 2.0f;
    float startY = modalY + 80.0f;

    for (int i = 0; i < cardCount; i++) {
        int handIdx = defendable[i];
        Rectangle cardRect = { startX + i * (cardW + spacing), startY, cardW, cardH };

        std::string cardName = hand[handIdx].get_name();
        std::string filename = GetCardFilename(cardName, faction);
        Texture2D tex = {0};
        auto it = cardTextures.find(filename);
        if (it != cardTextures.end()) {
            tex = it->second;
        } else {
            tex = LoadTextureWithFallbacksForMain("card", {filename});
            if (tex.id != 0) {
                cardTextures[filename] = tex;
                loadedCardTextures.push_back(tex);
            }
        }

        if (!suppressClicks && CheckCollisionPointRec(mousePos, cardRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            card chosen = hand[handIdx];
            std::string chosenName = chosen.get_name();

            if (chosenName == "Dash") {
                actionBar.combatChosenDefenseCard = chosen;
                actionBar.combatTargetHero = actionBar.pendingDefender;
                actionBar.combatTargetIsDefender = true;
                actionBar.awaitingDefense = false;
                actionBar.targetPrompt = TargetPrompt::DashNode;
                return;
            }
            if (chosenName == "Elementary") {
                actionBar.combatChosenDefenseCard = chosen;
                actionBar.combatTargetHero = actionBar.pendingDefender;
                actionBar.combatTargetIsDefender = true;
                actionBar.awaitingDefense = false;
                actionBar.targetPrompt = TargetPrompt::ElementaryPredict;
                return;
            }

            ActionBar_ResolveDefense(actionBar, gm, board, draculaHero, &chosen);
            return;
        }

        if (CheckCollisionPointRec(mousePos, cardRect)) {
            DrawRectangleLinesEx(cardRect, 2, GREEN);
        }

        if (tex.id != 0) {
            DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height}, cardRect, {0, 0}, 0.0f, WHITE);
        } else {
            DrawRectangleRec(cardRect, GetColor(0xE2D6BCFF));
            DrawRectangleLinesEx(cardRect, 2, GetColor(0x5A1A24FF));
            Vector2 nameSize = MeasureTextEx(GetSemiFont(), cardName.c_str(), 20, 0.5f);
            if (nameSize.x > cardRect.width - 10.0f) {
                DrawTextEx(GetSemiFont(), cardName.c_str(), { cardRect.x + 5, cardRect.y + 12 }, 16, 0.4f, BLACK);
            } else {
                DrawTextEx(GetSemiFont(), cardName.c_str(), { cardRect.x + 5, cardRect.y + 12 }, 20, 0.5f, BLACK);
            }
            std::string defLine = "DEF " + std::to_string(hand[handIdx].getdefense()) +
                                   "  BST " + std::to_string(hand[handIdx].getboost());
            DrawTextEx(GetRegularFont(), defLine.c_str(), { cardRect.x + 5, cardRect.y + cardRect.height - 26 }, 14, 0.3f, GetColor(0x3A1A1AFF));
        }
    }

    if (cardCount == 0) {
        std::string empty = "No valid defense card - click NO DEFENSE";
        DrawTextCentered(GetRegularFont(), empty.c_str(), modalX + modalW / 2.0f, modalY + modalH / 2.0f, 15, 0.5f, GetColor(0xA39BA0FF));
    }
}

void RunGameUI(GameManager& gm, character* dracula, character* sis1Obj, character* sis2Obj, character* sis3Obj,
               character* sherlock, character* watson, int firstPlayer, hero* draculaHero, hero* sherlockHero) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1366, 768, "Unmatched: Gothic Shadows");
    SetTargetFPS(60);

    LoadGameFonts();

    Board& board = gm.getBoard();

    Texture2D boardTex = LoadTextureWithFallbacksForMain("", {"board(1).png", "board.jpg", "board.png", "board"});
    Texture2D dracArt  = LoadTextureWithFallbacksForMain("", {"DracArt.png"});
    Texture2D sis1     = LoadTextureWithFallbacksForMain("", {"sis1.png"});
    Texture2D sis2     = LoadTextureWithFallbacksForMain("", {"sis2.png"});
    Texture2D sis3     = LoadTextureWithFallbacksForMain("", {"sis3.png"});
    Texture2D sherArt  = LoadTextureWithFallbacksForMain("", {"holmsArtTransparent.png", "sherlockTran (1).png"});
    Texture2D watsonArt= LoadTextureWithFallbacksForMain("", {"drwatson.png"});
    Texture2D invArt   = LoadTextureWithFallbacksForMain("", {"InvisibleManArt.png", "invArt.png", "tranInv (1).png"});
    Texture2D fogTex   = LoadTextureWithFallbacksForMain("", {"fog.png"});

    bool team1IsDracula = (dracula && dracula->getname() == "Dracula");
    bool team2IsSherlock = (sherlock && sherlock->getname() == "Sherlock Holmes");
    Texture2D team1PortraitArt = team1IsDracula ? dracArt : invArt;
    Texture2D team2PortraitArt = team2IsSherlock ? sherArt : invArt;

    // Whichever slot didn't go to Dracula/Sherlock Holmes is the Invisible
    // Man; keep a typed pointer to him so his fog tokens can be drawn.
    InvisibleMan* invTeam1 = dynamic_cast<InvisibleMan*>(draculaHero);
    InvisibleMan* invTeam2 = dynamic_cast<InvisibleMan*>(sherlockHero);

    std::unordered_map<std::string, Texture2D> cardTextures;
    std::vector<Texture2D> loadedCardTextures;

    int currentRound = 1;
    int activePlayerTurn = gm.getCurrentTeam();
    int lastTeam = activePlayerTurn;
    bool showHandP1 = false;
    bool showHandP2 = false;

    // Save-game feedback: F5/F6/F7 save to slots 1/2/3 at any point during
    // play; saveMessageTimer counts down while the confirmation text is
    // shown on screen.
    std::string saveMessageText;
    float saveMessageTimer = 0.0f;

    ActionBarState actionBar;
    if (draculaHero) actionBar.team1Label = HeroFactionLabel(draculaHero);
    if (sherlockHero) actionBar.team2Label = HeroFactionLabel(sherlockHero);

    while (!WindowShouldClose()) {
        float sw = (float)GetScreenWidth();
        float sh = (float)GetScreenHeight();

        activePlayerTurn = gm.getCurrentTeam();
        if (activePlayerTurn != lastTeam) {
            if (activePlayerTurn == firstPlayer) {
                currentRound++;
                ActionBar_ResetRoundCards(actionBar);
            }
            ActionBar_ResetOnTurnEnd(actionBar);
            showHandP1 = false;
            showHandP2 = false;
            lastTeam = activePlayerTurn;
        }

        float headerH = sh * 0.075f;
        float contentTopY = headerH + sh * 0.02f;

        Rectangle p1Panel = { sw * 0.015f, contentTopY, sw * 0.20f, sh * 0.74f - headerH };
        float mapH = sh * 0.55f - headerH * 0.5f;
        Rectangle mapDest = { sw * 0.23f, contentTopY, sw * 0.54f, mapH };
        Rectangle p2Panel = { sw * 0.785f, contentTopY, sw * 0.20f, sh * 0.74f - headerH };

        float bottomY = p1Panel.y + p1Panel.height + 12.0f;
        float bottomH = sh - bottomY - 12.0f;

        Rectangle endTurnButton = { p2Panel.x, bottomY, p2Panel.width, 48 };
        Rectangle turnOrderBox = { p2Panel.x, bottomY + 60.0f, p2Panel.width, bottomH - 60.0f };
        Rectangle handBox = { sw * 0.59f, bottomY, sw * 0.18f, bottomH };
        Rectangle cardEffectsBox = { sw * 0.23f, bottomY, sw * 0.34f, bottomH };

        ActionBarLayout actionLayout = ActionBar_ComputeLayout(p1Panel, endTurnButton.height, turnOrderBox.height);

Vector2 mousePos = GetMousePosition();

        // Single Save button, sitting in the header strip -- above and
        // outside mapDest/p1Panel/p2Panel entirely, so it never overlaps
        // or shifts any other on-screen element. Saves rotate quietly
        // through 3 backup slots on disk (so a stale save is never the
        // only copy) but the player only ever sees one button.
        static int saveSlotCursor = 1;
        Rectangle saveBtn = { sw - 12.0f - 110.0f, headerH * 0.5f - 16.0f, 110.0f, 32.0f };
        bool hoverSave = CheckCollisionPointRec(mousePos, saveBtn);

        if (hoverSave && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            std::string slotFile = "save_slot_" + std::to_string(saveSlotCursor) + ".txt";
            saveMessageText = gm.saveGame(slotFile) ? "Game Saved" : "Save Failed";
            saveMessageTimer = 2.0f;
            saveSlotCursor = (saveSlotCursor % 3) + 1;
        }

        hero* activeHero = (activePlayerTurn == 1) ? draculaHero : sherlockHero;
        bool gameOver = gm.isGameOver();
        character* winner = gameOver ? gm.getWinner() : nullptr;
        bool mustDiscard = !gameOver && activeHero && activeHero->handsize() > 7;

        if (!gameOver && !actionBar.awaitingDefense && !mustDiscard) {
            if (gm.getActionsRemaining() <= 0 &&
                ((CheckCollisionPointRec(mousePos, endTurnButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_ENTER))) {
                gm.getTurnManager().endTurn();
            }

            if (CheckCollisionPointRec(mousePos, handBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (activePlayerTurn == 1) showHandP1 = !showHandP1;
                else showHandP2 = !showHandP2;
            }

            ActionBar_SelectActorClick(actionBar, gm, mousePos, mapDest, boardTex, actionLayout.panel, handBox);
        }

        character* actingChar = ActionBar_GetActingCharacter(actionBar, gm);
        hero* actingHero = actingChar ? dynamic_cast<hero*>(actingChar) : nullptr;

        if (!gameOver && !mustDiscard) {
            ActionBar_Update(actionBar, gm, actionLayout, mousePos, mapDest, boardTex, handBox, actingChar, actingHero, activeHero, draculaHero);
            ActionBar_UpdateTargeting(actionBar, gm, activeHero, draculaHero, board, mapDest, boardTex, mousePos, actingChar);
        }

        const float CHAR_MAP_SIZE_MULTIPLIER = 2.4f;

        struct MapChar { character* c; Texture2D tex; Color color; float sizeMul; };
        MapChar mapChars[6] = {
            { dracula,  team1PortraitArt, RED,     CHAR_MAP_SIZE_MULTIPLIER },
            { sis1Obj,  sis1,      MAROON,  CHAR_MAP_SIZE_MULTIPLIER },
            { sis2Obj,  sis2,      MAROON,  CHAR_MAP_SIZE_MULTIPLIER },
            { sis3Obj,  sis3,      MAROON,  CHAR_MAP_SIZE_MULTIPLIER },
            { sherlock, team2PortraitArt, BLUE,    CHAR_MAP_SIZE_MULTIPLIER },
            { watson,   watsonArt, SKYBLUE, CHAR_MAP_SIZE_MULTIPLIER },
        };

        float dt = GetFrameTime();

        if (saveMessageTimer > 0.0f) {
            saveMessageTimer -= dt;
        }

        for (auto& mc : mapChars) {
            if (mc.c && mc.c->isalive()) {
                UpdateCharacterAnim(mc.c, board, mapDest, dt, (float)boardTex.width, (float)boardTex.height);
            }
        }

        BeginDrawing();
        ClearBackground(GetColor(0x050407FF));

        {
            std::string phaseLabel = "HERO PHASE";
            std::string turnLabel = "PLAYER " + std::to_string(activePlayerTurn) + " TURN";
            DrawTextCentered(GetTitleFont(), phaseLabel.c_str(), sw / 2.0f, headerH * 0.20f, 26, 1.2f, GetColor(0xE5C158FF));
            DrawTextCentered(GetRegularFont(), turnLabel.c_str(), sw / 2.0f, headerH * 0.60f, 15, 0.9f, GetColor(0xC2B6B9FF));
        }

        // Single save button lives entirely inside the header strip, well
        // above mapDest/p1Panel/p2Panel -- it never overlaps or resizes
        // any other UI element on this screen.
        DrawRectangleRec(saveBtn, hoverSave ? GetColor(0x2A2130FF) : GetColor(0x151218FF));
        DrawRectangleLinesEx(saveBtn, 2, GetColor(0xE5C158FF));
        DrawTextCentered(GetSemiFont(), "SAVE", saveBtn.x + saveBtn.width / 2.0f, saveBtn.y + 8, 15, 0.6f, GetColor(0xE5C158FF));

        if (saveMessageTimer > 0.0f) {
            float alpha = (saveMessageTimer > 1.0f) ? 1.0f : saveMessageTimer;
            DrawTextCentered(GetSemiFont(), saveMessageText.c_str(), sw / 2.0f, headerH + 8, 16, 0.6f, Fade(GetColor(0x6FCF97FF), alpha));
        }

        if (boardTex.id > 0) {
            DrawTexturePro(boardTex, {0, 0, (float)boardTex.width, (float)boardTex.height}, mapDest, {0, 0}, 0.0f, WHITE);
        } else {
            DrawRectangleRec(mapDest, GetColor(0x0F0D12FF));
        }

        ActionBar_DrawMapHighlights(actionBar, gm, board, mapDest, boardTex, actingChar, activeHero);
        ActionBar_DrawTargetingHighlights(actionBar, gm, board, mapDest, boardTex);
        ActionBar_DrawActorSelection(actionBar, gm, board, mapDest, boardTex);

        if (invTeam1) {
            for (int fogNode : invTeam1->getFogPositions()) {
                DrawFogTokenOnNode(fogNode, board, mapDest, (float)boardTex.width, (float)boardTex.height, fogTex);
            }
        }
        if (invTeam2) {
            for (int fogNode : invTeam2->getFogPositions()) {
                DrawFogTokenOnNode(fogNode, board, mapDest, (float)boardTex.width, (float)boardTex.height, fogTex);
            }
        }

        for (auto& mc : mapChars) {
            if (mc.c && mc.c->isalive()) {
                DrawCharacterOnNode(mc.c, mc.tex, mc.color, mapDest.width, mc.sizeMul, (float)boardTex.width);
            }
        }

        DrawRectangleLinesEx(mapDest, 4, GetColor(0x342936FF));
        DrawRectangleLines((int)mapDest.x - 3, (int)mapDest.y - 3, (int)mapDest.width + 6, (int)mapDest.height + 6, GetColor(0x5A4B53FF));

        ActionBar_DrawCardEffectsBox(actionBar, cardEffectsBox);

        DrawRectangleRec(p1Panel, GetColor(0x0B080CFF));
        DrawRectangleLinesEx(p1Panel, 3, GetColor(0x7A1A24FF));
        DrawRectangleLines((int)p1Panel.x - 3, (int)p1Panel.y - 3, (int)p1Panel.width + 6, (int)p1Panel.height + 6, GetColor(0x361C22FF));

        Rectangle dracRect = { p1Panel.x + 14, p1Panel.y + 18, p1Panel.width - 28, p1Panel.height * 0.40f };
        if (team1PortraitArt.id > 0) {
            DrawTexturePro(team1PortraitArt, {0, 0, (float)team1PortraitArt.width, (float)team1PortraitArt.height}, dracRect, {0, 0}, 0.0f, WHITE);
        } else {
            DrawRectangleRec(dracRect, DARKGRAY);
        }
        DrawRectangleLinesEx(dracRect, 3, GetColor(0x9E2230FF));

        if (dracula) {
            if (!dracula->isalive()) {
                DrawText("[ DEAD ]", (int)dracRect.x + (int)(dracRect.width / 2) - 30, (int)(dracRect.y + dracRect.height + 8), 10, RED);
            } else {
                DrawGothicHealthBar(dracRect.x, dracRect.y + dracRect.height + 8, dracRect.width, dracula->gethealth(), dracula->getMaxHp());
            }
        }

        float sisStartY = dracRect.y + dracRect.height + 40;

        if (!team1IsDracula) {
            DrawTextCentered(GetRegularFont(), "SOLO FIGHTER - NO SIDEKICKS", p1Panel.x + p1Panel.width / 2.0f, sisStartY + 10, 12, 0.4f, GetColor(0xA39BA0FF));
        } else {
            float sisWidth  = (p1Panel.width - 34) / 3.0f;
            float sisHeight = sisWidth * 1.15f;
            Texture2D sisTextures[3] = { sis1, sis2, sis3 };
            character* sisObjs[3] = { sis1Obj, sis2Obj, sis3Obj };

            for (int i = 0; i < 3; i++) {
                Rectangle sisBox = { p1Panel.x + 14 + i * (sisWidth + 3), sisStartY, sisWidth, sisHeight };
                if (sisTextures[i].id > 0) {
                    DrawTexturePro(sisTextures[i], {0, 0, (float)sisTextures[i].width, (float)sisTextures[i].height}, sisBox, {0, 0}, 0.0f, WHITE);
                } else {
                    DrawRectangleRec(sisBox, DARKGRAY);
                }
                DrawRectangleLinesEx(sisBox, 2, GetColor(0x602030FF));

                if (sisObjs[i]) {
                    if (!sisObjs[i]->isalive()) {
                        DrawText("[DEAD]", (int)sisBox.x + 2, (int)(sisBox.y + sisBox.height + 6), 8, RED);
                    } else {
                        DrawGothicHealthBar(sisBox.x, sisBox.y + sisBox.height + 6, sisBox.width, sisObjs[i]->gethealth(), sisObjs[i]->getMaxHp());
                    }
                }
            }
        }

        DrawSpecialAbilityButton(actionBar, gm, draculaHero, actingChar, activePlayerTurn == 1, gameOver, p1Panel, mousePos);

        ActionBar_DrawPanel(actionBar, actionLayout);

        DrawRectangleRec(p2Panel, GetColor(0x0B080CFF));
        DrawRectangleLinesEx(p2Panel, 3, GetColor(0x1C3D66FF));
        DrawRectangleLines((int)p2Panel.x - 3, (int)p2Panel.y - 3, (int)p2Panel.width + 6, (int)p2Panel.height + 6, GetColor(0x162A45FF));

        Rectangle sherRect = { p2Panel.x + 14, p2Panel.y + 18, p2Panel.width - 28, p2Panel.height * 0.40f };
        if (team2PortraitArt.id > 0) {
            DrawTexturePro(team2PortraitArt, {0, 0, (float)team2PortraitArt.width, (float)team2PortraitArt.height}, sherRect, {0, 0}, 0.0f, WHITE);
        }
        DrawRectangleLinesEx(sherRect, 3, GetColor(0x28558FFF));

        if (sherlock) {
            if (!sherlock->isalive()) {
                DrawText("[ DEAD ]", (int)sherRect.x + (int)(sherRect.width / 2) - 30, (int)(sherRect.y + sherRect.height + 8), 10, RED);
            } else {
                DrawGothicHealthBar(sherRect.x, sherRect.y + sherRect.height + 8, sherRect.width, sherlock->gethealth(), sherlock->getMaxHp());
            }
        }

        Rectangle watsonRect = { p2Panel.x + (p2Panel.width - (p2Panel.width * 0.55f)) / 2.0f, sherRect.y + sherRect.height + 40, p2Panel.width * 0.55f, p2Panel.height * 0.23f };
        if (!team2IsSherlock) {
            DrawTextCentered(GetRegularFont(), "SOLO FIGHTER - NO SIDEKICKS", p2Panel.x + p2Panel.width / 2.0f, watsonRect.y + 10, 12, 0.4f, GetColor(0xA39BA0FF));
        } else {
        if (watsonArt.id > 0) {
             DrawTexturePro(watsonArt, {0, 0, (float)watsonArt.width, (float)watsonArt.height}, watsonRect, {0, 0}, 0.0f, WHITE);
        }
        DrawRectangleLinesEx(watsonRect, 3, GetColor(0x28558FFF));

        if (watson) {
            if (!watson->isalive()) {
                DrawText("[DEAD]", (int)watsonRect.x + 2, (int)(watsonRect.y + watsonRect.height + 6), 8, RED);
            } else {
                DrawGothicHealthBar(watsonRect.x, watsonRect.y + watsonRect.height + 6, watsonRect.width, watson->gethealth(), watson->getMaxHp());
            }
        }
        }

        DrawSpecialAbilityButton(actionBar, gm, sherlockHero, actingChar, activePlayerTurn == 2, gameOver, p2Panel, mousePos);

        bool canEndTurn = gm.getActionsRemaining() <= 0;
        DrawRectangleRec(endTurnButton, GetColor(0x1B0A0DFF));
        DrawRectangleLinesEx(endTurnButton, 3, canEndTurn ? GetColor(0x9E2230FF) : Fade(GetColor(0x9E2230FF), 0.35f));
        DrawTextCentered(GetSemiFont(), "END TURN", endTurnButton.x + endTurnButton.width / 2.0f, endTurnButton.y + 15, 16, 0.9f, canEndTurn ? GetColor(0xE5C158FF) : Fade(GetColor(0xE5C158FF), 0.35f));

        DrawRectangleRec(turnOrderBox, GetColor(0x0B080CFF));
        DrawRectangleLinesEx(turnOrderBox, 3, GetColor(0x342936FF));

        DrawTextCentered(GetSemiFont(), "TURN ORDER", turnOrderBox.x + turnOrderBox.width / 2.0f, turnOrderBox.y + 6, 14, 0.5f, GetColor(0xA39BA0FF));

        float centerY = turnOrderBox.y + 46;
        float avatarRadius = 14.0f;
        float centerX1 = turnOrderBox.x + 45;
        float centerX2 = turnOrderBox.x + turnOrderBox.width - 45;

        auto TurnLetter = [](hero* h) -> const char* {
            if (!h) return "?";
            std::string n = h->getname();
            for (auto& ch : n) ch = std::tolower(ch);
            if (n.find("dracula") != std::string::npos) return "D";
            if (n.find("invisible") != std::string::npos) return "I";
            if (n.find("sherlock") != std::string::npos) return "S";
            return "?";
        };

        DrawCircle((int)centerX1, (int)centerY, avatarRadius, GetColor(0x1A0D10FF));
        DrawCircleLines((int)centerX1, (int)centerY, avatarRadius, activePlayerTurn == 1 ? GetColor(0xE5C158FF) : GetColor(0x5A1A24FF));
        DrawTextCentered(GetSemiFont(), TurnLetter(draculaHero), centerX1, centerY - 8, 14, 0.6f, GetColor(0xE5C158FF));

        DrawTextCentered(GetRegularFont(), "-->", (centerX1 + centerX2) / 2.0f, centerY - 7, 14, 0.6f, GetColor(0x5A5055FF));

        DrawCircle((int)centerX2, (int)centerY, avatarRadius, GetColor(0x0D121AFF));
        DrawCircleLines((int)centerX2, (int)centerY, avatarRadius, activePlayerTurn == 2 ? GetColor(0xE5C158FF) : GetColor(0x1C3D66FF));
        DrawTextCentered(GetSemiFont(), TurnLetter(sherlockHero), centerX2, centerY - 8, 14, 0.6f, GetColor(0xE5C158FF));

        std::string roundStr = "ROUND " + std::to_string(currentRound);
        DrawTextCentered(GetRegularFont(), roundStr.c_str(), turnOrderBox.x + turnOrderBox.width / 2.0f, turnOrderBox.y + 58, 13, 0.4f, GetColor(0xA39BA0FF));

        DrawRectangleRec(handBox, GetColor(0x0B080CFF));
        DrawRectangleLinesEx(handBox, 3, GetColor(0x342936FF));

        std::string handTitle = HeroFactionLabel(activeHero) + " - HAND";
        DrawTextCentered(GetSemiFont(), handTitle.c_str(), handBox.x + handBox.width / 2.0f, handBox.y + 8, 14, 0.5f, GetColor(0xE5C158FF));

        bool currentShowHand = (activePlayerTurn == 1) ? showHandP1 : showHandP2;
        if (!currentShowHand) {
            DrawTextCentered(GetRegularFont(), "CARDS IN HAND", handBox.x + handBox.width / 2.0f, handBox.y + 32, 13, 0.4f, GetColor(0xA39BA0FF));
            for (int i = 0; i < 5; i++) {
                Rectangle cardBack = { handBox.x + 12.0f + (i * 30.0f), handBox.y + 55.0f, 26.0f, 42.0f };
                DrawRectangleRec(cardBack, GetColor(0x151218FF));
                DrawRectangleLinesEx(cardBack, 1, GetColor(0xE5C158FF));
            }
            DrawTextCentered(GetSemiFont(), "TAP TO VIEW HAND", handBox.x + handBox.width / 2.0f, handBox.y + 112, 14, 0.5f, GetColor(0xE5C158FF));
        } else {
            DrawTextCentered(GetRegularFont(), "HAND OPENED (MODAL)", handBox.x + handBox.width / 2.0f, handBox.y + 52, 12, 0.4f, GetColor(0xA39BA0FF));
            DrawTextCentered(GetSemiFont(), "TAP TO HIDE HAND", handBox.x + handBox.width / 2.0f, handBox.y + 112, 14, 0.5f, GetColor(0xE04A4AFF));
        }

        if (!actionBar.awaitingDefense && currentShowHand && activeHero) {
            auto hand = activeHero->gethand();
            std::string faction = HeroFactionLabel(activeHero);

            float modalW = sw * 0.65f;
            float modalH = sh * 0.50f;
            float modalX = (sw - modalW) / 2.0f;
            float modalY = (sh - modalH) / 2.0f;
            Rectangle modalRect = { modalX, modalY, modalW, modalH };

            Rectangle closeBtnRect = { modalX + modalW - 35.0f, modalY + 12.0f, 25.0f, 25.0f };

            if (CheckCollisionPointRec(mousePos, closeBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (activePlayerTurn == 1) showHandP1 = false;
                else showHandP2 = false;
            }

            DrawRectangle(0, 0, (int)sw, (int)sh, Fade(BLACK, 0.7f));
            DrawRectangleRec(modalRect, GetColor(0x0B080CFF));
            DrawRectangleLinesEx(modalRect, 4, GetColor(0xE5C158FF));

            DrawRectangleRec(closeBtnRect, GetColor(0x9E2230FF));
            DrawRectangleLinesEx(closeBtnRect, 1, GetColor(0xE5C158FF));
            DrawTextCentered(GetSemiFont(), "X", closeBtnRect.x + closeBtnRect.width / 2.0f, closeBtnRect.y + 5, 14, 0.6f, WHITE);

            std::string modalTitle = HeroFactionLabel(activeHero) + " - EXPANDED HAND";
            DrawTextCentered(GetTitleFont(), modalTitle.c_str(), modalX + modalW / 2.0f, modalY + 16, 24, 1.0f, GetColor(0xE5C158FF));
            DrawTextCentered(GetRegularFont(), "Click on any card to play/interact", modalX + modalW / 2.0f, modalY + 46, 13, 0.4f, GetColor(0xA39BA0FF));

            int cardCount = (int)hand.size();
            if (cardCount > 7) cardCount = 7;

            float cardW = 108.0f;
            float cardH = 172.0f;
            float spacing = 18.0f;
            float totalWidth = (cardCount * cardW) + ((cardCount > 0 ? cardCount - 1 : 0) * spacing);
            float startX = modalX + (modalW - totalWidth) / 2.0f;
            float startY = modalY + 75.0f;

            for (int i = 0; i < cardCount; i++) {
                Rectangle cardRect = { startX + i * (cardW + spacing), startY, cardW, cardH };

                std::string cardName = hand[i].get_name();
                std::string filename = GetCardFilename(cardName, faction);
                Texture2D tex = {0};
                auto it = cardTextures.find(filename);
                if (it != cardTextures.end()) {
                    tex = it->second;
                } else {
                    tex = LoadTextureWithFallbacksForMain("card", {filename});
                    if (tex.id != 0) {
                        cardTextures[filename] = tex;
                        loadedCardTextures.push_back(tex);
                    }
                }

                if (CheckCollisionPointRec(mousePos, cardRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    bool* showHandFlag = (activePlayerTurn == 1) ? &showHandP1 : &showHandP2;
                    ActionBar_HandleCardClick(actionBar, gm, activeHero, hand[i], i, *showHandFlag, board, draculaHero, actingChar);
                }

                if (CheckCollisionPointRec(mousePos, cardRect)) {
                    DrawRectangleLinesEx(cardRect, 2, GREEN);
                }
                if (ActionBar_IsCardSelectedForAttack(actionBar, i)) {
                    DrawRectangleLinesEx(cardRect, 3, GetColor(0xE53935FF));
                }

                if (tex.id != 0) {
                    DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height}, cardRect, {0, 0}, 0.0f, WHITE);
                } else {
                    DrawRectangleRec(cardRect, GetColor(0xE2D6BCFF));
                    DrawRectangleLinesEx(cardRect, 2, GetColor(0x5A1A24FF));
                    // Bigger, wrapped fallback text (used whenever the real
                    // card art can't be found) so the card name is actually
                    // legible instead of a tiny 14px sliver.
                    Vector2 nameSize = MeasureTextEx(GetSemiFont(), cardName.c_str(), 20, 0.5f);
                    if (nameSize.x > cardRect.width - 10.0f) {
                        DrawTextEx(GetSemiFont(), cardName.c_str(), { cardRect.x + 5, cardRect.y + 12 }, 16, 0.4f, BLACK);
                    } else {
                        DrawTextEx(GetSemiFont(), cardName.c_str(), { cardRect.x + 5, cardRect.y + 12 }, 20, 0.5f, BLACK);
                    }

                    std::string atkDefLine = "ATK " + std::to_string(hand[i].getattack()) +
                                              "  DEF " + std::to_string(hand[i].getdefense()) +
                                              "  BST " + std::to_string(hand[i].getboost());
                    DrawTextEx(GetRegularFont(), atkDefLine.c_str(), { cardRect.x + 5, cardRect.y + cardRect.height - 26 }, 14, 0.3f, GetColor(0x3A1A1AFF));
                }
            }
        }

        if (mustDiscard) {
            DrawDiscardModal(activeHero, draculaHero, cardTextures, loadedCardTextures, mousePos, sw, sh);
        } else if (actionBar.awaitingDefense) {
            DrawDefenseModal(gm, board, actionBar, draculaHero, cardTextures, loadedCardTextures, mousePos, sw, sh);
        }

        if (actionBar.targetPrompt == TargetPrompt::ConfirmSuspicionValue) {
            ActionBar_DrawValuePicker(actionBar, gm, activeHero, draculaHero, mousePos, sw, sh);
        }
        if (actionBar.targetPrompt == TargetPrompt::EliminateCard) {
            ActionBar_DrawEliminatePicker(actionBar, gm, activeHero, draculaHero, mousePos, sw, sh);
        }
        if (actionBar.targetPrompt == TargetPrompt::BeastformDiscard) {
            ActionBar_DrawBeastformPicker(actionBar, mousePos, sw, sh);
        }
        if (actionBar.targetPrompt == TargetPrompt::ElementaryPredict) {
            ActionBar_DrawElementaryPicker(actionBar, gm, board, draculaHero, mousePos, sw, sh);
        }

        DrawSherlockAbilityInfo(actionBar, mousePos, sw, sh);

        if (gameOver) {
            DrawRectangle(0, 0, (int)sw, (int)sh, Fade(BLACK, 0.75f));
            std::string winnerName = winner ? winner->getname() : "Nobody";
            std::string subLine = winnerName + " WINS!";
            DrawTextCentered(GetTitleFont(), "GAME OVER", sw / 2.0f, sh / 2.0f - 40, 40, 1.5f, GetColor(0xE5C158FF));
            DrawTextCentered(GetRegularFont(), subLine.c_str(), sw / 2.0f, sh / 2.0f + 10, 22, 1.0f, WHITE);
            DrawTextCentered(GetRegularFont(), "Press ENTER to return to the menu", sw / 2.0f, sh / 2.0f + 50, 14, 0.6f, GetColor(0xC2B6B9FF));
        }

        EndDrawing();

        if (gameOver && IsKeyPressed(KEY_ENTER)) {
            break;
        }
    }

    UnloadTexture(boardTex);
    UnloadTexture(dracArt);
    UnloadTexture(sis1);
    UnloadTexture(sis2);
    UnloadTexture(sis3);
    UnloadTexture(sherArt);
    UnloadTexture(watsonArt);
    UnloadTexture(invArt);
    UnloadTexture(fogTex);
    for (auto& tex : loadedCardTextures) {
        UnloadTexture(tex);
    }
    UnloadGameFonts();
    CloseWindow();
}