#include "Command.hpp"
#include <ctype.h>
#include <stdlib.h>

void Command_InsertChar(DArray_char *command, char c)
{
    DArray_append(command, c);
}

void Command_RemoveChar(DArray_char *command)
{
    DArray_remove(command, command->count - 1);
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
        // DEBUG("i: %d 1: %c 2: %c", i, str1[i], str2[i]);
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

void Command_Consume(Editor *editor)
{
    if (editor->currentCommand.count == 0)
        return;
    if (editor->currentCommand.items[0] == '>')
    {
        if (strcmp(editor->currentCommand.items, ">save") == 0)
        {
            DEBUG(">save");
            TextFile_Save(&editor->currentTextFile);
        }
        else if (strcmp(editor->currentCommand.items, ">exit") == 0)
        {
            DEBUG(">exit");
            editor->the_end = true;
        }
        else if (strstart(editor->currentCommand.items, ">dir"))
        {
            Directory aux = {0};
            DArray_append_many(&aux, &editor->currentCommand.items[4], editor->currentCommand.count - 4);
            trimwhitespace((DArray_char *)&aux);
            DEBUG(">dir Name of dir: %s", aux.items);
            Directory_Load(&editor->currentDirectory, &aux);
            editor->editor_state = STATE_DIRECTORY;
            DArray_free(&aux);
        }
    }
    else if (editor->currentCommand.items[0] == ':')
    {
        char *endptr;
        long val = strtol(&editor->currentCommand.items[1], &endptr, 10);
        if (*endptr == '\0')
        {
            DEBUG("go to line: %ld", val);
            if (val - 1 < editor->currentTextFile.count && val > 0)
            {
                editor->currentTextFile.cursor.line_num = val - 1;
                editor->currentTextFile.cursor.position = 0;
            }
        }
    }

    DArray_clear(&editor->currentCommand);
    if (editor->editor_state == STATE_COMMAND)
        editor->editor_state = STATE_TEXTFILE;
}

void Command_Logic(Editor *editor)
{
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_P))
        editor->editor_state = STATE_COMMAND;

    if (editor->editor_state != STATE_COMMAND)
        return;

    if (editor->key_pressed == KEY_BACKSPACE)
        Command_RemoveChar(&editor->currentCommand);

    if (editor->char_pressed != 0)
        Command_InsertChar(&editor->currentCommand, editor->char_pressed);

    if (editor->key_pressed == KEY_ENTER)
    {
        Command_Consume(editor);
    }
}

void Command_Draw(Editor *editor)
{
    if (editor->editor_state != STATE_COMMAND)
        return;
    int centerX = GetScreenWidth() / 2;
    int centerY = FONT_SIZE;
    Vector2 command_size = MeasureTextEx(editor->font, editor->currentCommand.items, FONT_SIZE, FONT_SPACING);
    command_size.y = FONT_SIZE;
    int width = GetScreenWidth() * 0.66f;
    int height = command_size.y + 5;
    if (width < command_size.x)
        width = command_size.x + 5;
    DrawRectangleCenter(centerX, centerY, width + 5, height + 5, WHITE);
    DrawRectangleCenter(centerX, centerY, width, height, DARKGRAY);
    DrawTextExCenter(editor->font, editor->currentCommand.items, (Vector2){(float)centerX, (float)centerY}, FONT_SIZE, FONT_SPACING, WHITE);
}

void Command_Free(DArray_char *command)
{
    DArray_free(command);
}