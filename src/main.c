#include "raylib.h"

#include "rlgl.h"
#include "raymath.h"

#include "Common.h"
#include "TextFile.h"
#include "Editor.h"

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

// Global variables
extern Editor editor;
TextFile currentTextFile;
char currentCommand[STR_MAX_LEN];
int font_size = 20;
int font_spacing = 0;
Font font;

int keys_to_check[6] = {KEY_RIGHT,
                        KEY_LEFT,
                        KEY_UP,
                        KEY_DOWN,
                        KEY_BACKSPACE,
                        KEY_ENTER};

Vector2 GetCursorPosition(TextFile textFile)
{
    size_t cursor_pos = textFile.cursor.position;
    char cursor_char = textFile.cursor.line->data[cursor_pos];
    textFile.cursor.line->data[cursor_pos] = '\0';
    Vector2 cursor_vec_pos = MeasureTextEx(font, textFile.cursor.line->data, font_size, font_spacing);
    textFile.cursor.line->data[cursor_pos] = cursor_char;
    cursor_vec_pos.y = font_size * textFile.cursor.line_num;
    return cursor_vec_pos;
}

void InsertCharCommand(char c)
{
    size_t str_len = strlen(currentCommand);
    if (str_len < STR_MAX_LEN)
    {
        currentCommand[str_len] = c;
        currentCommand[str_len + 1] = '\0';
    }
}

int Vector2EqualsEpsilon(Vector2 p, Vector2 q, float epsilon)
{
    int result = ((fabsf(p.x - q.x)) <= (epsilon)) &&
                 ((fabsf(p.y - q.y)) <= (epsilon));

    return result;
}

void DrawRectangleCenter(int posX, int posY, int width, int height, Color color)
{
    posX -= width / 2;
    posY -= height / 2;
    DrawRectangle(posX, posY, width, height, color);
}

void DrawTextExCenter(Font font, const char *text, Vector2 position, float fontSize, float spacing, Color tint)
{
    Vector2 text_size = MeasureTextEx(font, text, fontSize, spacing);
    text_size = Vector2Scale(text_size, 0.5f);
    position = Vector2Subtract(position, text_size);
    DrawTextEx(font, text, position, fontSize, spacing, tint);
}

void DrawCommandBox()
{
    int centerX = GetScreenWidth() / 2;
    int centerY = font_size;
    Vector2 command_size = MeasureTextEx(font, currentCommand, font_size, font_spacing);
    int width = GetScreenWidth() * 0.66f;
    int height = command_size.y + 5;
    if (width < command_size.x)
        width = command_size.x + 5;
    DrawRectangleCenter(centerX, centerY, width + 5, height + 5, WHITE);
    DrawRectangleCenter(centerX, centerY, width, height, GRAY);
    DrawTextExCenter(font, currentCommand, (Vector2){centerX, centerY}, font_size, font_spacing, WHITE);
}

void ConsumeCommand()
{
    size_t str_len = strlen(currentCommand);

    if (strlen(currentCommand) == 0)
        return;
    if (currentCommand[0] == '>')
    {
        if (strcmp(currentCommand, ">save") == 0)
        {
            SaveTextFile(currentTextFile);
            DEBUG(">save");
        }
    }

    currentCommand[0] = '\0';
}

