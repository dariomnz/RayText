#include "TextFile.hpp"
#include "Cursor.hpp"
#include <string>

using namespace std;

void TextFile::Load()
{
    name = "empty.txt";

    string new_line;
    buffer.push_back(new_line);

    // Init Cursor
    cursor.position = 0;
    cursor.line_num = 0;

    cursor_start_select.line_num = -1;
}

void TextFile::Load(std::string file_name)
{
    name = file_name;
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
            buffer.push_back(new_line);
            last_line = i + 1;
        }
    }
    new_line.clear();
    new_line.append(&data[last_line], i - last_line);
    buffer.push_back(new_line);
    UnloadFileText(data);

    cursor.position = 0;
    cursor.line_num = 0;

    cursor_start_select.line_num = -1;
}

TextFile::TextFile() : cursor(*this), cursor_start_select(*this)
{
    DEBUG_MSG("Contructor TextFile");
    Load();
}

void TextFile::Free()
{
    buffer.clear();
    cursor_select.clear();
    rect_select.clear();
}

TextFile::~TextFile()
{
    DEBUG_MSG("delete TextFile " << name);
}

string TextFile::ToString()
{
    string data;
    for (size_t i = 0; i < buffer.size(); i++)
    {
        data += buffer[i];
        if (i != buffer.size() - 1)
            data += '\n';
    }
    return data;
}

void TextFile::Save()
{
    DEBUG_MSG("Saving: " << name);
    SaveFileText(name.c_str(), ToString().data());
}

void TextFile::Print()
{
    printf("--------File \"%s\"--------\n", name.c_str());

    string data = ToString();
    printf("%s\n", data.c_str());
    printf("------End file \"%s\"------\n", name.c_str());
}

void TextFile::InsertChar(char c)
{
    cursor.GetLine().insert(cursor.position, 1, c);
    cursor.position++;
}

void TextFile::InsertStr(const char *str)
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
            cursor.GetLine().insert(cursor.position, &str[start], size);
            cursor.position += size;
            InsertNewLine();
            start = i + 1;
        }
    }
    size = i - start;
    cursor.GetLine().insert(cursor.position, &str[start], size);
    cursor.position += size;
}

void TextFile::InsertNewLine()
{
    size_t new_line_pos = cursor.line_num + 1;
    size_t new_line_chars = cursor.GetLine().size() - cursor.position;

    // Build new line
    string new_line;

    new_line.insert(0, cursor.GetLine(), cursor.position, new_line_chars);
    buffer.insert(buffer.begin() + new_line_pos, new_line);

    // Add return of line
    if (cursor.position != cursor.GetLine().size())
    {
        string &line = cursor.GetLine();
        line = line.substr(0, cursor.position);
    }

    // Update cursor
    cursor.line_num++;
    cursor.position = 0;
}

void TextFile::RemoveLine_Left()
{
    if (cursor.line_num == 0)
        return;

    size_t position_pre_line = buffer[cursor.line_num - 1].size();

    buffer[cursor.line_num - 1] += cursor.GetLine();
    buffer.erase(buffer.begin() + cursor.line_num);

    // Update cursor
    cursor.line_num--;
    cursor.position = position_pre_line;
}

void TextFile::RemoveLine_Right()
{
    if (cursor.line_num + 1 == (int)buffer.size())
        return;

    cursor.GetLine() += buffer[cursor.line_num + 1];
    buffer.erase(buffer.begin() + cursor.line_num + 1);
}

void TextFile::RemoveChar_Left()
{
    size_t cursor_pos = cursor.position;
    if (cursor_pos == 0)
    {
        RemoveLine_Left();
        return;
    }
    string &line = cursor.GetLine();
    line.erase(line.begin() + cursor_pos - 1);

    // Update cursor
    cursor.position--;
}

void TextFile::RemoveChar_Right()
{
    size_t cursor_pos = cursor.position;
    if (cursor_pos == cursor.GetLine().size())
    {
        RemoveLine_Right();
        return;
    }

    string &line = cursor.GetLine();
    line.erase(line.begin() + cursor_pos);
}

void TextFile::Logic(Editor &editor)
{
    if (editor.editor_state != STATE_TEXTFILE)
        return;

    if (editor.key_pressed == KEY_BACKSPACE)
        RemoveChar_Left();

    if (editor.key_pressed == KEY_DELETE)
        RemoveChar_Right();

    if (editor.char_pressed != 0)
        InsertChar(editor.char_pressed);

    if (editor.key_pressed == KEY_TAB)
    {
        for (int i = 0; i < 4; i++)
            InsertChar(' ');
    }

    if (editor.key_pressed == KEY_ENTER)
        InsertNewLine();

    if (IsKeyDown(KEY_LEFT_CONTROL))
    {
        if (IsKeyPressed(KEY_S))
        {
            Save();
        }

        if (IsKeyPressed(KEY_C))
        {
            SetClipboardText(editor.GetCurrentTextFile().cursor_select.c_str());
        }

        if (IsKeyPressed(KEY_V))
        {
            InsertStr(GetClipboardText());
            editor.GetCurrentTextFile().cursor_select.clear();
            editor.GetCurrentTextFile().rect_select.clear();
        }
    }
}

void TextFile::Draw(Editor &editor)
{
    if (editor.editor_state != STATE_TEXTFILE && editor.editor_state != STATE_COMMAND)
        return;

    int plus_minus_lines = (editor.screenHeight / editor.camera.zoom) / FONT_SIZE / 2 * 1.5f;
    int current_line = editor.GetCurrentTextFile().cursor.line_num;
    int i;
    char str_line_number[10];
    Vector2 str_line_number_size;
    for (i = current_line - plus_minus_lines; i < current_line + plus_minus_lines; i++)
    {
        if (i < 0)
            continue;
        if (i >= (int)editor.GetCurrentTextFile().buffer.size())
            break;
        // Draw text
        DrawTextEx(editor.font, editor.GetCurrentTextFile().buffer[i].c_str(), (Vector2){0, (float)FONT_SIZE * i}, FONT_SIZE, FONT_SPACING, WHITE);
        // Draw line number
        sprintf(str_line_number, "%d ", i + 1);
        str_line_number_size = MeasureTextEx(editor.font, str_line_number, FONT_SIZE, FONT_SPACING);
        DrawTextEx(editor.font, str_line_number, (Vector2){-str_line_number_size.x, (float)FONT_SIZE * i}, FONT_SIZE, FONT_SPACING, GRAY);
    }
    // Draw line separator numbers
    int final_num_pos = FONT_SIZE * i;
    DrawLineEx((Vector2){-4, 0}, (Vector2){-4, (float)final_num_pos}, 1, GRAY);
}

void TextFile::DrawNoCamera(Editor &editor)
{
    string text = "Ln " + to_string(cursor.line_num + 1) + ", Col " + to_string(cursor.position + 1);
    const char *c_text = text.c_str();
    int mult = 5;
    Vector2 text_size = MeasureTextEx(editor.font, c_text, FONT_SIZE / mult, FONT_SPACING);
    DrawText(c_text, editor.screenWidth - text_size.x * mult, editor.screenHeight - text_size.y * mult, FONT_SIZE / mult, RAYWHITE);
}