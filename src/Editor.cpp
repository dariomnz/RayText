#include "Editor.hpp"
#include "Command.hpp"
#include "Cursor.hpp"
#include "TextFile.hpp"
#include "Directory.hpp"

constexpr int keys_to_check[8] = {KEY_RIGHT,
                                  KEY_LEFT,
                                  KEY_UP,
                                  KEY_DOWN,
                                  KEY_TAB,
                                  KEY_BACKSPACE,
                                  KEY_DELETE,
                                  KEY_ENTER};
TextFile &Editor::GetCurrentTextFile()
{
    return textFiles[currentTextFile_index];
}

Editor::Editor(std::vector<TextFile> &ref) : textFiles(ref)
{
    DEBUG_MSG("Construct Editor");
    screenWidth = 800;
    screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "RayText");

    Image icon = LoadImage("." PATH_SEPARATOR "resources" PATH_SEPARATOR "images" PATH_SEPARATOR "icon.png");
    SetWindowIcon(icon);
    UnloadImage(icon);

    font = LoadFontEx("." PATH_SEPARATOR "resources" PATH_SEPARATOR "fonts" PATH_SEPARATOR "Monaco.ttf", 82, 0, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    currentTextFile_index = 0;
    GetCurrentTextFile().Load();

    zoom = 1.0f;
    camera.zoom = 1.0f;
    camera.offset = (Vector2){(float)screenWidth / 2, (float)screenHeight / 2};

    editor_state = STATE_TEXTFILE;
    cursor_pos = Vector2Zero();
}

Editor::~Editor()
{
    DEBUG_MSG("delete Editor");
    // TextFile_Free(&currentTextFile);
    Directory_Free(&currentDirectory);
    // Command_Free(&editor->currentCommand);
    UnloadFont(font);

    CloseWindow(); // Close window and OpenGL context
}

void Editor::Logic()
{
    if (IsWindowResized())
    {
        screenWidth = GetScreenWidth();
        screenHeight = GetScreenHeight();

        camera.offset = (Vector2){(float)screenWidth / 2, (float)screenHeight / 2};
    }

    key_pressed = KEY_NULL;
    for (const int *key = keys_to_check; key < keys_to_check + sizeof(keys_to_check) / sizeof(keys_to_check[0]); key++)
    {
        if (IsKeyPressed(*key) || IsKeyPressedRepeat(*key))
            key_pressed = (KeyboardKey)*key;
    }
    char_pressed = GetCharPressed();

    GetCurrentTextFile().Logic(*this);
    Directory_Logic(this);
    Cursor_Logic(this);

    // Zoom based on mouse wheel
    float wheel = GetMouseWheelMove();
    if (wheel != 0)
    {
        const float zoomIncrement = 0.125f;

        zoom += (wheel * zoomIncrement);
        if (zoom < zoomIncrement)
            zoom = zoomIncrement;
    }

    if (Vector2EqualsEpsilon(cursor_pos, camera.target, 0.1f) == 0)
        camera.target = Vector2Lerp(camera.target, cursor_pos, 10.0f * GetFrameTime());

    if (FloatEqualsEpsilon(camera.zoom, zoom, 0.01f) == 0)
        camera.zoom = Lerp(camera.zoom, zoom, 10.0f * GetFrameTime());
}

void Editor::Draw()
{
    ClearBackground(BLACK);

    BeginMode2D(camera);

    GetCurrentTextFile().Draw(*this);
    Directory_Draw(this);
    Cursor_Draw(this);

    EndMode2D();

    GetCurrentTextFile().DrawNoCamera(*this);

    DrawFPS(0, 0);
}