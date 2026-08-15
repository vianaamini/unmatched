#include "raylib.h"
#include "../include/map.hpp"
#include "../include/character.hpp"
#include "../include/hero.hpp"
#include "../include/game_manager.hpp"
#include "actionbar.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <cmath>

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

Texture2D LoadTextureWithFallbacksForMain(const std::string& category, const std::vector<std::string>& filenames) {
    std::vector<std::string> basePaths = {
        "assets/", "assets/heroes/", "assets/images/",
        "../assets/", "../assets/heroes/", "../assets/images/",
        "build/assets/", "build/assets/heroes/"
    };

    for (const auto& filename : filenames) {
        for (const auto& basePath : basePaths) {
            std::string fullPath = basePath + (category.empty() ? "" : category + "/") + filename;
            if (FileExists(fullPath.c_str())) {
                Texture2D tex = LoadTexture(fullPath.c_str());
                if (tex.id > 0) {
                    SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);
                    return tex;
                }
            }
        }
    }
    return Texture2D{ 0 };
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
        {"administeraid", "administer-aid.png"}
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

void RunGameUI(GameManager& gm, character* dracula, character* sis1Obj, character* sis2Obj, character* sis3Obj,
               character* sherlock, character* watson, int firstPlayer, hero* draculaHero, hero* sherlockHero) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1366, 768, "Unmatched: Gothic Shadows");
    SetTargetFPS(60);

    Board& board = gm.getBoard(); 

    Texture2D boardTex = LoadTextureWithFallbacksForMain("", {"board(1).png", "board.jpg", "board.png", "board"});
    Texture2D dracArt  = LoadTextureWithFallbacksForMain("", {"DracArt.png"});
    Texture2D sis1     = LoadTextureWithFallbacksForMain("", {"sis1.png"});
    Texture2D sis2     = LoadTextureWithFallbacksForMain("", {"sis2.png"});
    Texture2D sis3     = LoadTextureWithFallbacksForMain("", {"sis3.png"});
    Texture2D sherArt  = LoadTextureWithFallbacksForMain("", {"holmsArtTransparent.png", "sherlockTran (1).png"});
    Texture2D watsonArt= LoadTextureWithFallbacksForMain("", {"drwatson.png"});

    std::unordered_map<std::string, Texture2D> cardTextures;
    std::vector<Texture2D> loadedCardTextures;

    int currentRound = 1;
    int activePlayerTurn = firstPlayer;
    bool showHandP1 = false;
    bool showHandP2 = false;

    ActionBarState actionBar;

    while (!WindowShouldClose()) {
        float sw = (float)GetScreenWidth();
        float sh = (float)GetScreenHeight();

        Rectangle p1Panel = { sw * 0.015f, sh * 0.025f, sw * 0.20f, sh * 0.74f };
        Rectangle mapDest = { sw * 0.23f, sh * 0.025f, sw * 0.54f, sh * 0.74f };
        Rectangle p2Panel = { sw * 0.785f, sh * 0.025f, sw * 0.20f, sh * 0.74f };

        Rectangle endTurnButton = { p2Panel.x, p2Panel.y + p2Panel.height + 12, p2Panel.width, 48 };
        Rectangle turnOrderBox = { p2Panel.x, endTurnButton.y + endTurnButton.height + 12, p2Panel.width, 78 };
        Rectangle handBox = { p2Panel.x - p2Panel.width - 15, endTurnButton.y, p2Panel.width, endTurnButton.height + 12 + turnOrderBox.height };

        ActionBarLayout actionLayout = ActionBar_ComputeLayout(p1Panel, endTurnButton.height, turnOrderBox.height);

        Vector2 mousePos = GetMousePosition();

        if ((CheckCollisionPointRec(mousePos, endTurnButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_ENTER)) {
            gm.nextTurn();
            if (activePlayerTurn == 1) {
                activePlayerTurn = 2;
            } else {
                activePlayerTurn = 1;
                currentRound++;
            }
            showHandP1 = false;
            showHandP2 = false;
            ActionBar_ResetOnTurnEnd(actionBar);
        }

        if (CheckCollisionPointRec(mousePos, handBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (activePlayerTurn == 1) showHandP1 = !showHandP1;
            else showHandP2 = !showHandP2;
        }

        ActionBar_SelectActorClick(actionBar, gm, mousePos, mapDest, boardTex, actionLayout.panel, handBox);

        character* actingChar = ActionBar_GetActingCharacter(actionBar, gm);
        hero* actingHero = actingChar ? dynamic_cast<hero*>(actingChar) : nullptr;
        hero* activeHero = (activePlayerTurn == 1) ? draculaHero : sherlockHero;

        ActionBar_Update(actionBar, gm, actionLayout, mousePos, mapDest, boardTex, handBox, actingChar, actingHero, activeHero);

        const float CHAR_MAP_SIZE_MULTIPLIER = 2.4f;

        struct MapChar { character* c; Texture2D tex; Color color; float sizeMul; };
        MapChar mapChars[6] = {
            { dracula,  dracArt,   RED,     CHAR_MAP_SIZE_MULTIPLIER },
            { sis1Obj,  sis1,      MAROON,  CHAR_MAP_SIZE_MULTIPLIER },
            { sis2Obj,  sis2,      MAROON,  CHAR_MAP_SIZE_MULTIPLIER },
            { sis3Obj,  sis3,      MAROON,  CHAR_MAP_SIZE_MULTIPLIER },
            { sherlock, sherArt,   BLUE,    CHAR_MAP_SIZE_MULTIPLIER },
            { watson,   watsonArt, SKYBLUE, CHAR_MAP_SIZE_MULTIPLIER },
        };

        float dt = GetFrameTime(); 

        for (auto& mc : mapChars) {
            if (mc.c && mc.c->isalive()) {
                UpdateCharacterAnim(mc.c, board, mapDest, dt, (float)boardTex.width, (float)boardTex.height);
            }
        }

        BeginDrawing();
        ClearBackground(GetColor(0x050407FF));

        if (boardTex.id > 0) {
            DrawTexturePro(boardTex, {0, 0, (float)boardTex.width, (float)boardTex.height}, mapDest, {0, 0}, 0.0f, WHITE);
        } else {
            DrawRectangleRec(mapDest, GetColor(0x0F0D12FF));
        }

        ActionBar_DrawMapHighlights(actionBar, gm, board, mapDest, boardTex, actingChar, actingHero);
        ActionBar_DrawActorSelection(actionBar, gm, board, mapDest, boardTex);

        for (auto& mc : mapChars) {
            if (mc.c && mc.c->isalive()) {
                DrawCharacterOnNode(mc.c, mc.tex, mc.color, mapDest.width, mc.sizeMul, (float)boardTex.width);
            }
        }

        DrawRectangleLinesEx(mapDest, 4, GetColor(0x342936FF));
        DrawRectangleLines((int)mapDest.x - 3, (int)mapDest.y - 3, (int)mapDest.width + 6, (int)mapDest.height + 6, GetColor(0x5A4B53FF));

        DrawRectangleRec(p1Panel, GetColor(0x0B080CFF));
        DrawRectangleLinesEx(p1Panel, 3, GetColor(0x7A1A24FF));
        DrawRectangleLines((int)p1Panel.x - 3, (int)p1Panel.y - 3, (int)p1Panel.width + 6, (int)p1Panel.height + 6, GetColor(0x361C22FF));

        Rectangle dracRect = { p1Panel.x + 14, p1Panel.y + 18, p1Panel.width - 28, p1Panel.height * 0.40f };
        if (dracArt.id > 0) {
            DrawTexturePro(dracArt, {0, 0, (float)dracArt.width, (float)dracArt.height}, dracRect, {0, 0}, 0.0f, WHITE);
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

        ActionBar_DrawPanel(actionBar, actionLayout);

        DrawRectangleRec(p2Panel, GetColor(0x0B080CFF));
        DrawRectangleLinesEx(p2Panel, 3, GetColor(0x1C3D66FF));
        DrawRectangleLines((int)p2Panel.x - 3, (int)p2Panel.y - 3, (int)p2Panel.width + 6, (int)p2Panel.height + 6, GetColor(0x162A45FF));

        Rectangle sherRect = { p2Panel.x + 14, p2Panel.y + 18, p2Panel.width - 28, p2Panel.height * 0.40f };
        if (sherArt.id > 0) {
            DrawTexturePro(sherArt, {0, 0, (float)sherArt.width, (float)sherArt.height}, sherRect, {0, 0}, 0.0f, WHITE);
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

        DrawRectangleRec(endTurnButton, GetColor(0x1B0A0DFF));
        DrawRectangleLinesEx(endTurnButton, 3, GetColor(0x9E2230FF));
        DrawText("END TURN", (int)(endTurnButton.x + (endTurnButton.width - MeasureText("END TURN", 14)) / 2), (int)(endTurnButton.y + 17), 14, GetColor(0xE5C158FF));

        DrawRectangleRec(turnOrderBox, GetColor(0x0B080CFF));
        DrawRectangleLinesEx(turnOrderBox, 3, GetColor(0x342936FF));

        DrawText("TURN ORDER", (int)(turnOrderBox.x + (turnOrderBox.width - MeasureText("TURN ORDER", 10)) / 2), (int)(turnOrderBox.y + 8), 10, GetColor(0x8A8085FF));

        float centerY = turnOrderBox.y + 46;
        float avatarRadius = 14.0f;
        float centerX1 = turnOrderBox.x + 45;
        float centerX2 = turnOrderBox.x + turnOrderBox.width - 45;

        DrawCircle((int)centerX1, (int)centerY, avatarRadius, GetColor(0x1A0D10FF));
        DrawCircleLines((int)centerX1, (int)centerY, avatarRadius, activePlayerTurn == 1 ? GetColor(0xE5C158FF) : GetColor(0x5A1A24FF));
        DrawText("D", (int)centerX1 - 4, (int)centerY - 6, 12, GetColor(0xE5C158FF));

        DrawText("-->", (int)((centerX1 + centerX2) / 2) - 12, (int)centerY - 6, 12, GetColor(0x5A5055FF));

        DrawCircle((int)centerX2, (int)centerY, avatarRadius, GetColor(0x0D121AFF));
        DrawCircleLines((int)centerX2, (int)centerY, avatarRadius, activePlayerTurn == 2 ? GetColor(0xE5C158FF) : GetColor(0x1C3D66FF));
        DrawText("S", (int)centerX2 - 4, (int)centerY - 6, 12, GetColor(0xE5C158FF));

        std::string roundStr = "ROUND " + std::to_string(currentRound);
        DrawText(roundStr.c_str(), (int)(turnOrderBox.x + (turnOrderBox.width - MeasureText(roundStr.c_str(), 9)) / 2), (int)(turnOrderBox.y + 62), 9, GetColor(0x8A8085FF));

        DrawRectangleRec(handBox, GetColor(0x0B080CFF));
        DrawRectangleLinesEx(handBox, 3, GetColor(0x342936FF));

        std::string handTitle = (activePlayerTurn == 1) ? "DRACULA - HAND" : "SHERLOCK - HAND";
        DrawText(handTitle.c_str(), (int)(handBox.x + (handBox.width - MeasureText(handTitle.c_str(), 10)) / 2), (int)(handBox.y + 10), 10, GetColor(0xE5C158FF));

        bool currentShowHand = (activePlayerTurn == 1) ? showHandP1 : showHandP2;
        if (!currentShowHand) {
            DrawText("CARDS IN HAND", (int)(handBox.x + (handBox.width - MeasureText("CARDS IN HAND", 9)) / 2), (int)(handBox.y + 35), 9, GetColor(0x8A8085FF));
            for (int i = 0; i < 5; i++) {
                Rectangle cardBack = { handBox.x + 12.0f + (i * 30.0f), handBox.y + 55.0f, 26.0f, 42.0f };
                DrawRectangleRec(cardBack, GetColor(0x151218FF));
                DrawRectangleLinesEx(cardBack, 1, GetColor(0xE5C158FF));
            }
            DrawText("TAP TO VIEW HAND", (int)(handBox.x + (handBox.width - MeasureText("TAP TO VIEW HAND", 10)) / 2), (int)(handBox.y + 115), 10, GetColor(0xE5C158FF));
        } else {
            DrawText("HAND OPENED (MODAL)", (int)(handBox.x + (handBox.width - MeasureText("HAND OPENED (MODAL)", 8)) / 2), (int)(handBox.y + 55), 8, GetColor(0x8A8085FF));
            DrawText("TAP TO HIDE HAND", (int)(handBox.x + (handBox.width - MeasureText("TAP TO HIDE HAND", 10)) / 2), (int)(handBox.y + 115), 10, GetColor(0x9E2230FF));
        }

        if (currentShowHand && activeHero) {
            auto hand = activeHero->gethand();
            std::string faction = (activeHero == draculaHero) ? "DRACULA" : "SHERLOCK HOLMES";

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
            DrawText("X", (int)closeBtnRect.x + 8, (int)closeBtnRect.y + 5, 14, WHITE);

            std::string modalTitle = (activePlayerTurn == 1) ? "DRACULA - EXPANDED HAND" : "SHERLOCK - EXPANDED HAND";
            DrawText(modalTitle.c_str(), (int)(modalX + (modalW - MeasureText(modalTitle.c_str(), 18)) / 2), (int)(modalY + 20), 18, GetColor(0xE5C158FF));
            DrawText("Click on any card to play/interact", (int)(modalX + (modalW - MeasureText("Click on any card to play/interact", 10)) / 2), (int)(modalY + 45), 10, GetColor(0x8A8085FF));

            int cardCount = (int)hand.size();
            if (cardCount > 5) cardCount = 5;

            float cardW = 90.0f;
            float cardH = 145.0f;
            float spacing = 20.0f;
            float totalWidth = (cardCount * cardW) + ((cardCount - 1) * spacing);
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
                    std::cout << "Clicked on card: " << cardName << std::endl;
                    bool* showHandFlag = (activePlayerTurn == 1) ? &showHandP1 : &showHandP2;
                    ActionBar_HandleCardClick(actionBar, gm, activeHero, hand[i], i, *showHandFlag);
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
                    DrawText(cardName.c_str(), (int)cardRect.x + 5, (int)cardRect.y + 10, 10, BLACK);
                }
            }
        }

        EndDrawing();
    }

    UnloadTexture(boardTex);
    UnloadTexture(dracArt);
    UnloadTexture(sis1);
    UnloadTexture(sis2);
    UnloadTexture(sis3);
    UnloadTexture(sherArt);
    UnloadTexture(watsonArt);
    for (auto& tex : loadedCardTextures) {
        UnloadTexture(tex);
    }
    CloseWindow();
}