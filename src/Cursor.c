#include "Cursor.h"

Vector2 Cursor_GetPosition(TextFile *textFile, Font font)
{
    size_t cursor_pos = textFile->cursor.position;
    if (textFile->cursor.line->count == 0)
    {
        return (Vector2){0, FONT_SIZE * textFile->cursor.line_num};
    }
    char cursor_char = textFile->cursor.line->items[cursor_pos];
    textFile->cursor.line->items[cursor_pos] = '\0';
    Vector2 cursor_vec_pos = MeasureTextEx(font, textFile->cursor.line->items, FONT_SIZE, FONT_SPACING);
    textFile->cursor.line->items[cursor_pos] = cursor_char;
    cursor_vec_pos.y = FONT_SIZE * textFile->cursor.line_num;
    return cursor_vec_pos;
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
            textFile->cursor.line = textFile->items[i];
            textFile->cursor.position = new_pos;
            DArray_free(&aux);
            break;
        }
    }
}

void Cursor_UpdateSelect(TextFile *textFile)
{
    if (IsKeyDown(KEY_LEFT_SHIFT))
        if (textFile->cursor_start_select.line == NULL)
        {
            textFile->cursor_start_select.line = textFile->cursor.line;
            textFile->cursor_start_select.line_num = textFile->cursor.line_num;
            textFile->cursor_start_select.position = textFile->cursor.position;
        }
        else
        {
            DArray_clear(&textFile->cursor_select);
            if (textFile->cursor.position >= textFile->cursor_start_select.position)
                DArray_append_many(&textFile->cursor_select, &textFile->cursor_start_select.line->items[textFile->cursor_start_select.position], textFile->cursor.position - textFile->cursor_start_select.position);
            else
                DArray_append_many(&textFile->cursor_select, &textFile->cursor_start_select.line->items[textFile->cursor.position], textFile->cursor_start_select.position - textFile->cursor.position);
        }
    else
        textFile->cursor_start_select.line = NULL;
}

void Cursor_Move(TextFile *textFile, KeyboardKey key_pressed)
{
    switch (key_pressed)
    {
    case KEY_RIGHT:
    case KEY_LEFT:
    case KEY_UP:
    case KEY_DOWN:
        Cursor_UpdateSelect(textFile);
    }
    switch (key_pressed)
    {
    case KEY_RIGHT:
        textFile->cursor.position++;
        if (textFile->cursor.position > textFile->cursor.line->count)
        {
            key_pressed = KEY_DOWN;
            if (textFile->cursor.line_num != textFile->count - 1)
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
            textFile->cursor.line = textFile->items[textFile->cursor.line_num];
        }
        // DEBUG("KEY_UP");
        break;
    case KEY_DOWN:
        if (textFile->cursor.line_num < textFile->count - 1)
        {
            textFile->cursor.line_num++;
            textFile->cursor.line = textFile->items[textFile->cursor.line_num];
        }
        // DEBUG("KEY_DOWN");
        break;
    default:
        break;
    }
    // Chech cursor position
    if (textFile->cursor.position > textFile->cursor.line->count)
        textFile->cursor.position = textFile->cursor.line->count;
    // DEBUG("cursor_pos %ld str_size %ld line %ld char %c", textFile->cursor.position, textFile->cursor.line->count, textFile->cursor.line_num, textFile->cursor.line->items[textFile->cursor.line->count - 1]);

    switch (key_pressed)
    {
    case KEY_RIGHT:
    case KEY_LEFT:
    case KEY_UP:
    case KEY_DOWN:
        Cursor_UpdateSelect(textFile);
    }
}

void Cursor_Logic(Editor *editor)
{

    Cursor_Move(&editor->currentTextFile, editor->key_pressed);
    editor->cursor_pos = Cursor_GetPosition(&editor->currentTextFile, editor->font);

    Cursor_CheckClickCursorPosition(&editor->currentTextFile, editor);
}

void Cursor_Draw(Editor *editor)
{
    if (editor->editor_state == STATE_DIRECTORY)
    {
        int selected = editor->currentDirectory.selected;
        Vector2 pos = (Vector2){0, FONT_SIZE * selected};
        Vector2 back_size = MeasureTextEx(editor->font, editor->currentDirectory.items[selected]->items, FONT_SIZE, FONT_SPACING);
        DrawRectangleV(pos, back_size, DARKGRAY);
    }
    else if (editor->editor_state == STATE_TEXTFILE)
    {
        DrawRectangleV(editor->cursor_pos, (Vector2){2, FONT_SIZE}, WHITE);
        DArray_char aux = {0};

        // Draw selection
        Vector2 str_line_start_select;
        Vector2 str_line_start_size;
        if (editor->currentTextFile.cursor_start_select.line != NULL)
        {
            if (editor->currentTextFile.cursor.position >= editor->currentTextFile.cursor_start_select.position)
                DArray_append_many(&aux, editor->currentTextFile.cursor_start_select.line, editor->currentTextFile.cursor_start_select.position);
            // DArray_append_many(&editor->currentTextFile.cursor_select, &editor->currentTextFile.cursor_start_select.line->items[editor->currentTextFile.cursor_start_select.position], editor->currentTextFile.cursor.position - editor->currentTextFile.cursor_start_select.position);
            else
                DArray_append_many(&aux, editor->currentTextFile.cursor_start_select.line, editor->currentTextFile.cursor.position);
            // DArray_append_many(&editor->currentTextFile.cursor_select, &editor->currentTextFile.cursor_start_select.line->items[editor->currentTextFile.cursor.position], editor->currentTextFile.cursor_start_select.position - editor->currentTextFile.cursor.position);

            // DArray_append_many(&aux, editor->currentTextFile.cursor_start_select.line, )
            str_line_start_select = MeasureTextEx(editor->font, aux.items, FONT_SIZE, FONT_SPACING);
            str_line_start_size = MeasureTextEx(editor->font, editor->currentTextFile.cursor_select.items, FONT_SIZE, FONT_SPACING);
            DrawRectangleV((Vector2){str_line_start_select.x, editor->currentTextFile.cursor_start_select.line_num * FONT_SIZE}, str_line_start_size, (Color){100, 100, 100, 100});
        }
        DArray_free(&aux);
    }
}