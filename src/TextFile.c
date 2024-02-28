#include "TextFile.h"
#include "DArray.h"

extern Editor editor;

TextFile TextFile_Load(const DArray_char *file_name)
{
    TextFile textFile = {0};
    DArray_append_many(&textFile.name, file_name->items, file_name->count);
    DEBUG("Opening file: %s", file_name->items);
    char *data = LoadFileText(file_name->items);
    size_t last_line = 0;
    DArray_char *new_line;
    size_t i = 0;
    for (i = 0; i < strlen(data); i++)
    {
        if (data[i] == '\n')
        {
            new_line = malloc(sizeof(DArray_char));
            memset(new_line, 0, sizeof(DArray_char));
            DArray_append_many(new_line, &data[last_line], i - last_line);
            DArray_append(&textFile, new_line);
            last_line = i + 1;
        }
    }
    new_line = malloc(sizeof(DArray_char));
    memset(new_line, 0, sizeof(DArray_char));
    DArray_append_many(new_line, &data[last_line], i - last_line);
    DArray_append(&textFile, new_line);
    UnloadFileText(data);

    textFile.cursor.position = 0;
    textFile.cursor.line = textFile.items[0];
    textFile.cursor.line_num = 0;

    return textFile;
}

TextFile TextFile_LoadEmpty(void)
{
    TextFile textFile = {0};
    DArray_append_many(&textFile.name, "empty.txt", 10);

    DArray_char *new_line = malloc(sizeof(DArray_char));
    if (new_line == NULL)
    {
        DEBUG("Error: malloc of size: %ld", sizeof(DArray_char));
        return textFile;
    }
    memset(new_line, 0, sizeof(DArray_char));

    DArray_append(&textFile, new_line);

    // Init Cursor
    textFile.cursor.position = 0;
    textFile.cursor.line = textFile.items[0];
    textFile.cursor.line_num = 0;

    return textFile;
}

DArray_char TextFile_to_string(void)
{
    TextFile *textFile = &editor.currentTextFile;
    DArray_char data = {0};
    for (size_t i = 0; i < textFile->count; i++)
    {
        DArray_append_many(&data, textFile->items[i]->items, textFile->items[i]->count);
        if (i != textFile->count - 1)
            DArray_append(&data, '\n');
    }
    return data;
}

void TextFile_Save(void)
{
    TextFile *textFile = &editor.currentTextFile;
    DEBUG("Saving: %s", textFile->name);
    DArray_char data = TextFile_to_string();
    
    SaveFileText(textFile->name.items, data.items);

    DArray_free(&data);
}

void TextFile_Free(void)
{
    TextFile *textFile = &editor.currentTextFile;
    for (size_t i = 0; i < textFile->count; i++)
    {
        DArray_free(textFile->items[i]);
        free(textFile->items[i]);
    }

    DArray_free(textFile);
}

void TextFile_Print(void)
{
    TextFile *textFile = &editor.currentTextFile;
    printf("--------File \"%s\"--------\n", textFile->name);

    DArray_char data = TextFile_to_string();
    printf("%s\n", data.items);
    DArray_free(&data);
    printf("------End file \"%s\"------\n", textFile->name);
}

void TextFile_InsertChar(void)
{
    TextFile *textFile = &editor.currentTextFile;

    DArray_insert(textFile->cursor.line, editor.char_pressed, textFile->cursor.position);
    textFile->cursor.position++;
}

void TextFile_InsertNewLine(void)
{
    TextFile *textFile = &editor.currentTextFile;
    size_t new_line_pos = textFile->cursor.line_num + 1;
    size_t new_line_chars = textFile->cursor.line->count - textFile->cursor.position;

    // Build new line
    DArray_char *new_line2 = malloc(sizeof(DArray_char));
    if (new_line2 == NULL)
    {
        DEBUG("Error: malloc of size: %ld", sizeof(DArray_char));
        return;
    }
    memset(new_line2, 0, sizeof(DArray_char));
    DArray_append_many(new_line2, &textFile->cursor.line->items[textFile->cursor.position], new_line_chars);

    DArray_insert(textFile, new_line2, new_line_pos);

    // Add return of line
    if (textFile->cursor.position != textFile->cursor.line->count)
        DArray_remove_from(textFile->cursor.line, textFile->cursor.position);

    // Update cursor
    textFile->cursor.line_num++;
    textFile->cursor.line = textFile->items[textFile->cursor.line_num];
    textFile->cursor.position = 0;
}

void TextFile_RemoveLine_Left(void)
{
    TextFile *textFile = &editor.currentTextFile;
    if (textFile->cursor.line_num == 0)
        return;

    size_t position_pre_line = textFile->items[textFile->cursor.line_num - 1]->count;
    DArray_append_many(textFile->items[textFile->cursor.line_num - 1], textFile->cursor.line->items, textFile->cursor.line->count);
    DArray_remove(textFile, textFile->cursor.line_num);

    // Update cursor
    textFile->cursor.line_num--;
    textFile->cursor.line = textFile->items[textFile->cursor.line_num];
    textFile->cursor.position = position_pre_line;
}