void DrawDir(char *dir_name)
{
    // DIR *dir;
    // struct dirent *ent;

    // // Abre el directorio actual
    // dir = opendir(dir_name);
    // int index = 0;

    // if (dir != NULL)
    // {
    //     // Lee todos los archivos y subdirectorios del directorio
    //     while ((ent = readdir(dir)) != NULL)
    //     {
    //         DrawTextEx(font, ent->d_name, (Vector2){0, font_size * index}, font_size, font_spacing, WHITE);
    //         // printf("%s\n", ent->d_name);
    //         index++;
    //     }
    //     closedir(dir);
    // }
    // else
    // {
    //     // Si no se puede abrir el directorio, imprime un mensaje de error
    //     DEBUG("ERROR: Cannot open dir %s", dir_name);
    //     return;
    // }
    char array_file[50][256] = {};
    unsigned char array_file_type[50] = {};
    char tmp[256] = {};
    unsigned char tmp_type;
    int i_file = 0, j = 0, k = 0;
    DIR *d;
    struct dirent *dir;
    d = opendir(dir_name);
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            // if (dir->d_type == DT_REG)
            // {
            strcpy(array_file[i_file], dir->d_name);
            array_file_type[i_file] = dir->d_type;
            // DEBUG("%s DT: %u %s DT: %u", array_file[i_file], array_file_type[i_file], dir->d_name, dir->d_type);
            i_file++;
            // }
            // if (dir->d_type == DT_DIR)
            // {
            //     strcpy(array_dir[i_dir], dir->d_name);
            //     i_dir++;
            // }
        }
        for (k = 0; k <= i_file; k++) /* Sorting files alphabetically */
            for (j = k + 1; j <= i_file; j++)
            {
                if (strcasecmp(array_file[k], array_file[j]) > 0)
                {
                    strcpy(tmp, array_file[k]);
                    strcpy(array_file[k], array_file[j]);
                    strcpy(array_file[j], tmp);
                    tmp_type = array_file_type[k];
                    array_file_type[k] = array_file_type[j];
                    array_file_type[j] = tmp_type;
                }
            }
        // for (k = 0; k <= i_dir; k++) /* Sorting files alphabetically */
        //     for (j = k + 1; j <= i_dir; j++)
        //     {
        //         if (strcasecmp(array_dir[k], array_dir[j]) > 0)
        //         {
        //             strcpy(tmp, array_dir[k]);
        //             strcpy(array_dir[k], array_dir[j]);
        //             strcpy(array_dir[j], tmp);
        //         }
        //     }
        closedir(d);
        Color color = RED;
        for (int a = 0; a < i_file; a++)
        {
            // DEBUG("%s DT: %u", array_file[a], array_file_type[a]);
            if (array_file_type[a] == DT_REG)
                color = WHITE;
            else if (array_file_type[a] == DT_DIR)
                color = BLUE;
            DrawTextEx(font, array_file[a], (Vector2){0, font_size * a}, font_size, font_spacing, color);
        }
    }
    // for (int a = 0; a < i_file; a++)
    // {
    //     DrawTextEx(font, array_file[a], (Vector2){0, font_size * (a + i_dir)}, font_size, font_spacing, WHITE);
    // }
}
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    int screenWidth = 800;
    int screenHeight = 450;
    int char_pressed = 0;
    Vector2 cursor_pos = {0};
    Vector2 cursor_pos_aux = {0};
    char str_line_number[10];
    Vector2 str_line_number_size;
    int key_press = 0;
    bool is_commands_showing = false;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 2d camera mouse zoom");

    InitializeEditor();
    font = LoadFont("./fonts/Monaco.ttf");
    SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);
    Camera2D camera = {0};
    camera.zoom = 1.0f;
    // printf("Prueba debug\n");
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    currentTextFile = LoadTextFile("./input.txt");
    // currentTextFile = LoadTextFile("./main.c");
    // PrintTextFile(currentTextFile);
    DEBUG("editor size %d", editor.font_size);

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        if (IsWindowResized())
        {
            screenWidth = GetScreenWidth();
            screenHeight = GetScreenHeight();
        }
        // Update
        //----------------------------------------------------------------------------------
        // Translate based on mouse right click
        // if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        // {
        //     Vector2 delta = GetMouseDelta();
        //     delta = Vector2Scale(delta, -1.0f / camera.zoom);

        //     camera.target = Vector2Add(camera.target, delta);
        // }

        // key_press = GetKeyPressed();
        // if (key_press != 0)
        // {
        // }
        key_press = 0;
        // for (int i = 0; i < 6; i++)
        FOR_EACH(int, key, keys_to_check)
        {
            if (IsKeyPressed(*key) || IsKeyPressedRepeat(*key))
                key_press = *key;
        }

        MoveCursor(&currentTextFile, key_press);
        if (key_press == KEY_BACKSPACE)
            RemoveChar(&currentTextFile);

        char_pressed = GetCharPressed();
        // DEBUG("char: %d", char_pressed);
        if (char_pressed != 0)
        {
            if (is_commands_showing)
                InsertCharCommand((char)char_pressed);
            else
                InsertChar(&currentTextFile, (char)char_pressed);
        }
        if (key_press == KEY_ENTER)
            if (is_commands_showing)
            {
                ConsumeCommand();
                is_commands_showing = false;
            }
            else
                InsertNewLine(&currentTextFile);

        cursor_pos = GetCursorPosition(currentTextFile);

        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_P))
        {
            is_commands_showing = !is_commands_showing;
            // DEBUG("is_commands_showing %d", is_commands_showing);
        }

        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S))
        {
            SaveTextFile(currentTextFile);
        }
        // DEBUG("Command");
        // if (Vector2Equals(cursor_pos, cursor_pos_aux) == 0)
        // {
        // DEBUG("camera.target %f:%f cursor_pos %f:%f", camera.target.x, camera.target.y, cursor_pos.x, cursor_pos.y);
        //     cursor_pos_aux = cursor_pos;
        // }

        // Zoom based on mouse wheel
        float wheel = GetMouseWheelMove();
        if (wheel != 0)
        {
            // Get the world point that is under the mouse
            // Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

            // Set the offset to where the mouse is
            // camera.offset = GetMousePosition();

            // Set the target to match, so that the camera maps the world space point
            // under the cursor to the screen space point under the cursor at any zoom
            // camera.target = mouseWorldPos;

            // Zoom increment
            const float zoomIncrement = 0.125f;

            camera.zoom += (wheel * zoomIncrement);
            if (camera.zoom < zoomIncrement)
                camera.zoom = zoomIncrement;
        }

        camera.offset = (Vector2){screenWidth / 2, screenHeight / 2};
        if (Vector2EqualsEpsilon(cursor_pos, camera.target, 0.1f) == 0)
            camera.target = Vector2Lerp(camera.target, cursor_pos, 0.2f);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);

        DrawFPS(0, 0);
        BeginMode2D(camera);
        int final_num_pos = 0;
        for (size_t i = 0; i < currentTextFile.n_lines; i++)
        {
            // Draw text
            DrawTextEx(font, currentTextFile.lines[i]->data, (Vector2){0, font_size * i}, font_size, font_spacing, WHITE);
            // Draw line number
            sprintf(str_line_number, "%ld ", i + 1);
            str_line_number_size = MeasureTextEx(font, str_line_number, font_size, font_spacing);
            DrawTextEx(font, str_line_number, (Vector2){-str_line_number_size.x, font_size * i}, font_size, font_spacing, GRAY);
            final_num_pos = font_size * (i + 1);
        }
        // Draw line separator numbers
        DrawLineEx((Vector2){-4, 0}, (Vector2){-4, final_num_pos}, 1, GRAY);
        // Draw cursor
        DrawRectangleV(cursor_pos, (Vector2){2, font_size}, WHITE);

        EndMode2D();

        // Draw menu
        if (is_commands_showing)
        {
            DrawCommandBox();
        }

        // Draw dir
        DrawDir(".");

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    FreeTextFile(currentTextFile);
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}