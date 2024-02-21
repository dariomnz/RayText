#include "Editor.h"

Editor editor = {0};

int keys_to_check[6] = {KEY_RIGHT,
                        KEY_LEFT,
                        KEY_UP,
                        KEY_DOWN,
                        KEY_BACKSPACE,
                        KEY_ENTER};

void Editor_Init()
{
    if (editor.initialized != 0)
        return;
    editor.screenWidth = 800;
    editor.screenHeight = 450;
    InitWindow(editor.screenWidth, editor.screenHeight, "RayText");

    editor.font = LoadFont("./fonts/Monaco.ttf");
    SetTextureFilter(editor.font.texture, TEXTURE_FILTER_ANISOTROPIC_8X);
    editor.font_size = 20;
    editor.font_spacing = 0;
    editor.currentCommand[0] = '\0';
    editor.is_commands_showing = false;
    editor.currentTextFile = TextFile_LoadEmpty();

    editor.camera.zoom = 1.0f;
    editor.camera.offset = (Vector2){editor.screenWidth / 2, editor.screenHeight / 2};

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

    TextFile_MoveCursor(&editor.currentTextFile);
    if (editor.key_pressed == KEY_BACKSPACE)
        if (editor.is_commands_showing)
            Command_RemoveChar();
        else
            TextFile_RemoveChar(&editor.currentTextFile);

    // DEBUG("char: %d", char_pressed);
    if (editor.char_pressed != 0)
    {
        if (editor.is_commands_showing)
            Command_InsertChar(editor.char_pressed);
        else
            TextFile_InsertChar(&editor.currentTextFile, editor.char_pressed);
    }
    if (editor.key_pressed == KEY_ENTER)
        if (editor.is_commands_showing)
        {
            Command_Consume();
            editor.is_commands_showing = false;
        }
        else
            TextFile_InsertNewLine(&editor.currentTextFile);

    editor.cursor_pos = TextFile_GetCursorPosition(editor.currentTextFile);

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_P))
    {
        editor.is_commands_showing = !editor.is_commands_showing;
        // DEBUG("is_commands_showing %d", is_commands_showing);
    }

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S))
    {
        TextFile_Save(editor.currentTextFile);
    }

    // Zoom based on mouse wheel
    float wheel = GetMouseWheelMove();
    if (wheel != 0)
    {
        const float zoomIncrement = 0.125f;

        editor.camera.zoom += (wheel * zoomIncrement);
        if (editor.camera.zoom < zoomIncrement)
            editor.camera.zoom = zoomIncrement;
    }

    if (Vector2EqualsEpsilon(editor.cursor_pos, editor.camera.target, 0.1f) == 0)
        editor.camera.target = Vector2Lerp(editor.camera.target, editor.cursor_pos, 0.2f);
}

int final_num_pos = 0;
char str_line_number[10];
Vector2 str_line_number_size;
void Editor_Draw()
{
    ClearBackground(BLACK);

    BeginMode2D(editor.camera);
    {
        size_t i;
        for (i = 0; i < editor.currentTextFile.n_lines; i++)
        {
            // Draw text
            DrawTextEx(editor.font, editor.currentTextFile.lines[i]->data, (Vector2){0, editor.font_size * i}, editor.font_size, editor.font_spacing, WHITE);
            // Draw line number
            sprintf(str_line_number, "%ld ", i + 1);
            str_line_number_size = MeasureTextEx(editor.font, str_line_number, editor.font_size, editor.font_spacing);
            DrawTextEx(editor.font, str_line_number, (Vector2){-str_line_number_size.x, editor.font_size * i}, editor.font_size, editor.font_spacing, GRAY);
        }
        // Draw line separator numbers
        final_num_pos = editor.font_size * i;
        DrawLineEx((Vector2){-4, 0}, (Vector2){-4, final_num_pos}, 1, GRAY);
        // Draw cursor
        DrawRectangleV(editor.cursor_pos, (Vector2){2, editor.font_size}, WHITE);
    }

    EndMode2D();

    // Draw menu
    Command_Draw();

    // Draw dir
    Directory_Draw("./src");

    DrawFPS(0, 0);
}

void Editor_Close()
{
    TextFile_Free(editor.currentTextFile);
    UnloadFont(editor.font);

    CloseWindow(); // Close window and OpenGL context
}