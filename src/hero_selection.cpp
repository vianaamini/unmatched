#include "hero_selection.hpp"
#include "raylib.h"
#include <string>

static const Color GOLD_COLOR = { 207, 169, 82, 255 };
static const Color GOLD_LIGHT_COLOR = { 238, 211, 148, 255 };
static const Color PANEL_COLOR = { 20, 20, 28, 245 };
static const Color PANEL_HOVER_COLOR = { 27, 27, 36, 255 };
static const Color TEXT_COLOR = { 235, 231, 220, 255 };
static const Color DIM_COLOR = { 145, 150, 165, 255 };
static const Color RED_COLOR = { 125, 8, 17, 255 };
static const Color RED_HOVER_COLOR = { 170, 15, 25, 255 };

static void CenterText(Font font, const char* text, float x, float y, float size, float spacing, Color color)
{
    Vector2 textSize = MeasureTextEx(font, text, size, spacing);
    DrawTextEx(font, text, { x - textSize.x / 2.0f, y }, size, spacing, color);
}

struct FighterCard
{
    const char* name;
    const char* title;
    const char* imagePath;
    Texture2D texture;
};

HeroSelectionResult HeroSelection::show(int firstPlayer)
{
    const int WIDTH = 1600;
    const int HEIGHT = 900;

    InitWindow(WIDTH, HEIGHT, "Unmatched - Hero Selection");
    SetTargetFPS(60);

    Texture2D background = LoadTexture("assets/menu_bg.png");
    if (background.id != 0) SetTextureFilter(background, TEXTURE_FILTER_BILINEAR);

    Font titleFont = LoadFontEx("assets/fonts/Cinzel-Black.ttf", 76, nullptr, 0);
    Font regularFont = LoadFontEx("assets/fonts/Cinzel-Regular.ttf", 60, nullptr, 0);
    Font semiBoldFont = LoadFontEx("assets/fonts/Cinzel-SemiBold.ttf", 60, nullptr, 0);

    if (titleFont.texture.id != 0) SetTextureFilter(titleFont.texture, TEXTURE_FILTER_BILINEAR);
    if (regularFont.texture.id != 0) SetTextureFilter(regularFont.texture, TEXTURE_FILTER_BILINEAR);
    if (semiBoldFont.texture.id != 0) SetTextureFilter(semiBoldFont.texture, TEXTURE_FILTER_BILINEAR);

    FighterCard fighters[3] =
    {
        { "DRACULA", "THE LORD OF BLOODS", "assets/heroes/dracula (1).png", {} },
        { "INVISIBLE MAN", "THE GHOST", "assets/heroes/tranInv (1).png", {} },
        { "SHERLOCK HOLMES", "THE GRAND DETECTIVE", "assets/heroes/sherlockTran (1).png", {} }
    };

    for (auto& fighter : fighters)
    {
        fighter.texture = LoadTexture(fighter.imagePath);
        if (fighter.texture.id != 0) SetTextureFilter(fighter.texture, TEXTURE_FILTER_BILINEAR);
    }

    int selectedFirst = -1;
    int selectedSecond = -1;
    bool firstSelected = false;
    bool secondSelected = false;
    int currentPlayer = firstPlayer;

    while (!WindowShouldClose())
    {
        Vector2 mouse = GetMousePosition();

        Rectangle cards[3] =
        {
            { 220, 240, 360, 520 },
            { 620, 240, 360, 520 },
            { 1020, 240, 360, 520 }
        };

        Rectangle enterButton = { 550, 785, 500, 58 };
        Rectangle retreatButton = { 650, 855, 300, 38 };

        for (int i = 0; i < 3; ++i)
        {
            bool hover = CheckCollisionPointRec(mouse, cards[i]);
            bool alreadySelected = (i == selectedFirst || i == selectedSecond);

            // قابلیت کلیک و انتخاب یا تغییر انتخاب
            if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                if (!firstSelected)
                {
                    selectedFirst = i;
                    firstSelected = true;
                    currentPlayer = (firstPlayer == 1) ? 2 : 1;
                }
                else if (!secondSelected && i != selectedFirst)
                {
                    selectedSecond = i;
                    secondSelected = true;
                }
                else
                {
                    // اگر هر دو انتخاب شده بودند و کاربر خواست دوباره از اول انتخاب کند، با کلیک روی کارت‌ها بازنشانی شود
                    selectedFirst = i;
                    selectedSecond = -1;
                    firstSelected = true;
                    secondSelected = false;
                    currentPlayer = (firstPlayer == 1) ? 2 : 1;
                }
            }
        }

        bool enterHover = CheckCollisionPointRec(mouse, enterButton);
        bool retreatHover = CheckCollisionPointRec(mouse, retreatButton);

        // تایید نهایی و ورود به بازی
        if (secondSelected && enterHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            std::string player1Hero = (firstPlayer == 1) ? fighters[selectedFirst].name : fighters[selectedSecond].name;
            std::string player2Hero = (firstPlayer == 1) ? fighters[selectedSecond].name : fighters[selectedFirst].name;

            for (auto& fighter : fighters) UnloadTexture(fighter.texture);
            UnloadFont(titleFont); UnloadFont(regularFont); UnloadFont(semiBoldFont); UnloadTexture(background);
            CloseWindow();

            return { HeroSelectionResultType::EnterGame, firstPlayer, player1Hero, player2Hero };
        }

        // دکمه ریتریت: پاک کردن انتخاب‌ها بدون بستن پنجره تا کاربر بتواند دوباره هیرو انتخاب کند
        if (retreatHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            selectedFirst = -1;
            selectedSecond = -1;
            firstSelected = false;
            secondSelected = false;
            currentPlayer = firstPlayer;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        if (background.id != 0)
        {
            DrawTexturePro(background, { 0, 0, (float)background.width, (float)background.height }, { 0, 0, (float)WIDTH, (float)HEIGHT }, { 0, 0 }, 0, WHITE);
        }

        DrawRectangle(0, 0, WIDTH, HEIGHT, { 0, 0, 0, 170 });

        std::string heading = "PLAYER " + std::to_string(currentPlayer) + ", CHOOSE YOUR LEGEND";
        CenterText(titleFont, heading.c_str(), WIDTH / 2.0f, 100, 38, 1.2f, GOLD_LIGHT_COLOR);

        CenterText(regularFont, "SELECT ONE FIGHTER TO STEP INTO THE FOG", WIDTH / 2.0f, 158, 17, 1.0f, DIM_COLOR);
        DrawLine((WIDTH / 2) - 140, 192, (WIDTH / 2) + 140, 192, { 212, 175, 55, 90 });

        for (int i = 0; i < 3; ++i)
        {
            Rectangle card = cards[i];
            bool hover = CheckCollisionPointRec(mouse, card);
            bool selected = (i == selectedFirst || i == selectedSecond);

            Color cardColor = selected ? Color{ 32, 28, 20, 255 } : (hover ? PANEL_HOVER_COLOR : PANEL_COLOR);
            DrawRectangleRounded(card, 0.045f, 16, cardColor);

            Color borderColor = selected ? GOLD_LIGHT_COLOR : (hover ? GOLD_COLOR : Color{ 58, 54, 47, 255 });
            DrawRectangleRoundedLines(card, 0.045f, 16, selected ? 2.5f : 1.2f, borderColor);

            Rectangle imageArea = { card.x + 20, card.y + 25, card.width - 40, 340 };
            Texture2D texture = fighters[i].texture;

            if (texture.id != 0)
            {
                float scaleX = imageArea.width / (float)texture.width;
                float scaleY = imageArea.height / (float)texture.height;
                float scale = (scaleX < scaleY) ? scaleX : scaleY;

                float imageWidth = texture.width * scale;
                float imageHeight = texture.height * scale;

                Rectangle source = { 0, 0, (float)texture.width, (float)texture.height };
                Rectangle destination = {
                    imageArea.x + (imageArea.width - imageWidth) / 2.0f,
                    imageArea.y + (imageArea.height - imageHeight) / 2.0f,
                    imageWidth,
                    imageHeight
                };

                DrawTexturePro(texture, source, destination, { 0, 0 }, 0, WHITE);
            }

            DrawLine((int)(card.x + 20), (int)(card.y + 390), (int)(card.x + card.width - 20), (int)(card.y + 390), { 55, 52, 48, 150 });
            CenterText(titleFont, fighters[i].name, card.x + card.width / 2.0f, card.y + 412, 24, 0.8f, TEXT_COLOR);

            Color titleColor = (i == 0) ? Color{ 210, 35, 42, 255 } : (i == 2) ? GOLD_COLOR : DIM_COLOR;
            CenterText(semiBoldFont, fighters[i].title, card.x + card.width / 2.0f, card.y + 460, 14, 0.7f, titleColor);
        }

        if (secondSelected)
        {
            DrawRectangleRounded(enterButton, 0.18f, 12, enterHover ? RED_HOVER_COLOR : RED_COLOR);
            CenterText(semiBoldFont, "ENTER THE FOG", enterButton.x + enterButton.width / 2.0f, enterButton.y + 16, 20, 0.9f, WHITE);
        }
        else
        {
            CenterText(regularFont, "The drafting phase has begun.", WIDTH / 2.0f, 795, 15, 0.5f, DIM_COLOR);
        }

        DrawRectangleRounded(retreatButton, 0.18f, 10, retreatHover ? Color{ 35, 35, 43, 255 } : Color{ 18, 18, 25, 255 });
        DrawRectangleRoundedLines(retreatButton, 0.18f, 10, 1, { 55, 55, 65, 255 });
        CenterText(semiBoldFont, "RESET CHOICES", retreatButton.x + retreatButton.width / 2.0f, retreatButton.y + 9, 13, 0.7f, DIM_COLOR);

        EndDrawing();
    }

    for (auto& fighter : fighters) UnloadTexture(fighter.texture);
    UnloadFont(titleFont); UnloadFont(regularFont); UnloadFont(semiBoldFont); UnloadTexture(background);
    CloseWindow();

    return { HeroSelectionResultType::Retreat, firstPlayer, "", "" };
}