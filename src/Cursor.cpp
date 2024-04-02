#include "Cursor.hpp"
#include "DArray.hpp"

Vector2 Cursor_GetPosition(TextFile *textFile, Font font)
{
    size_t cursor_pos = textFile->cursor.position;
    if (Cursor_GetLine(textFile, &textFile->cursor)->count == 0)
    {
        return (Vector2){0, FONT_SIZE * textFile->cursor.line_num};
    }
    char cursor_char = Cursor_GetLine(textFile, &textFile->cursor)->items[cursor_pos];
    Cursor_GetLine(textFile, &textFile->cursor)->items[cursor_pos] = '\0';
    Vector2 cursor_vec_pos = MeasureTextEx(font, Cursor_GetLine(textFile, &textFile->cursor)->items, FONT_SIZE, FONT_SPACING);
    Cursor_GetLine(textFile, &textFile->cursor)->items[cursor_pos] = cursor_char;
    cursor_vec_pos.y = FONT_SIZE * textFile->cursor.line_num;
    return cursor_vec_pos;
}

DArray_char *Cursor_GetLine(TextFile *textFile, Cursor *cursor)
{
    return cursor->line_num != -1 ? textFile->items[cursor->line_num] : NULL;
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

    DArray_char aux = {0};
    DArray_char aux2 = {0};
    DArray_clear(&textFile->cursor_select);
    DArray_clear(&textFile->rect_select);
    Vector2 start_blank_size;
    Vector2 str_size;
    for (size_t i = cursor1->line_num; i <= cursor2->line_num; i++)
    {
        DArray_clear(&aux);
        DArray_clear(&aux2);
        // First line
        if (i == cursor1->line_num)
        {
            if (cursor1->line_num == cursor2->line_num)
            {
                DArray_append_many(&aux, textFile->items[i]->items, cursor1->position);
                DArray_append_many(&aux2, &textFile->items[i]->items[cursor1->position], cursor2->position - cursor1->position);

                start_blank_size = MeasureTextEx(font, aux.items, FONT_SIZE, FONT_SPACING);
                str_size = MeasureTextEx(font, aux2.items, FONT_SIZE, FONT_SPACING);
                Rectangle new_rect = {
                    .x = start_blank_size.x,
                    .y = start_blank_size.y * cursor1->line_num,
                    .width = str_size.x,
                    .height = str_size.y,
                };
                DArray_append(&textFile->rect_select, new_rect);

                // Append to total_str
                DArray_append_many(&textFile->cursor_select, aux2.items, aux2.count);
            }
            else
            {
                DArray_append_many(&aux, textFile->items[i]->items, cursor1->position);

                start_blank_size = MeasureTextEx(font, aux.items, FONT_SIZE, FONT_SPACING);
                str_size = MeasureTextEx(font, &textFile->items[i]->items[cursor1->position], FONT_SIZE, FONT_SPACING);
                Rectangle new_rect = {
                    .x = start_blank_size.x,
                    .y = start_blank_size.y * i,
                    .width = str_size.x,
                    .height = str_size.y,
                };
                DArray_append(&textFile->rect_select, new_rect);
                // Append to total_str
                DArray_append_many(&textFile->cursor_select, &textFile->items[i]->items[cursor1->position], textFile->items[i]->count - cursor1->position);
            }
        }
        // Last line
        else if (i == cursor2->line_num)
        {
            DArray_append_many(&aux, textFile->items[i]->items, cursor2->position);
            str_size = MeasureTextEx(font, aux.items, FONT_SIZE, FONT_SPACING);
            Rectangle new_rect = {
                .x = 0,
                .y = FONT_SIZE * i,
                .width = str_size.x,
                .height = str_size.y,
            };
            DArray_append(&textFile->rect_select, new_rect);
            // Append to total_str
            DArray_append(&textFile->cursor_select, '\n');
            DArray_append_many(&textFile->cursor_select, aux.items, aux.count);
        }
        // Middle lines
        else
        {
            str_size = MeasureTextEx(font, textFile->items[i]->items, FONT_SIZE, FONT_SPACING);

            Rectangle new_rect = {
                .x = 0,
                .y = FONT_SIZE * i,
                .width = str_size.x,
                .height = str_size.y,
            };
            DArray_append(&textFile->rect_select, new_rect);
            // Append to total_str
            DArray_append(&textFile->cursor_select, '\n');
            DArray_append_many(&textFile->cursor_select, textFile->items[i]->items, textFile->items[i]->count);
        }
    }
    DArray_free(&aux);
    DArray_free(&aux2);
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
        if (i >= textFile->count)
            break;
        line_collision_box.y = FONT_SIZE * i;
        line_size = MeasureTextEx(editor->font, textFile->items[i]->items, FONT_SIZE, FONT_SPACING);
        line_collision_box.width = line_size.x + editor->screenWidth;
        line_collision_box.height = FONT_SIZE;
        if (CheckCollisionPointRec(cursor_world_position, line_collision_box))
        {
            DArray_char aux = {0};
            size_t new_pos = __SIZE_MAX__ - 1;
            for (size_t j = 0; j < textFile->items[i]->count; j++)
            {
                DArray_append(&aux, textFile->items[i]->items[j]);
                line_size = MeasureTextEx(editor->font, aux.items, FONT_SIZE, FONT_SPACING);
                if (cursor_world_position.x < line_size.x)
                {
                    new_pos = j;
                    break;
                }
            }

            textFile->cursor.line_num = i;
            textFile->cursor.position = new_pos;
            DArray_free(&aux);
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
        DArray_clear(&textFile->cursor_select);
        DArray_clear(&textFile->rect_select);
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
        if (textFile->cursor.position > Cursor_GetLine(textFile, &textFile->cursor)->count)
        {
            key_pressed = KEY_DOWN;
            if (textFile->cursor.line_num != (int)textFile->count - 1)
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
        if (textFile->cursor.line_num < (int)textFile->count - 1)
        {
            textFile->cursor.line_num++;
        }
        // DEBUG("KEY_DOWN");
        break;
    default:
        break;
    }
    // Chech cursor position
    if (textFile->cursor.position > Cursor_GetLine(textFile, &textFile->cursor)->count)
        textFile->cursor.position = Cursor_GetLine(textFile, &textFile->cursor)->count;

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
            if (editor->currentDirectory.selected + 1 < (int)editor->currentDirectory.count)
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
        Vector2 back_size = MeasureTextEx(editor->font, editor->currentDirectory.items[selected]->items, FONT_SIZE, FONT_SPACING);
        DrawRectangleV(pos, back_size, DARKGRAY);
    }
    else if (editor->editor_state == STATE_TEXTFILE)
    {
        // Draw Cursor
        DrawRectangleV(editor->cursor_pos, (Vector2){2, FONT_SIZE}, WHITE);
        // Draw selected
        for (size_t i = 0; i < editor->currentTextFile.rect_select.count; i++)
        {
            DrawRectangleRec(editor->currentTextFile.rect_select.items[i], (Color){100, 100, 100, 100});
        }
    }
}