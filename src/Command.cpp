#include "Command.hpp"
#include "Directories.hpp"
#include "TextFile.hpp"
#include "App.hpp"
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

void trimwhitespace(string &str)
{
    // Trim leading space
    while (str.front() == ' ')
        str.erase(0, 1);
    // Trim trailing space
    while (str.back() == ' ')
        str.pop_back();
}

Command::Command()
{
    DEBUG_MSG("Construct Command");
}

Command::~Command()
{
    DEBUG_MSG("delete Command");
}

void Command::Consume(App &app)
{
    if (command.size() == 0)
        return;
    if (command[0] == '>')
    {
        if (command.compare(">save") == 0)
        {
            DEBUG(">save");
            app.editor.GetCurrentTextFile().Save();
        }
        else if (command.compare(">exit") == 0)
        {
            DEBUG(">exit");
            app.the_end = true;
        }
        else if (command.compare(">dir") == 0)
        {
            string aux = command.substr(4);
            trimwhitespace(aux);
            DEBUG_MSG(">dir Name of dir:" << aux);
            app.directories.Load(aux);
            app.editor.editor_state = STATE_DIRECTORY;
        }
    }
    else if (command[0] == ':')
    {
        char *endptr;
        long val = strtol(&command[1], &endptr, 10);
        if (*endptr == '\0')
        {
            DEBUG("go to line: %ld", val);
            if (val - 1 < (long)app.editor.GetCurrentTextFile().buffer.size() && val > 0)
            {
                app.editor.GetCurrentTextFile().cursor.line_num = val - 1;
                app.editor.GetCurrentTextFile().cursor.position = 0;
            }
        }
    }
    command.clear();
    if (app.editor.editor_state == STATE_COMMAND)
        app.editor.editor_state = STATE_TEXTFILE;
}

void Command::Logic(App &app)
{
    if (app.editor.editor_state != STATE_COMMAND)
    {
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_P))
        {
            if (IsKeyDown(KEY_LEFT_SHIFT))
                command += ">";
            app.editor.editor_state = STATE_COMMAND;
        }
        return;
    }

    if (app.editor.key_pressed == KEY_BACKSPACE)
        command.pop_back();

    if (app.editor.char_pressed != 0)
        command += app.editor.char_pressed;

    if (app.editor.key_pressed == KEY_ENTER)
    {
        Consume(app);
    }
}

void Command::Draw(App &app)
{
    if (app.editor.editor_state != STATE_COMMAND)
        return;
    int centerX = GetScreenWidth() / 2;
    int centerY = FONT_SIZE;
    Vector2 command_size = MeasureTextEx(app.editor.font, command.c_str(), FONT_SIZE, FONT_SPACING);
    command_size.y = FONT_SIZE;
    int width = GetScreenWidth() * 0.66f;
    int height = command_size.y + 5;
    if (width < command_size.x)
        width = command_size.x + 5;
    DrawRectangleCenter(centerX, centerY, width + 5, height + 5, WHITE);
    DrawRectangleCenter(centerX, centerY, width, height, DARKGRAY);
    DrawTextExCenter(app.editor.font, command.c_str(), (Vector2){(float)centerX, (float)centerY}, FONT_SIZE, FONT_SPACING, WHITE);
}