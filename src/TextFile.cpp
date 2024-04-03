#include "TextFile.hpp"
#include "Cursor.hpp"
#include <string>

using namespace std;

TextFile TextFile_Load(string file_name)
{
    TextFile textFile = {};
    textFile.name = file_name;
    DEBUG("Opening file: %s", file_name.c_str());
    char *data = LoadFileText(file_name.c_str());
    size_t last_line = 0;
    string new_line;
    size_t i = 0;
    for (i = 0; i < strlen(data); i++)
    {
        if (data[i] == '\n')
        {
            new_line.clear();
            new_line.append(&data[last_line], i - last_line);
            textFile.buffer.push_back(new_line);
            last_line = i + 1;
        }
    }
    new_line.clear();
    new_line.append(&data[last_line], i - last_line);
    textFile.buffer.push_back(new_line);
    UnloadFileText(data);

    textFile.cursor.position = 0;
    textFile.cursor.line_num = 0;

    textFile.cursor_start_select.line_num = -1;

    return textFile;
}

TextFile TextFile_LoadEmpty(void)
{
    TextFile textFile = {};
    textFile.name = "empty.txt";

    string new_line;
    textFile.buffer.push_back(new_line);

    // Init Cursor
    textFile.cursor.position = 0;
    textFile.cursor.line_num = 0;

    textFile.cursor_start_select.line_num = -1;

    return textFile;
}

string TextFile_to_string(TextFile *textFile)
{
    string data;
    for (size_t i = 0; i < textFile->buffer.size(); i++)
    {
        data += textFile->buffer[i];
        if (i != textFile->buffer.size() - 1)
            data += '\n';
    }
    return data;
}

void TextFile_Save(TextFile *textFile)
{
    DEBUG_MSG("Saving: " << textFile->name);
    SaveFileText(textFile->name.c_str(), TextFile_to_string(textFile).data());
}

void TextFile_Free(TextFile *textFile)
{
    textFile->buffer.clear();
    textFile->cursor_select.clear();
    textFile->rect_select.clear();
}

void TextFile_Print(TextFile *textFile)
{
    printf("--------File \"%s\"--------\n", textFile->name.c_str());

    string data = TextFile_to_string(textFile);
    printf("%s\n", data.c_str());
    printf("------End file \"%s\"------\n", textFile->name.c_str());
}

void TextFile_InsertChar(TextFile *textFile, char c)
{
    Cursor_GetLine(textFile, &textFile->cursor).insert(textFile->cursor.position, 1, c);
    textFile->cursor.position++;
}

void TextFile_InsertStr(TextFile *textFile, const char *str)
{
    size_t str_size = strlen(str);
    size_t start = 0;
    size_t size = 0;
    size_t i;
    for (i = 0; i < str_size; i++)
    {
        if (str[i] == '\n')
        {
            size = i - start - 1;
            Cursor_GetLine(textFile, &textFile->cursor).insert(textFile->cursor.position, &str[start], size);
            textFile->cursor.position += size;
            TextFile_InsertNewLine(textFile);
            start = i + 1;
        }
    }
    size = i - start;
    Cursor_GetLine(textFile, &textFile->cursor).insert(textFile->cursor.position, &str[start], size);
    textFile->cursor.position += size;
}

void TextFile_InsertNewLine(TextFile *textFile)
{
    size_t new_line_pos = textFile->cursor.line_num + 1;
    size_t new_line_chars = Cursor_GetLine(textFile, &textFile->cursor).size() - textFile->cursor.position;

    // Build new line
    string new_line;

    new_line.insert(0, Cursor_GetLine(textFile, &textFile->cursor), textFile->cursor.position, new_line_chars);
    textFile->buffer.insert(textFile->buffer.begin() + new_line_pos, new_line);

    // Add return of line
    if (textFile->cursor.position != Cursor_GetLine(textFile, &textFile->cursor).size())
    {
        string &line = Cursor_GetLine(textFile, &textFile->cursor);
        line = line.substr(0, textFile->cursor.position);
    }

    // Update cursor
    textFile->cursor.line_num++;
    textFile->cursor.position = 0;
}

void TextFile_RemoveLine_Left(TextFile *textFile)
{
    if (textFile->cursor.line_num == 0)
        return;

    size_t position_pre_line = textFile->buffer[textFile->cursor.line_num - 1].size();

    textFile->buffer[textFile->cursor.line_num - 1] += Cursor_GetLine(textFile, &textFile->cursor);
    textFile->buffer.erase(textFile->buffer.begin() + textFile->cursor.line_num);

    // Update cursor
    textFile->cursor.line_num--;
    textFile->cursor.position = position_pre_line;
}

