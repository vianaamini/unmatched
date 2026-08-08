#include "raylib.h"
#include "../include/map.hpp"
#include "../include/gui.hpp"
#include <vector>
#include <string>

Texture2D LoadTextureWithFallbacks(const std::string& category, const std::vector<std::string>& filenames) {
    std::vector<std::string> basePaths = {
        "assest/images/",
        "../assest/images/",
        "../../assest/images/",
        "build/assest/images/",
        "../build/assest/images/",
        "assets/images/",
        "../assets/images/"
    };

    for (const auto& filename : filenames) {
        for (const auto& basePath : basePaths) {
            std::string fullPath = basePath + (category.empty() ? "" : category + "/") + filename;
            if (FileExists(fullPath.c_str())) {
                Texture2D tex = LoadTexture(fullPath.c_str());
                if (tex.id > 0) return tex;
            }
        }
    }
    return Texture2D{ 0 };
}

void RunGameUI(Board& board) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1366, 768, "Unmatched Game - Raylib View");
    SetTargetFPS(60);

    Texture2D boardTex = LoadTextureWithFallbacks("", {"board.jpeg", "board.jpg", "board.png"});
    
    Texture2D dracArt  = LoadTextureWithFallbacks("dracula", {"DracArtTran.png", "DracArt.png"});
    Texture2D sis1     = LoadTextureWithFallbacks("dracula", {"sis1.webp", "sis1.png", "3sisters.png"});
    Texture2D sis2     = LoadTextureWithFallbacks("dracula", {"sis2.webp", "sis2.png", "3sisters.png"});
    Texture2D sis3     = LoadTextureWithFallbacks("dracula", {"sis3.webp", "sis3.png", "3sisters.png"});

    Texture2D sherArt   = LoadTextureWithFallbacks("sherlock", {"holmsArtTransparent.png", "holmsArt.png"});
    Texture2D watsonArt = LoadTextureWithFallbacks("sherlock", {"drwatson.png", "watsonHealth.png"});

    while (!WindowShouldClose()) {
        float sw = (float)GetScreenWidth();
        float sh = (float)GetScreenHeight();

        Rectangle p1Panel = { sw * 0.02f, sh * 0.03f, sw * 0.19f, sh * 0.72f };
        Rectangle mapDest = { sw * 0.23f, sh * 0.03f, sw * 0.54f, sh * 0.72f };
        Rectangle p2Panel = { sw * 0.79f, sh * 0.03f, sw * 0.19f, sh * 0.72f };

        BeginDrawing();
        ClearBackground(GetColor(0x111115FF));

        DrawRectangleRec(p1Panel, GetColor(0x1C1C24FF));
        DrawRectangleLinesEx(p1Panel, 2, RED);

        Rectangle dracRect = { p1Panel.x + 10, p1Panel.y + 15, p1Panel.width - 20, p1Panel.height * 0.42f };
        if (dracArt.id > 0) {
            DrawTexturePro(dracArt, {0, 0, (float)dracArt.width, (float)dracArt.height}, dracRect, {0, 0}, 0.0f, WHITE);
        } else {
            DrawRectangleRec(dracRect, DARKGRAY);
        }
        DrawRectangleLinesEx(dracRect, 2, MAROON);

        float sisStartY = dracRect.y + dracRect.height + 25;
        float sisWidth = (p1Panel.width - 26) / 3.0f; 
        float sisHeight = sisWidth * 1.15f;
        Texture2D sisTextures[3] = { sis1, sis2, sis3 };

        for (int i = 0; i < 3; i++) {
            Rectangle sisBox = { p1Panel.x + 10 + i * (sisWidth + 3), sisStartY, sisWidth, sisHeight };
            if (sisTextures[i].id > 0) {
                DrawTexturePro(sisTextures[i], {0, 0, (float)sisTextures[i].width, (float)sisTextures[i].height}, sisBox, {0, 0}, 0.0f, WHITE);
            } else {
                DrawRectangleRec(sisBox, DARKGRAY);
            }
            DrawRectangleLinesEx(sisBox, 1, MAROON);
        }

        if (boardTex.id > 0) {
            DrawTexturePro(boardTex, {0, 0, (float)boardTex.width, (float)boardTex.height}, mapDest, {0, 0}, 0.0f, WHITE);
            DrawRectangleLinesEx(mapDest, 2, GRAY);

            float scaleX = mapDest.width / (float)boardTex.width;
            float scaleY = mapDest.height / (float)boardTex.height;

            std::vector<std::string> spaceIds = board.getAllSpaceIds();

            for (const auto& spaceId : spaceIds) {
                auto coords = board.getCoordinates(spaceId);
                if (coords.first != -1 && coords.second != -1) {
                    float drawX = mapDest.x + (coords.first * scaleX);
                    float drawY = mapDest.y + (coords.second * scaleY);

                    DrawCircle((int)drawX, (int)drawY, 9.0f * scaleX, RED);
                    DrawCircleLines((int)drawX, (int)drawY, 9.0f * scaleX, WHITE);
                }
            }
        }

        DrawRectangleRec(p2Panel, GetColor(0x1C1C24FF));
        DrawRectangleLinesEx(p2Panel, 2, BLUE);

        Rectangle sherRect = { p2Panel.x + 10, p2Panel.y + 15, p2Panel.width - 20, p2Panel.height * 0.42f };
        if (sherArt.id > 0) {
            DrawTexturePro(sherArt, {0, 0, (float)sherArt.width, (float)sherArt.height}, sherRect, {0, 0}, 0.0f, WHITE);
        } else {
            DrawRectangleRec(sherRect, DARKGRAY);
        }
        DrawRectangleLinesEx(sherRect, 2, DARKBLUE);

        float watStartY = sherRect.y + sherRect.height + 25;
        float watSize = p2Panel.width * 0.48f;
        Rectangle watBox = { p2Panel.x + (p2Panel.width - watSize) / 2.0f, watStartY, watSize, watSize };
        
        if (watsonArt.id > 0) {
            DrawTexturePro(watsonArt, {0, 0, (float)watsonArt.width, (float)watsonArt.height}, watBox, {0, 0}, 0.0f, WHITE);
        } else {
            DrawRectangleRec(watBox, DARKGRAY);
        }
        DrawRectangleLinesEx(watBox, 1, SKYBLUE);

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