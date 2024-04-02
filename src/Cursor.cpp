#include "Cursor.hpp"
#include <string>

using namespace std;

Vector2 Cursor_GetPosition(TextFile *textFile, Font font)
{
    size_t cursor_pos = textFile->cursor.position;
    if (Cursor_GetLine(textFile, &textFile->cursor).size() == 0)
    {
        return (Vector2){0, (float)FONT_SIZE * textFile->cursor.line_num};
    }
    string aux = Cursor_GetLine(textFile, &textFile->cursor).substr(0, cursor_pos);
    Vector2 cursor_vec_pos = MeasureTextEx(font, aux.c_str(), FONT_SIZE, FONT_SPACING);
    cursor_vec_pos.y = FONT_SIZE * textFile->cursor.line_num;
    return cursor_vec_pos;
}

string &Cursor_GetLine(TextFile *textFile, Cursor *cursor)
{
    if (cursor->line_num == -1)
        throw std::range_error("Cursor_GetLine cursor line_num out of range");
    return textFile->buffer.at(cursor->line_num);
}

void Cursor_GetSelected(TextFile *textFile, Font font)
{
    Cursor *cursor1 = &textFile->cursor;
    Cursor *cursor2 = &textFile->cursor_start_select;

    // Check if there are selected
    if (cursor2->line_num == -1 || (cursor1->line_num == cursor2->line_num && cursor1->position == cursor2->position))
        return;

    if (cursor1->line_num > cursor2->line_num || ((cursor1->line_num == cursor2->line_num) && (cursor1->position > cursor2->position)))
    {
        cursor1 = &textFile->cursor_start_select;
        cursor2 = &textFile->cursor;
    }

    string aux, aux2;
    textFile->cursor_select.clear();
    textFile->rect_select.clear();
    Vector2 start_blank_size;
    Vector2 str_size;
    for (int i = cursor1->line_num; i <= cursor2->line_num; i++)
    {
        aux.clear();
        aux2.clear();
        // First line
        if (i == cursor1->line_num)
        {
            if (cursor1->line_num == cursor2->line_num)
            {
                aux.append(textFile->buffer[i], cursor1->position);
                aux2.append(textFile->buffer[i].substr(cursor1->position), cursor2->position - cursor1->position);

                start_blank_size = MeasureTextEx(font, aux.c_str(), FONT_SIZE, FONT_SPACING);
                str_size = MeasureTextEx(font, aux2.c_str(), FONT_SIZE, FONT_SPACING);
                Rectangle new_rect = {
                    .x = start_blank_size.x,
                    .y = start_blank_size.y * cursor1->line_num,
                    .width = str_size.x,
                    .height = str_size.y,
                };
                textFile->rect_select.push_back(new_rect);

                // Append to total_str
                textFile->cursor_select += aux2;
            }
            else
            {
                aux.append(textFile->buffer[i], cursor1->position);

                start_blank_size = MeasureTextEx(font, aux.c_str(), FONT_SIZE, FONT_SPACING);
                str_size = MeasureTextEx(font, textFile->buffer[i].substr(cursor1->position).c_str(), FONT_SIZE, FONT_SPACING);
                Rectangle new_rect = {
                    .x = start_blank_size.x,
                    .y = start_blank_size.y * i,
                    .width = str_size.x,
                    .height = str_size.y,
                };
                textFile->rect_select.push_back(new_rect);
                // Append to total_str
                textFile->cursor_select += textFile->buffer[i].substr(cursor1->position);
            }
        }
        // Last line
        else if (i == cursor2->line_num)
        {
            aux.append(textFile->buffer[i], cursor2->position);
            str_size = MeasureTextEx(font, aux.c_str(), FONT_SIZE, FONT_SPACING);
            Rectangle new_rect = {
                .x = 0,
                .y = (float)FONT_SIZE * i,
                .width = str_size.x,
                .height = str_size.y,
            };
            textFile->rect_select.push_back(new_rect);
            // Append to total_str
            textFile->cursor_select += '\n';
            textFile->cursor_select += aux;
        }
        // Middle lines
        else
        {
            str_size = MeasureTextEx(font, textFile->buffer[i].c_str(), FONT_SIZE, FONT_SPACING);

            Rectangle new_rect = {
                .x = 0,
                .y = (float)FONT_SIZE * i,
                .width = str_size.x,
                .height = str_size.y,
            };
            textFile->rect_select.push_back(new_rect);
            // Append to total_str
            textFile->cursor_select += '\n';
            textFile->cursor_select += textFile->buffer[i];
        }
    }
}

