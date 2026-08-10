#include "raylib.h"
#include "../include/map.hpp"
#include <vector>
#include <string>
#include <iostream>

Texture2D LoadTextureWithFallbacksForMain(const std::string& category, const std::vector<std::string>& filenames) {
    std::vector<std::string> basePaths = {
        "assets/",
    "assets/heroes/",
    "assets/images/",
    "../assets/",
    "../assets/heroes/",
    "../assets/images/",
    "build/assets/",
    "build/assets/heroes/"
    };

    for (const auto& filename : filenames) {
        for (const auto& basePath : basePaths) {
            std::string fullPath = basePath + (category.empty() ? "" : category + "/") + filename;
            
            if (FileExists(fullPath.c_str())) {
                Texture2D tex = LoadTexture(fullPath.c_str());
                if (tex.id > 0) {
                    SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);
                    std::cout << "[SUCCESS] Loaded: " << fullPath << std::endl;
                    return tex;
                }
            }
        }
    }
    std::cout << "[ERROR] Could not find texture for: " << (filenames.empty() ? "" : filenames[0]) << std::endl;
    return Texture2D{ 0 };
}

void RunGameUI(Board& board) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1366, 768, "Unmatched Game - Raylib View");
    SetTargetFPS(60);

    Texture2D boardTex = LoadTextureWithFallbacksForMain("", {"board(1).png", "board.jpg", "board.png", "board"});
    Texture2D dracArt = LoadTextureWithFallbacksForMain("", {"DracArt.png", });
    Texture2D sis1   = LoadTextureWithFallbacksForMain( "",{"sis1.png"});
    Texture2D sis2   = LoadTextureWithFallbacksForMain("" ,{"sis2.png"});;
    Texture2D sis3   = LoadTextureWithFallbacksForMain("" ,{"sis3.png"});
    Texture2D sherArt   = LoadTextureWithFallbacksForMain("", {"holmsArtTransparent.png", "sherlockTran (1).png"});
    Texture2D watsonArt = LoadTextureWithFallbacksForMain("", {"drwatson.png"});

    while (!WindowShouldClose()) {
        float sw = (float)GetScreenWidth();
        float sh = (float)GetScreenHeight();

        Rectangle p1Panel = { sw * 0.02f, sh * 0.03f, sw * 0.19f, sh * 0.72f };
        Rectangle mapDest = { sw * 0.23f, sh * 0.03f, sw * 0.54f, sh * 0.72f };
        Rectangle p2Panel = { sw * 0.79f, sh * 0.03f, sw * 0.19f, sh * 0.72f };

        BeginDrawing();
        ClearBackground(GetColor(0x111115FF));

        // --- ۱. رسم بورد بازی در وسط صفحه (مشکل بورد حل شد) ---
        if (boardTex.id > 0) {
            DrawTexturePro(boardTex,
                           Rectangle{ 0.0f, 0.0f, (float)boardTex.width, (float)boardTex.height },
                           mapDest,
                           Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        } else {
            DrawRectangleRec(mapDest, DARKGRAY);
            DrawRectangleLinesEx(mapDest, 2, GOLD);
        }

        // --- ۲. رسم پنل بازیکن اول (دراکولا) ---
        DrawRectangleRec(p1Panel, GetColor(0x1C1C24FF));
        DrawRectangleLinesEx(p1Panel, 2, RED);

        // عکس دراکولا (مشکل عکس دراکولا حل شد)
        Rectangle dracRect = { p1Panel.x + 10, p1Panel.y + 15, p1Panel.width - 20, p1Panel.height * 0.42f };
        if (dracArt.id > 0) {
            DrawTexturePro(dracArt,
                           Rectangle{ 0.0f, 0.0f, (float)dracArt.width, (float)dracArt.height },
                           dracRect,
                           Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        } else {
            DrawRectangleRec(dracRect, DARKGRAY);
        }
        DrawRectangleLinesEx(dracRect, 2, MAROON);

        // عکس خواهرها
        float sisStartY = dracRect.y + dracRect.height + 25;
        float sisWidth  = (p1Panel.width - 26) / 3.0f;
        float sisHeight = sisWidth * 1.15f;
        Texture2D sisTextures[3] = { sis1, sis2, sis3 };

        for (int i = 0; i < 3; i++) {
            Rectangle sisBox = { p1Panel.x + 10 + i * (sisWidth + 3), sisStartY, sisWidth, sisHeight };
            if (sisTextures[i].id > 0) {
                DrawTexturePro(sisTextures[i],
                               Rectangle{ 0.0f, 0.0f, (float)sisTextures[i].width, (float)sisTextures[i].height },
                               sisBox,
                               Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
            } else {
                DrawRectangleRec(sisBox, DARKGRAY);
            }
            DrawRectangleLinesEx(sisBox, 1, MAROON);
        }

        // --- ۳. رسم پنل بازیکن دوم (شرلوک و واتسون) ---
        DrawRectangleRec(p2Panel, GetColor(0x1C1C24FF));
        DrawRectangleLinesEx(p2Panel, 2, BLUE);

        Rectangle sherRect = { p2Panel.x + 10, p2Panel.y + 15, p2Panel.width - 20, p2Panel.height * 0.42f };
        if (sherArt.id > 0) {
            DrawTexturePro(sherArt,
                           Rectangle{ 0.0f, 0.0f, (float)sherArt.width, (float)sherArt.height },
                           sherRect,
                           Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        }

        Rectangle watsonRect = { p2Panel.x + (p2Panel.width - (p2Panel.width * 0.5f)) / 2.0f, sherRect.y + sherRect.height + 20, p2Panel.width * 0.5f, p2Panel.height * 0.25f };
        if (watsonArt.id > 0) {
             DrawTexturePro(watsonArt,
                   Rectangle{ 0.0f, 0.0f, (float)watsonArt.width, (float)watsonArt.height },
                   watsonRect,
                   Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
            }
        DrawRectangleLinesEx(watsonRect, 2, BLUE);
        EndDrawing();
    }

    UnloadTexture(boardTex);
    UnloadTexture(dracArt);
    UnloadTexture(sis1);
    UnloadTexture(sherArt);
    UnloadTexture(watsonArt);
    CloseWindow();
}