void TextFile_RemoveLine_Right(TextFile *textFile)
{
    if (textFile->cursor.line_num + 1 == (int)textFile->buffer.size())
        return;

    Cursor_GetLine(textFile, &textFile->cursor) += textFile->buffer[textFile->cursor.line_num + 1];
    textFile->buffer.erase(textFile->buffer.begin() + textFile->cursor.line_num + 1);
}

void TextFile_RemoveChar_Left(TextFile *textFile)
{
    size_t cursor_pos = textFile->cursor.position;
    if (cursor_pos == 0)
    {
        TextFile_RemoveLine_Left(textFile);
        return;
    }
    string &line = Cursor_GetLine(textFile, &textFile->cursor);
    line.erase(line.begin() + cursor_pos - 1);

    // Update cursor
    textFile->cursor.position--;
}

void TextFile_RemoveChar_Right(TextFile *textFile)
{
    size_t cursor_pos = textFile->cursor.position;
    if (cursor_pos == Cursor_GetLine(textFile, &textFile->cursor).size())
    {
        TextFile_RemoveLine_Right(textFile);
        return;
    }

    string &line = Cursor_GetLine(textFile, &textFile->cursor);
    line.erase(line.begin() + cursor_pos);
}

void TextFile_Logic(Editor *editor)
{
    if (editor->editor_state != STATE_TEXTFILE)
        return;

    if (editor->key_pressed == KEY_BACKSPACE)
        TextFile_RemoveChar_Left(&editor->currentTextFile);

    if (editor->key_pressed == KEY_DELETE)
        TextFile_RemoveChar_Right(&editor->currentTextFile);

    if (editor->char_pressed != 0)
        TextFile_InsertChar(&editor->currentTextFile, editor->char_pressed);

    if (editor->key_pressed == KEY_TAB)
    {
        for (int i = 0; i < 4; i++)
            TextFile_InsertChar(&editor->currentTextFile, ' ');
    }

    if (editor->key_pressed == KEY_ENTER)
        TextFile_InsertNewLine(&editor->currentTextFile);

    if (IsKeyDown(KEY_LEFT_CONTROL))
    {
        if (IsKeyPressed(KEY_S))
        {
            TextFile_Save(&editor->currentTextFile);
        }

        if (IsKeyPressed(KEY_C))
        {
            SetClipboardText(editor->currentTextFile.cursor_select.c_str());
        }

        if (IsKeyPressed(KEY_V))
        {
            TextFile_InsertStr(&editor->currentTextFile, GetClipboardText());
            editor->currentTextFile.cursor_select.clear();
            editor->currentTextFile.rect_select.clear();
        }
    }
}

void TextFile_Draw(Editor *editor)
{
    if (editor->editor_state != STATE_TEXTFILE && editor->editor_state != STATE_COMMAND)
        return;

    int plus_minus_lines = (editor->screenHeight / editor->camera.zoom) / FONT_SIZE / 2 * 1.5f;
    int current_line = editor->currentTextFile.cursor.line_num;
    int i;
    char str_line_number[10];
    Vector2 str_line_number_size;
    for (i = current_line - plus_minus_lines; i < current_line + plus_minus_lines; i++)
    {
        if (i < 0)
            continue;
        if (i >= (int)editor->currentTextFile.buffer.size())
            break;
        // Draw text
        DrawTextEx(editor->font, editor->currentTextFile.buffer[i].c_str(), (Vector2){0, (float)FONT_SIZE * i}, FONT_SIZE, FONT_SPACING, WHITE);
        // Draw line number
        sprintf(str_line_number, "%d ", i + 1);
        str_line_number_size = MeasureTextEx(editor->font, str_line_number, FONT_SIZE, FONT_SPACING);
        DrawTextEx(editor->font, str_line_number, (Vector2){-str_line_number_size.x, (float)FONT_SIZE * i}, FONT_SIZE, FONT_SPACING, GRAY);
    }
    // Draw line separator numbers
    int final_num_pos = FONT_SIZE * i;
    DrawLineEx((Vector2){-4, 0}, (Vector2){-4, (float)final_num_pos}, 1, GRAY);
}