void TextFile_RemoveLine_Right(void)
{
    TextFile *textFile = &editor.currentTextFile;
    if (textFile->cursor.line_num + 1 == textFile->count)
        return;

    DArray_append_many(textFile->cursor.line, textFile->items[textFile->cursor.line_num + 1]->items, textFile->items[textFile->cursor.line_num + 1]->count);
    DArray_remove(textFile, textFile->cursor.line_num + 1);
}

void TextFile_RemoveChar_Left(void)
{
    TextFile *textFile = &editor.currentTextFile;

    size_t cursor_pos = textFile->cursor.position;
    if (cursor_pos == 0)
    {
        TextFile_RemoveLine_Left();
        return;
    }

    DArray_remove(textFile->cursor.line, cursor_pos - 1);

    // Update cursor
    textFile->cursor.position--;
}

void TextFile_RemoveChar_Right(void)
{
    TextFile *textFile = &editor.currentTextFile;

    size_t cursor_pos = textFile->cursor.position;
    if (cursor_pos == textFile->cursor.line->count)
    {
        TextFile_RemoveLine_Right();
        return;
    }

    DArray_remove(textFile->cursor.line, cursor_pos);
}

void TextFile_MoveCursor(void)
{
    TextFile *textFile = &editor.currentTextFile;
    switch (editor.key_pressed)
    {
    case KEY_RIGHT:

        textFile->cursor.position++;
        if (textFile->cursor.position > textFile->cursor.line->count)
        {
            editor.key_pressed = KEY_DOWN;
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
            editor.key_pressed = KEY_UP;
            if (textFile->cursor.line_num != 0)
                textFile->cursor.position = __SIZE_MAX__ - 1;
        }
        // DEBUG("KEY_LEFT");
        break;
    default:
        break;
    }
    switch (editor.key_pressed)
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
}

Vector2 TextFile_GetCursorPosition(void)
{
    TextFile *textFile = &editor.currentTextFile;
    size_t cursor_pos = textFile->cursor.position;
    if (textFile->cursor.line->count == 0)
    {
        return (Vector2){0, editor.font_size * textFile->cursor.line_num};
    }
    char cursor_char = textFile->cursor.line->items[cursor_pos];
    textFile->cursor.line->items[cursor_pos] = '\0';
    Vector2 cursor_vec_pos = MeasureTextEx(editor.font, textFile->cursor.line->items, editor.font_size, editor.font_spacing);
    textFile->cursor.line->items[cursor_pos] = cursor_char;
    cursor_vec_pos.y = editor.font_size * textFile->cursor.line_num;
    return cursor_vec_pos;
}

void TextFile_Logic(void)
{
    if (editor.editor_state != STATE_TEXTFILE)
        return;

    TextFile_MoveCursor();
    editor.cursor_pos = TextFile_GetCursorPosition();

    if (editor.key_pressed == KEY_BACKSPACE)
        TextFile_RemoveChar_Left();

    if (editor.key_pressed == KEY_DELETE)
        TextFile_RemoveChar_Right();

    if (editor.char_pressed != 0)
        TextFile_InsertChar();

    if (editor.key_pressed == KEY_TAB)
    {
        editor.char_pressed = ' ';
        for (int i = 0; i < 4; i++)
            TextFile_InsertChar();
    }

    if (editor.key_pressed == KEY_ENTER)
        TextFile_InsertNewLine();

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S))
    {
        TextFile_Save();
    }
}

int final_num_pos = 0;
char str_line_number[10];
Vector2 str_line_number_size;
void TextFile_Draw(void)
{
    if (editor.editor_state != STATE_TEXTFILE && editor.editor_state != STATE_COMMAND)
        return;

    int plus_minus_lines = (editor.screenHeight / editor.camera.zoom) / editor.font_size / 2;
    int current_line = editor.currentTextFile.cursor.line_num;
    int i;
    for (i = current_line - plus_minus_lines; i < current_line + plus_minus_lines; i++)
    {
        if (i < 0)
            continue;
        if (i >= editor.currentTextFile.count)
            break;
        // Draw text
        DrawTextEx(editor.font, editor.currentTextFile.items[i]->items, (Vector2){0, editor.font_size * i}, editor.font_size, editor.font_spacing, WHITE);
        // Draw line number
        sprintf(str_line_number, "%d ", i + 1);
        str_line_number_size = MeasureTextEx(editor.font, str_line_number, editor.font_size, editor.font_spacing);
        DrawTextEx(editor.font, str_line_number, (Vector2){-str_line_number_size.x, editor.font_size * i}, editor.font_size, editor.font_spacing, GRAY);
    }
    // Draw line separator numbers
    final_num_pos = editor.font_size * i;
    DrawLineEx((Vector2){-4, 0}, (Vector2){-4, final_num_pos}, 1, GRAY);
    // Draw cursor
    DrawRectangleV(editor.cursor_pos, (Vector2){2, editor.font_size}, WHITE);
}