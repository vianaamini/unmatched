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

void RunGameUI(Board& board, character* dracula, character* sis1Obj, character* sis2Obj, character* sis3Obj, character* sherlock, character* watson) {
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

    while (!WindowShouldClose()) {
        float sw = (float)GetScreenWidth();
        float sh = (float)GetScreenHeight();

        Rectangle p1Panel = { sw * 0.02f, sh * 0.03f, sw * 0.19f, sh * 0.72f };
        Rectangle mapDest = { sw * 0.23f, sh * 0.03f, sw * 0.54f, sh * 0.72f };
        Rectangle p2Panel = { sw * 0.79f, sh * 0.03f, sw * 0.19f, sh * 0.72f };

        BeginDrawing();
        ClearBackground(GetColor(0x050407FF));

        if (boardTex.id > 0) {
            DrawTexturePro(boardTex, {0, 0, (float)boardTex.width, (float)boardTex.height}, mapDest, {0, 0}, 0.0f, WHITE);
        } else {
            DrawRectangleRec(mapDest, GetColor(0x0F0D12FF));
        }
        DrawRectangleLinesEx(mapDest, 3, GetColor(0x241D26FF));
        DrawRectangleLines((int)mapDest.x - 2, (int)mapDest.y - 2, (int)mapDest.width + 4, (int)mapDest.height + 4, GetColor(0x4A3B43FF));

        DrawRectangleRec(p1Panel, GetColor(0x0B080CFF));
        DrawRectangleLinesEx(p1Panel, 2, GetColor(0x591218FF));
        DrawRectangleLines((int)p1Panel.x - 2, (int)p1Panel.y - 2, (int)p1Panel.width + 4, (int)p1Panel.height + 4, GetColor(0x261418FF));

        Rectangle dracRect = { p1Panel.x + 12, p1Panel.y + 18, p1Panel.width - 24, p1Panel.height * 0.40f };
        if (dracArt.id > 0) {
            DrawTexturePro(dracArt, {0, 0, (float)dracArt.width, (float)dracArt.height}, dracRect, {0, 0}, 0.0f, WHITE);
        } else {
            DrawRectangleRec(dracRect, DARKGRAY);
        }
        DrawRectangleLinesEx(dracRect, 2, GetColor(0x7A1A24FF));

        if (dracula) {
            if (!dracula->isalive()) {
                DrawText("[ DEAD ]", (int)dracRect.x + (int)(dracRect.width / 2) - 30, (int)(dracRect.y + dracRect.height + 8), 10, RED);
            } else {
                DrawGothicHealthBar(dracRect.x, dracRect.y + dracRect.height + 8, dracRect.width, dracula->gethealth(), dracula->getMaxHp());
            }
        }

        float sisStartY = dracRect.y + dracRect.height + 40;
        float sisWidth  = (p1Panel.width - 30) / 3.0f;
        float sisHeight = sisWidth * 1.15f;
        Texture2D sisTextures[3] = { sis1, sis2, sis3 };
        character* sisObjs[3] = { sis1Obj, sis2Obj, sis3Obj };

        for (int i = 0; i < 3; i++) {
            Rectangle sisBox = { p1Panel.x + 12 + i * (sisWidth + 3), sisStartY, sisWidth, sisHeight };
            if (sisTextures[i].id > 0) {
                DrawTexturePro(sisTextures[i], {0, 0, (float)sisTextures[i].width, (float)sisTextures[i].height}, sisBox, {0, 0}, 0.0f, WHITE);
            } else {
                DrawRectangleRec(sisBox, DARKGRAY);
            }
            DrawRectangleLinesEx(sisBox, 1, GetColor(0x401622FF));

            if (sisObjs[i]) {
                if (!sisObjs[i]->isalive()) {
                    DrawText("[DEAD]", (int)sisBox.x + 2, (int)(sisBox.y + sisBox.height + 6), 8, RED);
                } else {
                    DrawGothicHealthBar(sisBox.x, sisBox.y + sisBox.height + 6, sisBox.width, sisObjs[i]->gethealth(), sisObjs[i]->getMaxHp());
                }
            }
        }

        DrawRectangleRec(p2Panel, GetColor(0x0B080CFF));
        DrawRectangleLinesEx(p2Panel, 2, GetColor(0x132A4AFF));
        DrawRectangleLines((int)p2Panel.x - 2, (int)p2Panel.y - 2, (int)p2Panel.width + 4, (int)p2Panel.height + 4, GetColor(0x111E2EFF));

        Rectangle sherRect = { p2Panel.x + 12, p2Panel.y + 18, p2Panel.width - 24, p2Panel.height * 0.40f };
        if (sherArt.id > 0) {
            DrawTexturePro(sherArt, {0, 0, (float)sherArt.width, (float)sherArt.height}, sherRect, {0, 0}, 0.0f, WHITE);
        }
        DrawRectangleLinesEx(sherRect, 2, GetColor(0x1C3D66FF));

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
        DrawRectangleLinesEx(watsonRect, 2, GetColor(0x1C3D66FF));

        if (watson) {
            if (!watson->isalive()) {
                DrawText("[DEAD]", (int)watsonRect.x + 2, (int)(watsonRect.y + watsonRect.height + 6), 8, RED);
            } else {
                DrawGothicHealthBar(watsonRect.x, watsonRect.y + watsonRect.height + 6, watsonRect.width, watson->gethealth(), watson->getMaxHp());
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
    CloseWindow();
}