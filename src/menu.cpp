#include "menu.hpp"
#include "raylib.h"

MenuResult Menu::show()
{
    const int screenWidth = 1600;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Unmatched - Main Menu");
    SetTargetFPS(60);

    Texture2D bg = LoadTexture("assets/menu_bg.png");
    if (bg.id != 0)
    {
        SetTextureFilter(bg, TEXTURE_FILTER_BILINEAR);
    }

    Font titleFont = LoadFontEx("assets/fonts/Cinzel-Black.ttf", 110, nullptr, 0);
    Font semiFont = LoadFontEx("assets/fonts/Cinzel-SemiBold.ttf", 56, nullptr, 0);
    Font regularFont = LoadFontEx("assets/fonts/Cinzel-Regular.ttf", 56, nullptr, 0);

    if (titleFont.texture.id != 0) SetTextureFilter(titleFont.texture, TEXTURE_FILTER_BILINEAR);
    if (semiFont.texture.id != 0) SetTextureFilter(semiFont.texture, TEXTURE_FILTER_BILINEAR);
    if (regularFont.texture.id != 0) SetTextureFilter(regularFont.texture, TEXTURE_FILTER_BILINEAR);

    Rectangle startBtn = { (float)screenWidth / 2 - 210, 420, 420, 64 };
    Rectangle loadBtn  = { (float)screenWidth / 2 - 210, 505, 420, 56 };
    Rectangle exitBtn  = { (float)screenWidth / 2 - 210, 580, 420, 56 };

    Color gold = { 212, 175, 55, 255 };
    Color glass = { 20, 24, 38, 190 };
    Color glassHover = { 32, 38, 58, 220 };
    Color redBtn = { 135, 12, 18, 255 };
    Color redHover = { 165, 20, 28, 255 };

    while (!WindowShouldClose())
    {
        Vector2 mouse = GetMousePosition();

        bool hoverStart = CheckCollisionPointRec(mouse, startBtn);
        bool hoverLoad  = CheckCollisionPointRec(mouse, loadBtn);
        bool hoverExit  = CheckCollisionPointRec(mouse, exitBtn);

        if (hoverStart && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            UnloadFont(titleFont);
            UnloadFont(semiFont);
            UnloadFont(regularFont);
            UnloadTexture(bg);
            CloseWindow();
            return MenuResult::Start;
        }

        if (hoverLoad && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            UnloadFont(titleFont);
            UnloadFont(semiFont);
            UnloadFont(regularFont);
            UnloadTexture(bg);
            CloseWindow();
            return MenuResult::Load;
        }

        if (hoverExit && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            UnloadFont(titleFont);
            UnloadFont(semiFont);
            UnloadFont(regularFont);
            UnloadTexture(bg);
            CloseWindow();
            return MenuResult::Exit;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        if (bg.id != 0)
        {
            DrawTexturePro(
                bg,
                { 0, 0, (float)bg.width, (float)bg.height },
                { 0, 0, (float)screenWidth, (float)screenHeight },
                { 0, 0 },
                0,
                WHITE
            );
        }

        DrawRectangle(0, 0, screenWidth, screenHeight, { 0, 0, 0, 140 });

        Vector2 subTextSize = MeasureTextEx(regularFont,
                                            "THE ETERNAL BATTLE OF SHADOWS",
                                            20, 1.2f);

        DrawTextEx(regularFont,
                   "THE ETERNAL BATTLE OF SHADOWS",
                   { (float)screenWidth / 2 - subTextSize.x / 2.0f, 250 },
                   20, 1.2f,
                   { 180, 140, 70, 255 });

        Vector2 titleSize = MeasureTextEx(titleFont, "UNMATCHED", 76, 1.0f);

        DrawTextEx(titleFont,
                   "UNMATCHED",
                   { (float)screenWidth / 2 - titleSize.x / 2.0f, 285 },
                   76, 1.0f,
                   gold);

        
        DrawRectangleRounded(startBtn, 0.22f, 20,
                             hoverStart ? redHover : redBtn);

        DrawRectangleRoundedLines(startBtn, 0.22f, 20, 1.2f,
                                    { 255, 255, 255, 35 });

        Vector2 startTextSize = MeasureTextEx(semiFont, "START", 22, 1.0f);

        DrawTextEx(semiFont,
                   "START",
                   {
                       startBtn.x + (startBtn.width - startTextSize.x) / 2.0f,
                       startBtn.y + 17
                   },
                   22, 1.0f,
                   WHITE);

        
        DrawRectangleRounded(loadBtn, 0.22f, 20,
                             hoverLoad ? glassHover : glass);

        DrawRectangleRoundedLines(loadBtn, 0.22f, 20, 1.2f,
                                    { 255, 255, 255, 35 });

        DrawText("[ SAVE ]",
                 (int)(loadBtn.x + 30),
                 (int)(loadBtn.y + 18),
                 16,
                 gold);

        Vector2 loadTextSize = MeasureTextEx(regularFont,
                                             "LOAD GAME",
                                             19, 0.8f);

        DrawTextEx(regularFont,
                   "LOAD GAME",
                   {
                       loadBtn.x + (loadBtn.width / 2.0f) - (loadTextSize.x / 2.0f) + 12,
                       loadBtn.y + 16
                   },
                   19, 0.8f,
                   { 230, 230, 235, 255 });

        
        DrawRectangleRounded(exitBtn, 0.22f, 20,
                             hoverExit ? glassHover : glass);

        DrawRectangleRoundedLines(exitBtn, 0.22f, 20, 1.2f,
                                    { 255, 255, 255, 35 });

        Vector2 exitTextSize = MeasureTextEx(regularFont,
                                             "EXIT",
                                             19, 0.8f);

        DrawTextEx(regularFont,
                   "EXIT",
                   {
                       exitBtn.x + (exitBtn.width - exitTextSize.x) / 2.0f,
                       exitBtn.y + 16
                   },
                   19, 0.8f,
                   { 230, 230, 235, 255 });

        EndDrawing();
    }

    UnloadFont(titleFont);
    UnloadFont(semiFont);
    UnloadFont(regularFont);
    UnloadTexture(bg);

    CloseWindow();
    return MenuResult::Exit;
}