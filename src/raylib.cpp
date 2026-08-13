#include "raylib.h"
#include "../include/map.hpp"
#include "../include/character.hpp"
#include "../include/hero.hpp"
#include <vector>
#include <string>
#include <iostream>

void DrawGothicHealthBar(float x, float y, float width, int currentHp, int maxHp) {
    float percent = (maxHp > 0) ? (float)currentHp / (float)maxHp : 0.0f;
    if (percent < 0.0f) percent = 0.0f;
    if (percent > 1.0f) percent = 1.0f;

    DrawRectangleLines((int)x - 1, (int)y - 1, (int)width + 2, 11, GetColor(0x3A2B32FF));
    DrawRectangle((int)x, (int)y, (int)width, 9, GetColor(0x0E0A0DFF));
    DrawRectangle((int)x, (int)y, (int)(width * percent), 9, GetColor(0x7A0010FF));
    DrawText(TextFormat("HP: %d/%d", currentHp, maxHp), (int)x, (int)y + 12, 10, GetColor(0xC2B6B9FF));
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

void RunGameUI(Board& board, character* dracula, character* sis1Obj, character* sis2Obj, character* sis3Obj, character* sherlock, character* watson, int firstPlayer, hero* draculaHero, hero* sherlockHero) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1366, 768, "Unmatched: Gothic Shadows");
    SetTargetFPS(60);

    Texture2D boardTex = LoadTextureWithFallbacksForMain("", {"board(1).png", "board.jpg", "board.png", "board"});
    Texture2D dracArt  = LoadTextureWithFallbacksForMain("", {"DracArt.png"});
    Texture2D sis1     = LoadTextureWithFallbacksForMain("", {"sis1.png"});
    Texture2D sis2     = LoadTextureWithFallbacksForMain("", {"sis2.png"});
    Texture2D sis3     = LoadTextureWithFallbacksForMain("", {"sis3.png"});
    Texture2D sherArt  = LoadTextureWithFallbacksForMain("", {"holmsArtTransparent.png", "sherlockTran (1).png"});
    Texture2D watsonArt= LoadTextureWithFallbacksForMain("", {"drwatson.png"});

    int currentRound = 1;
    int activePlayerTurn = firstPlayer; 
    bool showHandP1 = false;
    bool showHandP2 = false;

    while (!WindowShouldClose()) {
        float sw = (float)GetScreenWidth();
        float sh = (float)GetScreenHeight();

        Rectangle p1Panel = { sw * 0.015f, sh * 0.025f, sw * 0.20f, sh * 0.74f };
        Rectangle mapDest = { sw * 0.23f, sh * 0.025f, sw * 0.54f, sh * 0.74f };
        Rectangle p2Panel = { sw * 0.785f, sh * 0.025f, sw * 0.20f, sh * 0.74f };

        Rectangle endTurnButton = { p2Panel.x, p2Panel.y + p2Panel.height + 12, p2Panel.width, 48 };
        Rectangle turnOrderBox = { p2Panel.x, endTurnButton.y + endTurnButton.height + 12, p2Panel.width, 78 };
        Rectangle handBox = { p2Panel.x - p2Panel.width - 15, endTurnButton.y, p2Panel.width, endTurnButton.height + 12 + turnOrderBox.height };

        Vector2 mousePos = GetMousePosition();
        
        // با زدن دکمه End Turn یا کلید Enter، نوبت جابجا شده و وضعیت نمایش هند برای بازیکن جدید ریست/تنظیم می‌شود
        if ((CheckCollisionPointRec(mousePos, endTurnButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_ENTER)) {
            if (activePlayerTurn == 1) {
                activePlayerTurn = 2; 
            } else {
                activePlayerTurn = 1; 
                currentRound++;
            }
        }

        if (CheckCollisionPointRec(mousePos, handBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (activePlayerTurn == 1) showHandP1 = !showHandP1;
            else showHandP2 = !showHandP2;
        }

        BeginDrawing();
        ClearBackground(GetColor(0x050407FF));

        if (boardTex.id > 0) {
            DrawTexturePro(boardTex, {0, 0, (float)boardTex.width, (float)boardTex.height}, mapDest, {0, 0}, 0.0f, WHITE);
        } else {
            DrawRectangleRec(mapDest, GetColor(0x0F0D12FF));
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

        // بخش رسم باکس هند که به طور کاملاً داینامیک بر اساس نوبت هیروی فعال را می‌خواند
        DrawRectangleRec(handBox, GetColor(0x0B080CFF));
        DrawRectangleLinesEx(handBox, 3, GetColor(0x342936FF));

        hero* activeHero = (activePlayerTurn == 1) ? draculaHero : sherlockHero;
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
            if (activeHero) {
                auto hand = activeHero->gethand();
                for (size_t i = 0; i < hand.size() && i < 5; i++) {
                    Rectangle cardFace = { handBox.x + 8.0f + (i * 30.0f), handBox.y + 30.0f, 27.0f, 75.0f };
                    DrawRectangleRec(cardFace, GetColor(0xE2D6BCFF));
                    DrawRectangleLinesEx(cardFace, 1, GetColor(0x5A1A24FF));

                    std::string cName = hand[i].get_name();
                    if (cName.length() > 4) cName = cName.substr(0, 4);
                    DrawText(cName.c_str(), (int)cardFace.x + 2, (int)cardFace.y + 4, 6, BLACK);

                    std::string typeStr = "";
                    if (hand[i].gettype() == cardtype::attack) typeStr = "ATK";
                    else if (hand[i].gettype() == cardtype::defense) typeStr = "DEF";
                    else typeStr = "SCH";
                    DrawText(typeStr.c_str(), (int)cardFace.x + 2, (int)cardFace.y + 25, 6, DARKBLUE);

                    std::string valStr = "";
                    if (hand[i].gettype() == cardtype::attack) valStr = std::to_string(hand[i].getattack());
                    else if (hand[i].gettype() == cardtype::defense) valStr = std::to_string(hand[i].getdefense());
                    else valStr = std::to_string(hand[i].getboost());
                    DrawText(valStr.c_str(), (int)cardFace.x + 8, (int)cardFace.y + 45, 8, RED);
                }
            }
            DrawText("TAP TO HIDE HAND", (int)(handBox.x + (handBox.width - MeasureText("TAP TO HIDE HAND", 10)) / 2), (int)(handBox.y + 115), 10, GetColor(0x9E2230FF));
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
    CloseWindow();
}