void Cursor_CheckClickCursorPosition(TextFile *textFile, Editor *editor)
{
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        return;

    Vector2 cursor_world_position = GetMousePosition();
    cursor_world_position = GetScreenToWorld2D(cursor_world_position, editor->camera);

    int plus_minus_lines = (editor->screenHeight / editor->camera.zoom) / FONT_SIZE / 2;
    int current_line = textFile->cursor.line_num;
    int i;
    Rectangle line_collision_box = {0};
    Vector2 line_size = {0};
    for (i = current_line - plus_minus_lines; i < current_line + plus_minus_lines; i++)
    {
        if (i < 0)
            continue;
        if (i >= (int)textFile->buffer.size())
            break;
        line_collision_box.y = FONT_SIZE * i;
        line_size = MeasureTextEx(editor->font, textFile->buffer[i].c_str(), FONT_SIZE, FONT_SPACING);
        line_collision_box.width = line_size.x + editor->screenWidth;
        line_collision_box.height = FONT_SIZE;
        if (CheckCollisionPointRec(cursor_world_position, line_collision_box))
        {
            string aux;
            size_t new_pos = __SIZE_MAX__ - 1;
            for (size_t j = 0; j < textFile->buffer[i].size(); j++)
            {
                aux.push_back(textFile->buffer[i][j]);
                line_size = MeasureTextEx(editor->font, aux.c_str(), FONT_SIZE, FONT_SPACING);
                if (cursor_world_position.x < line_size.x)
                {
                    new_pos = j;
                    break;
                }
            }

            textFile->cursor.line_num = i;
            textFile->cursor.position = new_pos;
            break;
        }
    }
}

void Cursor_UpdateSelect(TextFile *textFile, Font font)
{
    if (IsKeyDown(KEY_LEFT_SHIFT))
    {
        if (textFile->cursor_start_select.line_num == -1)
        {
            textFile->cursor_start_select.line_num = textFile->cursor.line_num;
            textFile->cursor_start_select.position = textFile->cursor.position;
        }
        Cursor_GetSelected(textFile, font);
    }
    else
    {
        textFile->cursor_start_select.line_num = -1;
        textFile->cursor_select.clear();
        textFile->rect_select.clear();
    }
}

void Cursor_Move(TextFile *textFile, Font font, KeyboardKey key_pressed)
{
    switch (key_pressed)
    {
    case KEY_RIGHT:
    case KEY_LEFT:
    case KEY_UP:
    case KEY_DOWN:
        Cursor_UpdateSelect(textFile, font);
    default:
        break;
    }
    switch (key_pressed)
    {
    case KEY_RIGHT:
        textFile->cursor.position++;
        if (textFile->cursor.position > Cursor_GetLine(textFile, &textFile->cursor).size())
        {
            key_pressed = KEY_DOWN;
            if (textFile->cursor.line_num != (int)textFile->buffer.size() - 1)
                textFile->cursor.position = 0;
        }
        // DEBUG("KEY_RIGHT");
        break;
    case KEY_LEFT:
        if (textFile->cursor.position != 0)
            textFile->cursor.position--;
        else
        {
            key_pressed = KEY_UP;
            if (textFile->cursor.line_num != 0)
                textFile->cursor.position = __SIZE_MAX__ - 1;
        }
        // DEBUG("KEY_LEFT");
        break;
    default:
        break;
    }
    switch (key_pressed)
    {
    case KEY_UP:
        if (textFile->cursor.line_num != 0)
        {
            textFile->cursor.line_num--;
        }
        // DEBUG("KEY_UP");
        break;
    case KEY_DOWN:
        if (textFile->cursor.line_num < (int)textFile->buffer.size() - 1)
        {
            textFile->cursor.line_num++;
        }
        // DEBUG("KEY_DOWN");
        break;
    default:
        break;
    }
    // Chech cursor position
    if (textFile->cursor.position > Cursor_GetLine(textFile, &textFile->cursor).size())
        textFile->cursor.position = Cursor_GetLine(textFile, &textFile->cursor).size();

    switch (key_pressed)
    {
    case KEY_RIGHT:
    case KEY_LEFT:
    case KEY_UP:
    case KEY_DOWN:
        Cursor_UpdateSelect(textFile, font);
    default:
        break;
    }
}

void Cursor_Logic(Editor *editor)
{
    if (editor->editor_state == STATE_DIRECTORY)
    {
        if (editor->key_pressed == KEY_DOWN)
        {
            if (editor->currentDirectory.selected + 1 < (int)editor->currentDirectory.items.size())
            {
                editor->currentDirectory.selected++;
            }
        }
        if (editor->key_pressed == KEY_UP)
        {
            if (editor->currentDirectory.selected != 0)
            {
                editor->currentDirectory.selected--;
            }
        }
    }
    else if (editor->editor_state == STATE_TEXTFILE)
    {
        Cursor_Move(&editor->currentTextFile, editor->font, editor->key_pressed);
        editor->cursor_pos = Cursor_GetPosition(&editor->currentTextFile, editor->font);

        Cursor_CheckClickCursorPosition(&editor->currentTextFile, editor);
    }
}

void Cursor_Draw(Editor *editor)
{
    if (editor->editor_state == STATE_DIRECTORY)
    {
        int selected = editor->currentDirectory.selected;
        Vector2 pos = (Vector2){0, (float)FONT_SIZE * selected};
        Vector2 back_size = MeasureTextEx(editor->font, editor->currentDirectory.items[selected].name.c_str(), FONT_SIZE, FONT_SPACING);
        DrawRectangleV(pos, back_size, DARKGRAY);
    }
    else if (editor->editor_state == STATE_TEXTFILE)
    {
        // Draw Cursor
        DrawRectangleV(editor->cursor_pos, (Vector2){2, FONT_SIZE}, WHITE);
        // Draw selected
        for (size_t i = 0; i < editor->currentTextFile.rect_select.size(); i++)
        {
            DrawRectangleRec(editor->currentTextFile.rect_select[i], (Color){100, 100, 100, 100});
        }
    }
}