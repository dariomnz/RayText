#include "Command.h"

extern Editor editor;

void Command_InsertChar()
{
    size_t str_len = strlen(editor.currentCommand);
    if (str_len < MAX_COMMAND_LEN)
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

char *trimwhitespace(char *str)
{
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str))
        str++;

    if (*str == 0) // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
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
        else if (strcmp(editor.currentCommand, ">exit") == 0)
        {
            DEBUG(">exit");
            editor.the_end = true;
        }
        else if (strstart(editor.currentCommand, ">dir"))
        {
            char aux_str[MAX_COMMAND_LEN];
            strncpy(aux_str, &editor.currentCommand[4], MAX_COMMAND_LEN);
            DEBUG(">dir Name of dir: %s", aux_str);
            Directory_Load(trimwhitespace(aux_str));
            editor.editor_state = STATE_DIRECTORY;
        }
    }
    else if (editor.currentCommand[0] == ':')
    {
        char *endptr;
        long val = strtol(&editor.currentCommand[1], &endptr, 10);
        if (*endptr == '\0')
        {
            DEBUG("go to line: %ld", val);
            if (val - 1 < editor.currentTextFile.n_lines && val > 0)
            {
                editor.currentTextFile.cursor.line_num = val - 1;
                editor.currentTextFile.cursor.line = editor.currentTextFile.lines[val - 1];
                editor.currentTextFile.cursor.position = 0;
            }
        }
    }

    editor.currentCommand[0] = '\0';
    if (editor.editor_state == STATE_COMMAND)
        editor.editor_state = STATE_TEXTFILE;
}

void Command_Logic()
{
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_P))
        editor.editor_state = STATE_COMMAND;

    if (editor.editor_state != STATE_COMMAND)
        return;

    if (editor.key_pressed == KEY_BACKSPACE)
        Command_RemoveChar();

    if (editor.char_pressed != 0)
        Command_InsertChar(editor.char_pressed);

    if (editor.key_pressed == KEY_ENTER)
    {
        Command_Consume();
    }
}

void Command_Draw()
{
    if (editor.editor_state != STATE_COMMAND)
        return;
    int centerX = GetScreenWidth() / 2;
    int centerY = editor.font_size;
    Vector2 command_size = MeasureTextEx(editor.font, editor.currentCommand, editor.font_size, editor.font_spacing);
    int width = GetScreenWidth() * 0.66f;
    int height = command_size.y + 5;
    if (width < command_size.x)
        width = command_size.x + 5;
    DrawRectangleCenter(centerX, centerY, width + 5, height + 5, WHITE);
    DrawRectangleCenter(centerX, centerY, width, height, DARKGRAY);
    DrawTextExCenter(editor.font, editor.currentCommand, (Vector2){centerX, centerY}, editor.font_size, editor.font_spacing, WHITE);
}