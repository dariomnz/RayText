#include "Command.hpp"
#include <ctype.h>
#include <stdlib.h>
#include <string>

using namespace std;

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

void trimwhitespace(string *str)
{
    // Trim leading space
    while (str->front() == ' ')
        str->erase(0);
    // Trim trailing space
    while (str->back() == ' ')
        str->pop_back();
}

void Command_Consume(Editor *editor)
{
    if (editor->currentCommand.size() == 0)
        return;
    if (editor->currentCommand[0] == '>')
    {
        if (editor->currentCommand.compare(">save") == 0)
        {
            DEBUG(">save");
            TextFile_Save(&editor->currentTextFile);
        }
        else if (editor->currentCommand.compare(">exit") == 0)
        {
            DEBUG(">exit");
            editor->the_end = true;
        }
        else if (editor->currentCommand.compare(">dir"))
        {
            string aux = editor->currentCommand.substr(4);
            trimwhitespace(&aux);
            DEBUG_MSG(">dir Name of dir: %s" << aux);
            Directory_Load(&editor->currentDirectory, aux);
            editor->editor_state = STATE_DIRECTORY;
        }
    }
    else if (editor->currentCommand[0] == ':')
    {
        char *endptr;
        long val = strtol(&editor->currentCommand[1], &endptr, 10);
        if (*endptr == '\0')
        {
            DEBUG("go to line: %ld", val);
            if (val - 1 < (long)editor->currentTextFile.buffer.size() && val > 0)
            {
                editor->currentTextFile.cursor.line_num = val - 1;
                editor->currentTextFile.cursor.position = 0;
            }
        }
    }
    editor->currentCommand.clear();
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
        editor->currentCommand.pop_back();

    if (editor->char_pressed != 0)
        editor->currentCommand += editor->char_pressed;

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
    Vector2 command_size = MeasureTextEx(editor->font, editor->currentCommand.c_str(), FONT_SIZE, FONT_SPACING);
    command_size.y = FONT_SIZE;
    int width = GetScreenWidth() * 0.66f;
    int height = command_size.y + 5;
    if (width < command_size.x)
        width = command_size.x + 5;
    DrawRectangleCenter(centerX, centerY, width + 5, height + 5, WHITE);
    DrawRectangleCenter(centerX, centerY, width, height, DARKGRAY);
    DrawTextExCenter(editor->font, editor->currentCommand.c_str(), (Vector2){(float)centerX, (float)centerY}, FONT_SIZE, FONT_SPACING, WHITE);
}