#include "age_screen.hpp"
#include "raylib.h"
#include <string>

static const Color GOLD_C = { 212, 175, 55, 255 };
static const Color GOLD_LIGHT_C = { 238, 205, 92, 255 };
static const Color PANEL_C = { 10, 12, 18, 248 };
static const Color INPUT_C = { 7, 9, 14, 255 };
static const Color TEXT_C = { 225, 225, 220, 255 };
static const Color DIM_C = { 135, 140, 150, 255 };
static const Color RED_C = { 105, 10, 17, 255 };
static const Color RED_LIGHT_C = { 165, 25, 32, 255 };

static void CenterText(Font font, const char* text, float centerX, float y, float size, float spacing, Color color)
{
    Vector2 m = MeasureTextEx(font, text, size, spacing);
    DrawTextEx(font, text, { centerX - m.x / 2.0f, y }, size, spacing, color);
}

static void VerticalArrows(float x, float y, Color color)
{
    DrawText("/\\", (int)x, (int)y, 12, color);
    DrawText("\\/", (int)x, (int)y + 13, 12, color);
}

AgeScreenResult AgeScreen::show()
{
    const int W = 1600;
    const int H = 900;

    InitWindow(W, H, "Unmatched - Assessment of Mortality");
    SetTargetFPS(60);

    Texture2D bg = LoadTexture("E:/assassins/new/unmatched/assets/menu_bg.png");
    if (bg.id != 0) SetTextureFilter(bg, TEXTURE_FILTER_BILINEAR);

    Font titleFont = LoadFontEx("E:/assassins/new/unmatched/assets/fonts/Cinzel-Black.ttf", 72, nullptr, 0);
    Font normalFont = LoadFontEx("E:/assassins/new/unmatched/assets/fonts/Cinzel-Regular.ttf", 56, nullptr, 0);
    Font semiFont = LoadFontEx("E:/assassins/new/unmatched/assets/fonts/Cinzel-SemiBold.ttf", 56, nullptr, 0);

    if (titleFont.texture.id != 0) SetTextureFilter(titleFont.texture, TEXTURE_FILTER_BILINEAR);
    if (normalFont.texture.id != 0) SetTextureFilter(normalFont.texture, TEXTURE_FILTER_BILINEAR);
    if (semiFont.texture.id != 0) SetTextureFilter(semiFont.texture, TEXTURE_FILTER_BILINEAR);

    int player1Age = 0;
    int player2Age = 0;
    int activeField = 1;
    bool submitted = false;

    while (!WindowShouldClose())
    {
        Vector2 mouse = GetMousePosition();

        Rectangle card = { 520, 50, 560, 800 };
        Rectangle box1 = { card.x + 45, card.y + 205, card.width - 90, 60 };
        Rectangle box2 = { card.x + 45, card.y + 325, card.width - 90, 60 };
        Rectangle okButton = { card.x + 45, card.y + 420, 220, 52 };
        Rectangle retreatButton = { card.x + 295, card.y + 420, 220, 52 };

        bool box1Hover = CheckCollisionPointRec(mouse, box1);
        bool box2Hover = CheckCollisionPointRec(mouse, box2);
        bool okHover = CheckCollisionPointRec(mouse, okButton);
        bool retreatHover = CheckCollisionPointRec(mouse, retreatButton);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (box1Hover) activeField = 1;
            else if (box2Hover) activeField = 2;
        }

        if (!submitted)
        {
            int key = GetCharPressed();
            while (key > 0)
            {
                if (key >= '0' && key <= '9')
                {
                    if (activeField == 1)
                    {
                        if (player1Age < 99) player1Age = player1Age * 10 + key - '0';
                        if (player1Age > 99) player1Age = 99;
                    }
                    else
                    {
                        if (player2Age < 99) player2Age = player2Age * 10 + key - '0';
                        if (player2Age > 99) player2Age = 99;
                    }
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE))
            {
                if (activeField == 1) player1Age /= 10;
                else player2Age /= 10;
            }

            if (IsKeyPressed(KEY_TAB)) activeField = (activeField == 1) ? 2 : 1;

            if (IsKeyPressed(KEY_ENTER) && player1Age > 0 && player2Age > 0) submitted = true;
            if (okHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && player1Age > 0 && player2Age > 0) submitted = true;

            if (retreatHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                UnloadFont(titleFont); UnloadFont(normalFont); UnloadFont(semiFont); UnloadTexture(bg);
                CloseWindow();
                return AgeScreenResult::EqualAge;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        if (bg.id != 0)
        {
            DrawTexturePro(bg, { 0, 0, (float)bg.width, (float)bg.height }, { 0, 0, (float)W, (float)H }, { 0, 0 }, 0, WHITE);
        }

        DrawRectangle(0, 0, W, H, { 0, 0, 0, 170 });
        DrawRectangleRounded(card, 0.04f, 20, PANEL_C);
        DrawRectangleRoundedLinesEx(card, 0.04f, 20, 1.5f, GOLD_C);

        CenterText(titleFont, "ASSESSMENT OF", card.x + card.width / 2, card.y + 45, 27, 1.2f, GOLD_C);
        CenterText(titleFont, "MORTALITY", card.x + card.width / 2, card.y + 80, 29, 1.2f, GOLD_LIGHT_C);

        DrawLine((int)(card.x + 80), (int)(card.y + 125), (int)(card.x + card.width - 80), (int)(card.y + 125), { 212, 175, 55, 80 });

        // Player 1 Box
        DrawTextEx(semiFont, "LIFE CYCLE OF PLAYER 1:", { box1.x, box1.y - 32 }, 14, 0.8f, GOLD_C);
        DrawRectangleRounded(box1, 0.18f, 12, INPUT_C);
        DrawRectangleRoundedLinesEx(box1, 0.18f, 12, 1.3f, activeField == 1 ? GOLD_C : Color{ 40, 45, 58, 255 });

        std::string age1 = (player1Age == 0) ? "Enter age" : std::to_string(player1Age);
        CenterText(normalFont, age1.c_str(), box1.x + box1.width / 2 - 10, box1.y + 17, 19, 0.5f, player1Age == 0 ? Color{ 75, 80, 92, 255 } : TEXT_C);
        VerticalArrows(box1.x + box1.width - 32, box1.y + 15, activeField == 1 ? GOLD_C : Color{ 80, 85, 100, 255 });

        // Player 2 Box
        DrawTextEx(semiFont, "LIFE CYCLE OF PLAYER 2:", { box2.x, box2.y - 32 }, 14, 0.8f, GOLD_C);
        DrawRectangleRounded(box2, 0.18f, 12, INPUT_C);
        DrawRectangleRoundedLinesEx(box2, 0.18f, 12, 1.3f, activeField == 2 ? GOLD_C : Color{ 40, 45, 58, 255 });

        std::string age2 = (player2Age == 0) ? "Enter age" : std::to_string(player2Age);
        CenterText(normalFont, age2.c_str(), box2.x + box2.width / 2 - 10, box2.y + 17, 19, 0.5f, player2Age == 0 ? Color{ 75, 80, 92, 255 } : TEXT_C);
        VerticalArrows(box2.x + box2.width - 32, box2.y + 15, activeField == 2 ? GOLD_C : Color{ 80, 85, 100, 255 });

        // OK Button
        DrawRectangleRounded(okButton, 0.18f, 12, okHover ? GOLD_LIGHT_C : GOLD_C);
        CenterText(semiFont, "OK", okButton.x + okButton.width / 2, okButton.y + 15, 17, 0.8f, { 15, 15, 20, 255 });

        // Retreat Button
        DrawRectangleRounded(retreatButton, 0.18f, 12, retreatHover ? Color{ 35, 39, 52, 255 } : Color{ 18, 21, 30, 255 });
        DrawRectangleRoundedLinesEx(retreatButton, 0.18f, 12, 1, { 55, 60, 75, 255 });
        CenterText(semiFont, "RETREAT", retreatButton.x + retreatButton.width / 2, retreatButton.y + 15, 17, 0.8f, retreatHover ? GOLD_C : DIM_C);

        if (submitted)
        {
            Rectangle resultBox = { card.x + 45, card.y + 500, card.width - 90, 190 };
            DrawRectangleRounded(resultBox, 0.08f, 12, { 27, 9, 13, 250 });
            DrawRectangleRoundedLinesEx(resultBox, 0.08f, 12, 1.2f, { 115, 25, 32, 255 });

            CenterText(titleFont, "VERDICT", resultBox.x + resultBox.width / 2, resultBox.y + 12, 19, 1.0f, RED_LIGHT_C);

            std::string experience = (player1Age < player2Age) ? "PLAYER II IS MORE EXPERIENCED" : 
                                     (player2Age < player1Age) ? "PLAYER I IS MORE EXPERIENCED" : "BOTH PLAYERS SHARE EQUAL EXPERIENCE";
            CenterText(semiFont, experience.c_str(), resultBox.x + resultBox.width / 2, resultBox.y + 48, 13, 0.4f, TEXT_C);
            DrawLine((int)(resultBox.x + 30), (int)(resultBox.y + 75), (int)(resultBox.x + resultBox.width - 30), (int)(resultBox.y + 75), { 110, 25, 32, 150 });

            std::string starter = (player1Age <= player2Age) ? "PLAYER I CHOOSES THEIR HERO FIRST" : "PLAYER II CHOOSES THEIR HERO FIRST";
            CenterText(normalFont, starter.c_str(), resultBox.x + resultBox.width / 2, resultBox.y + 86, 12, 0.3f, GOLD_C);

            Rectangle advanceButton = { resultBox.x + 30, resultBox.y + 125, resultBox.width - 60, 42 };
            bool advanceHover = CheckCollisionPointRec(mouse, advanceButton);

            DrawRectangleRounded(advanceButton, 0.18f, 12, advanceHover ? RED_LIGHT_C : RED_C);
            CenterText(semiFont, "ADVANCE TO HERO SELECTION", advanceButton.x + advanceButton.width / 2, advanceButton.y + 12, 12, 0.4f, WHITE);

            if (advanceHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                AgeScreenResult res = (player1Age < player2Age) ? AgeScreenResult::Player1First :
                                      (player2Age < player1Age) ? AgeScreenResult::Player2First : AgeScreenResult::EqualAge;
                UnloadFont(titleFont); UnloadFont(normalFont); UnloadFont(semiFont); UnloadTexture(bg);
                CloseWindow();
                return res;
            }
        }

        EndDrawing();
    }

    UnloadFont(titleFont); UnloadFont(normalFont); UnloadFont(semiFont); UnloadTexture(bg);
    CloseWindow();
    return AgeScreenResult::EqualAge;
}