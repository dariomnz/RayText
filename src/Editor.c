#include "Editor.h"

Editor editor = {0};

void InitializeEditor()
{
    if (editor.initialized != 0)
        return;
    editor.screenWidth = 800;
    editor.screenHeight = 450;
    InitWindow(editor.screenWidth, editor.screenHeight, "RayText");

    editor.font = LoadFont("./fonts/Monaco.ttf");
    SetTextureFilter(editor.font.texture, TEXTURE_FILTER_TRILINEAR);
    editor.font_size = 20;
    editor.font_spacing = 0;
    editor.currentCommand[0] = '\0';
    editor.is_commands_showing = false;
    editor.currentTextFile = LoadEmptyTextFile();

    editor.camera.zoom = 1.0f;
    editor.cursor_pos = Vector2Zero();
}

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
    Vector2 cursor_vec_pos = MeasureTextEx(editor.font, textFile.cursor.line->data, editor.font_size, editor.font_spacing);
    textFile.cursor.line->data[cursor_pos] = cursor_char;
    cursor_vec_pos.y = editor.font_size * textFile.cursor.line_num;
    return cursor_vec_pos;
}

void InsertCharCommand(char c)
{
    size_t str_len = strlen(editor.currentCommand);
    if (str_len < STR_MAX_LEN)
    {
        editor.currentCommand[str_len] = c;
        editor.currentCommand[str_len + 1] = '\0';
    }
}

void RemoveCharCommand()
{
    size_t str_len = strlen(editor.currentCommand);
    if (str_len != 0)
    {
        editor.currentCommand[str_len-1] = '\0';
    }
}

void ConsumeCommand()
{
    size_t str_len = strlen(editor.currentCommand);

    if (strlen(editor.currentCommand) == 0)
        return;
    if (editor.currentCommand[0] == '>')
    {
        if (strcmp(editor.currentCommand, ">save") == 0)
        {
            DEBUG(">save");
            SaveTextFile(editor.currentTextFile);
        }
    }

    editor.currentCommand[0] = '\0';
}

void LogicEditor()
{
    int key_press = 0;
    int char_pressed = 0;
    if (IsWindowResized())
    {
        editor.screenWidth = GetScreenWidth();
        editor.screenHeight = GetScreenHeight();
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
    // for (int i = 0; i < 6; i++)
    FOR_EACH(int, key, keys_to_check)
    {
        if (IsKeyPressed(*key) || IsKeyPressedRepeat(*key))
            key_press = *key;
    }

    MoveCursor(&editor.currentTextFile, key_press);
    if (key_press == KEY_BACKSPACE)

        if (editor.is_commands_showing)
            RemoveCharCommand();
        else
            RemoveChar(&editor.currentTextFile);

    char_pressed = GetCharPressed();
    // DEBUG("char: %d", char_pressed);
    if (char_pressed != 0)
    {
        if (editor.is_commands_showing)
            InsertCharCommand((char)char_pressed);
        else
            InsertChar(&editor.currentTextFile, (char)char_pressed);
    }
    if (key_press == KEY_ENTER)
        if (editor.is_commands_showing)
        {
            ConsumeCommand();
            editor.is_commands_showing = false;
        }
        else
            InsertNewLine(&editor.currentTextFile);

    editor.cursor_pos = GetCursorPosition(editor.currentTextFile);

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_P))
    {
        editor.is_commands_showing = !editor.is_commands_showing;
        // DEBUG("is_commands_showing %d", is_commands_showing);
    }

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S))
    {
        SaveTextFile(editor.currentTextFile);
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

        editor.camera.zoom += (wheel * zoomIncrement);
        if (editor.camera.zoom < zoomIncrement)
            editor.camera.zoom = zoomIncrement;
    }

    editor.camera.offset = (Vector2){editor.screenWidth / 2, editor.screenHeight / 2};
    if (Vector2EqualsEpsilon(editor.cursor_pos, editor.camera.target, 0.1f) == 0)
        editor.camera.target = Vector2Lerp(editor.camera.target, editor.cursor_pos, 0.2f);
}

void DrawCommandBox()
{
    int centerX = GetScreenWidth() / 2;
    int centerY = editor.font_size;
    Vector2 command_size = MeasureTextEx(editor.font, editor.currentCommand, editor.font_size, editor.font_spacing);
    int width = GetScreenWidth() * 0.66f;
    int height = command_size.y + 5;
    if (width < command_size.x)
        width = command_size.x + 5;
    DrawRectangleCenter(centerX, centerY, width + 5, height + 5, WHITE);
    DrawRectangleCenter(centerX, centerY, width, height, GRAY);
    DrawTextExCenter(editor.font, editor.currentCommand, (Vector2){centerX, centerY}, editor.font_size, editor.font_spacing, WHITE);
}

void DrawDir(char *dir_name)
{
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
            DrawTextEx(editor.font, array_file[a], (Vector2){0, editor.font_size * a}, editor.font_size, editor.font_spacing, color);
        }
    }
    // for (int a = 0; a < i_file; a++)
    // {
    //     DrawTextEx(font, array_file[a], (Vector2){0, font_size * (a + i_dir)}, font_size, font_spacing, WHITE);
    // }
}
void DrawEditor()
{
    DrawFPS(0, 0);
    BeginMode2D(editor.camera);
    int final_num_pos = 0;
    char str_line_number[10];
    Vector2 str_line_number_size;
    for (size_t i = 0; i < editor.currentTextFile.n_lines; i++)
    {
        // Draw text
        DrawTextEx(editor.font, editor.currentTextFile.lines[i]->data, (Vector2){0, editor.font_size * i}, editor.font_size, editor.font_spacing, WHITE);
        // Draw line number
        sprintf(str_line_number, "%ld ", i + 1);
        str_line_number_size = MeasureTextEx(editor.font, str_line_number, editor.font_size, editor.font_spacing);
        DrawTextEx(editor.font, str_line_number, (Vector2){-str_line_number_size.x, editor.font_size * i}, editor.font_size, editor.font_spacing, GRAY);
        final_num_pos = editor.font_size * (i + 1);
    }
    // Draw line separator numbers
    DrawLineEx((Vector2){-4, 0}, (Vector2){-4, final_num_pos}, 1, GRAY);
    // Draw cursor
    DrawRectangleV(editor.cursor_pos, (Vector2){2, editor.font_size}, WHITE);

    EndMode2D();

    // Draw menu
    if (editor.is_commands_showing)
    {
        DrawCommandBox();
    }

    // Draw dir
    DrawDir(".");
}