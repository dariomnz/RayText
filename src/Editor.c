#include "Editor.h"

Editor editor = {0};

int keys_to_check[8] = {KEY_RIGHT,
                        KEY_LEFT,
                        KEY_UP,
                        KEY_DOWN,
                        KEY_TAB,
                        KEY_BACKSPACE,
                        KEY_DELETE,
                        KEY_ENTER};

void Editor_Init()
{
    if (editor.initialized != 0)
        return;
    editor.screenWidth = 800;
    editor.screenHeight = 450;
    InitWindow(editor.screenWidth, editor.screenHeight, "RayText");

    editor.font = LoadFontEx("./fonts/Monaco.ttf", 82, 0, 0);
    SetTextureFilter(editor.font.texture, TEXTURE_FILTER_BILINEAR);
    editor.font_size = 20;
    editor.font_spacing = 0;
    memset(&editor.currentCommand, 0, sizeof(DArray_char));
    editor.currentTextFile = TextFile_LoadEmpty();
    memset(&editor.currentDirectory, 0, sizeof(Directory));

    editor.zoom = 1.0f;
    editor.camera.zoom = 1.0f;
    editor.camera.offset = (Vector2){editor.screenWidth / 2, editor.screenHeight / 2};

    editor.editor_state = STATE_TEXTFILE;
    editor.cursor_pos = Vector2Zero();

    editor.the_end = false;
}

void Editor_Logic()
{
    if (IsWindowResized())
    {
        editor.screenWidth = GetScreenWidth();
        editor.screenHeight = GetScreenHeight();

        editor.camera.offset = (Vector2){editor.screenWidth / 2, editor.screenHeight / 2};
    }

    editor.key_pressed = 0;
    FOR_EACH(int, key, keys_to_check)
    {
        if (IsKeyPressed(*key) || IsKeyPressedRepeat(*key))
            editor.key_pressed = *key;
    }
    editor.char_pressed = GetCharPressed();

    TextFile_Logic();
    Command_Logic();
    Directory_Logic();

    // Zoom based on mouse wheel
    float wheel = GetMouseWheelMove();
    if (wheel != 0)
    {
        const float zoomIncrement = 0.125f;

        editor.zoom += (wheel * zoomIncrement);
        if (editor.zoom < zoomIncrement)
            editor.zoom = zoomIncrement;
    }

    if (Vector2EqualsEpsilon(editor.cursor_pos, editor.camera.target, 0.1f) == 0)
        editor.camera.target = Vector2Lerp(editor.camera.target, editor.cursor_pos, 0.2f);

    if (FloatEqualsEpsilon(editor.camera.zoom, editor.zoom, 0.01f) == 0)
        editor.camera.zoom = Lerp(editor.camera.zoom, editor.zoom, 0.1f);
}

void Editor_Draw()
{
    ClearBackground(BLACK);

    BeginMode2D(editor.camera);

    // Draw text file
    TextFile_Draw();

    // Draw dir
    Directory_Draw();

    EndMode2D();

    // Draw menu
    Command_Draw();

    DrawFPS(0, 0);
}

void Editor_Close()
{
    TextFile_Free();
    Directory_Free();
    UnloadFont(editor.font);

    CloseWindow(); // Close window and OpenGL context
}