#include "Command.h"
#include <ctype.h>
#include <stdlib.h>

extern Editor editor;

void Command_InsertChar(void)
{
    DArray_append(&editor.currentCommand, editor.char_pressed);
}

void Command_RemoveChar(void)
{
    DArray_remove(&editor.currentCommand, editor.currentCommand.count - 1);
}

/// @brief strstart( "123456789", "123" ) = true
/// @param str1 str to check
/// @param str2 str start to check
/// @return
bool strstart(char *str1, char *str2)
{
    int i = 0;
    while (str2[i] != '\0')
    {
        DEBUG("i: %d 1: %c 2: %c", i, str1[i], str2[i]);
        if (str1[i] != str2[i])
            return false;
        i++;
    }
    return true;
}

void trimwhitespace(DArray_char *str)
{
    // Trim leading space
    while (str->items[0] == ' ')
        DArray_remove(str, 0);
    // Trim trailing space
    while (str->items[str->count - 1] == ' ')
        DArray_remove(str, str->count - 1);
}

void Command_Consume(void)
{
    if (editor.currentCommand.count == 0)
        return;
    if (editor.currentCommand.items[0] == '>')
    {
        if (strcmp(editor.currentCommand.items, ">save") == 0)
        {
            DEBUG(">save");
            TextFile_Save();
        }
        else if (strcmp(editor.currentCommand.items, ">exit") == 0)
        {
            DEBUG(">exit");
            editor.the_end = true;
        }
        else if (strstart(editor.currentCommand.items, ">dir"))
        {
            DArray_char aux = {0};
            DArray_append_many(&aux, &editor.currentCommand.items[4], editor.currentCommand.count - 4);
            trimwhitespace(&aux);
            DEBUG(">dir Name of dir: %s", aux.items);
            Directory_Load(&aux);
            editor.editor_state = STATE_DIRECTORY;
            DArray_free(&aux);
        }
    }
    else if (editor.currentCommand.items[0] == ':')
    {
        char *endptr;
        long val = strtol(&editor.currentCommand.items[1], &endptr, 10);
        if (*endptr == '\0')
        {
            DEBUG("go to line: %ld", val);
            if (val - 1 < editor.currentTextFile.count && val > 0)
            {
                editor.currentTextFile.cursor.line_num = val - 1;
                editor.currentTextFile.cursor.line = editor.currentTextFile.items[val - 1];
                editor.currentTextFile.cursor.position = 0;
            }
        }
    }

    DArray_clear(&editor.currentCommand);
    if (editor.editor_state == STATE_COMMAND)
        editor.editor_state = STATE_TEXTFILE;
}

void Command_Logic(void)
{
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_P))
        editor.editor_state = STATE_COMMAND;

    if (editor.editor_state != STATE_COMMAND)
        return;

    if (editor.key_pressed == KEY_BACKSPACE)
        Command_RemoveChar();

    if (editor.char_pressed != 0)
        Command_InsertChar();

    if (editor.key_pressed == KEY_ENTER)
    {
        Command_Consume();
    }
}

void Command_Draw(void)
{
    if (editor.editor_state != STATE_COMMAND)
        return;
    int centerX = GetScreenWidth() / 2;
    int centerY = editor.font_size;
    Vector2 command_size = MeasureTextEx(editor.font, editor.currentCommand.items, editor.font_size, editor.font_spacing);
    command_size.y = editor.font_size;
    int width = GetScreenWidth() * 0.66f;
    int height = command_size.y + 5;
    if (width < command_size.x)
        width = command_size.x + 5;
    DrawRectangleCenter(centerX, centerY, width + 5, height + 5, WHITE);
    DrawRectangleCenter(centerX, centerY, width, height, DARKGRAY);
    DrawTextExCenter(editor.font, editor.currentCommand.items, (Vector2){centerX, centerY}, editor.font_size, editor.font_spacing, WHITE);
}

void Command_Free(void)
{
    DArray_free(&editor.currentCommand);
}