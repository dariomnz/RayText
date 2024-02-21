#include "Command.h"

extern Editor editor;

void Command_InsertChar()
{
    size_t str_len = strlen(editor.currentCommand);
    if (str_len < STR_MAX_LEN)
    {
        editor.currentCommand[str_len] = editor.char_pressed;
        editor.currentCommand[str_len + 1] = '\0';
    }
}

void Command_RemoveChar()
{
    size_t str_len = strlen(editor.currentCommand);
    if (str_len != 0)
    {
        editor.currentCommand[str_len - 1] = '\0';
    }
}

void Command_Consume()
{
    size_t str_len = strlen(editor.currentCommand);

    if (strlen(editor.currentCommand) == 0)
        return;
    if (editor.currentCommand[0] == '>')
    {
        if (strcmp(editor.currentCommand, ">save") == 0)
        {
            DEBUG(">save");
            TextFile_Save(editor.currentTextFile);
        }
        if (strcmp(editor.currentCommand, ">exit") == 0)
        {
            DEBUG(">exit");
            editor.the_end = true;
        }
    }
    else if (editor.currentCommand[0] == ':')
    {
        char *endptr;
        long val = strtol(&editor.currentCommand[1], &endptr, 10);
        DEBUG(":%s", &editor.currentCommand[1]);
        if (*endptr == '\0')
        {
            DEBUG("number: %ld", val);
            // Check if is posible go to line
            if (val - 1 < editor.currentTextFile.n_lines && val > 0)
            {
                editor.currentTextFile.cursor.line_num = val - 1;
                editor.currentTextFile.cursor.line = editor.currentTextFile.lines[val - 1];
                editor.currentTextFile.cursor.position = 0;
            }
        }
    }

    editor.currentCommand[0] = '\0';
}

void Command_Draw()
{
    if (!editor.is_commands_showing)
        return;
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