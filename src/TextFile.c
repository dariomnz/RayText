#include "TextFile.h"
#include "DArray.h"

extern Editor editor;

size_t GetLineNumber(const char *file_name)
{
    FILE *file;
    char ch;
    size_t line_count = 0;

    file = fopen(file_name, "r");
    if (file == NULL)
    {
        DEBUG("Error: opening the file %s\n", file_name);
        exit(1);
    }

    while ((ch = fgetc(file)) != EOF)
        if (ch == '\n')
            line_count++;

    fclose(file);

    return line_count + 1;
}

TextFile TextFile_Load(const char *file_name)
{
    // TextFile textFile;
    // FILE *file;
    // size_t line_number = GetLineNumber(file_name);
    // size_t line_len = 0;
    // char *line = NULL;
    // size_t line_size;
    // DEBUG("Opening file: %s", file_name);
    // file = fopen(file_name, "r");
    // if (file == NULL)
    // {
    //     DEBUG("Error: opening the file %s", file_name);
    //     exit(1);
    // }

    // sprintf(textFile.name, "%s", file_name);

    // textFile.n_lines = line_number;
    // textFile.lines = (Line **)malloc(sizeof(Line *) * line_number);

    // for (size_t i = 0; i < line_number; i++)
    // {
    //     textFile.lines[i] = (Line *)malloc(sizeof(Line));
    //     line_size = getline(&line, &line_len, file);
    //     if (line_size != -1)
    //     {
    //         textFile.lines[i]->items = malloc(sizeof(char) * line_size + 1);
    //         memcpy(textFile.lines[i]->items, line, line_size);
    //         textFile.lines[i]->items[line_size] = '\0';
    //         textFile.lines[i]->capacity = line_size + 1;
    //     }
    //     else
    //     {
    //         textFile.lines[i]->items = malloc(1);
    //         textFile.lines[i]->items[0] = '\0';
    //         textFile.lines[i]->capacity = 1;
    //     }

    //     textFile.lines[i]->count = strlen(textFile.lines[i]->items);
    //     // DEBUG("Size: %ld line: %s", textFile.lines[i]->capacity, textFile.lines[i]->items);
    // }
    // free(line);
    // fclose(file);

    // textFile.cursor.position = 0;
    // textFile.cursor.line = textFile.lines[0];
    // textFile.cursor.line_num = 0;

    // return textFile;
}

TextFile TextFile_LoadEmpty()
{
    TextFile textFile = {0};
    // size_t line_number = 1;
    // size_t line_len = 0;
    // char *line = NULL;
    // size_t line_size;

    sprintf(textFile.name, "empty.txt");

    // textFile.n_lines = 1;
    Line new_line = {0};
    darray_append(&new_line, '\0');
    darray_append(&textFile, &new_line);

    // textFile.lines = (Line **)malloc(sizeof(Line *));

    // for (size_t i = 0; i < line_number; i++)
    // {
    // textFile.lines[i] = (Line *)malloc(sizeof(Line));
    // darray_append(textFile.lines[i], '\0');
    // textFile.lines[i]->items = malloc(1);
    // textFile.lines[i]->items[0] = '\0';
    // textFile.lines[i]->capacity = 1;
    // textFile.lines[i]->count = strlen(textFile.lines[i]->items);
    // DEBUG("Size: %ld line: %s", textFile.lines[i]->capacity, textFile.lines[i]->items);
    // }
    // free(line);

    textFile.cursor.position = 0;
    textFile.cursor.line = textFile.items[0];
    textFile.cursor.line_num = 0;

    return textFile;
}

void TextFile_Save()
{
    TextFile *textFile = &editor.currentTextFile;
    DEBUG("Saving: %s", textFile->name);
    FILE *file;
    // file = fopen("./input2.txt", "w");
    file = fopen(textFile->name, "w");
    if (file == NULL)
    {
        DEBUG("Error: opening the file %s", textFile->name);
        return;
    }

    for (size_t i = 0; i < textFile->count; i++)
    {
        fwrite(textFile->items[i]->items, textFile->items[i]->count, 1, file);
    }

    fclose(file);
}

void TextFile_Free()
{
    TextFile *textFile = &editor.currentTextFile;
    for (size_t i = 0; i < textFile->count; i++)
    {
        darray_free(textFile->items[i]);
        free(textFile->items[i]->items);
        free(textFile->items[i]);
    }

    free(textFile->items);
}

void TextFile_Print()
{
    TextFile *textFile = &editor.currentTextFile;
    printf("--------File \"%s\"--------\n", textFile->name);
    for (size_t i = 0; i < textFile->count; i++)
    {
        printf("%s", textFile->items[i]->items);
        if (textFile->items[i]->items[0] == '\0')
            printf("\n");
    }
    printf("------End file \"%s\"------\n", textFile->name);
}

void TextFile_InsertChar()
{
    TextFile *textFile = &editor.currentTextFile;

    darray_insert(textFile->cursor.line, editor.char_pressed, textFile->cursor.position);
    textFile->cursor.position++;

    // size_t len = textFile->cursor.line->capacity;
    // size_t lenstr = strlen(textFile->cursor.line->items);
    // size_t cursor_pos = textFile->cursor.position;

    // if (lenstr + 1 >= len)
    // {
    //     char *new_data = malloc((len + 128) * sizeof(char));
    //     if (textFile->cursor.line->items == NULL)
    //     {
    //         DEBUG("Error: Memory allocation failed\n");
    //         exit(1);
    //     }
    //     strcpy(new_data, textFile->cursor.line->items);
    //     free(textFile->cursor.line->items);
    //     textFile->cursor.line->items = new_data;
    //     len += 64;
    //     textFile->cursor.line->capacity = len;
    // }

    // memmove(textFile->cursor.line->items + cursor_pos + 1, textFile->cursor.line->items + cursor_pos, (len - cursor_pos) * sizeof(char));
    // textFile->cursor.line->items[cursor_pos] = editor.char_pressed;
    // textFile->cursor.position++;
    // textFile->cursor.line->count++;
}

void TextFile_InsertNewLine()
{
    TextFile *textFile = &editor.currentTextFile;
    size_t new_line_pos = textFile->cursor.line_num + 1;
    Line **new_lines = (Line **)realloc(textFile->items, sizeof(Line *) * (textFile->count + 1));
    if (new_lines == NULL)
    {
        DEBUG("Error: realloc of size: %ld", sizeof(Line *) * (textFile->count + 1));
        return;
    }
    textFile->items = new_lines;

    Line *new_line = (Line *)malloc(sizeof(Line));
    if (new_lines == NULL)
    {
        DEBUG("Error: malloc of size: %ld", sizeof(Line));
        return;
    }
    size_t new_line_size = textFile->cursor.line->capacity - textFile->cursor.position;
    new_line->items = malloc(new_line_size);
    if (new_lines == NULL)
    {
        DEBUG("Error: malloc of size: %ld", new_line_size);
        return;
    }
    new_line->capacity = new_line_size;
    // Copy data
    memcpy(new_line->items, &textFile->cursor.line->items[textFile->cursor.position], new_line_size);
    // Move array by 1
    memmove(&textFile->items[new_line_pos + 1], &textFile->items[new_line_pos], (textFile->count - new_line_pos) * sizeof(Line *));

    textFile->items[new_line_pos] = new_line;

    // Add return of line
    textFile->cursor.line->items[textFile->cursor.position] = '\n';
    textFile->cursor.line->items[textFile->cursor.position + 1] = '\0';
    textFile->cursor.line->count = strlen(textFile->cursor.line->items);

    // Update cursor
    textFile->cursor.line_num++;
    textFile->cursor.line = textFile->items[textFile->cursor.line_num];
    textFile->cursor.line->count = strlen(textFile->cursor.line->items);
    textFile->cursor.position = 0;

    textFile->count++;
}

void TextFile_RemovePreLine()
{
    TextFile *textFile = &editor.currentTextFile;
    if (textFile->cursor.line_num == 0)
        return;

    size_t new_line_size = textFile->items[textFile->cursor.line_num - 1]->capacity + textFile->items[textFile->cursor.line_num]->capacity;

    char *new_line_data = (char *)realloc(textFile->items[textFile->cursor.line_num - 1]->items, new_line_size);
    if (new_line_data == NULL)
    {
        DEBUG("Error: realloc of size: %ld", new_line_size);
        return;
    }
    textFile->items[textFile->cursor.line_num - 1]->items = new_line_data;
    memcpy(&new_line_data[textFile->items[textFile->cursor.line_num - 1]->count - 1], textFile->items[textFile->cursor.line_num]->items, textFile->items[textFile->cursor.line_num]->count);

    free(textFile->items[textFile->cursor.line_num]->items);
    free(textFile->items[textFile->cursor.line_num]);

    memcpy(&textFile->items[textFile->cursor.line_num], &textFile->items[textFile->cursor.line_num + 1], (textFile->count - textFile->cursor.line_num - 1) * sizeof(Line *));

    textFile->cursor.line_num--;
    textFile->cursor.line = textFile->items[textFile->cursor.line_num];
    textFile->cursor.position = textFile->cursor.line->count - 1;

    textFile->cursor.line->count = strlen(textFile->cursor.line->items);
    textFile->cursor.line->capacity = new_line_size;
    textFile->cursor.line->items[textFile->cursor.line->count + 1] = '\0';

    textFile->count--;
    textFile->items = (Line **)realloc(textFile->items, sizeof(Line *) * textFile->count);
}

void TextFile_RemoveChar()
{
    TextFile *textFile = &editor.currentTextFile;
    size_t len = textFile->cursor.line->capacity;
    size_t cursor_pos = textFile->cursor.position;
    if (cursor_pos == 0)
    {
        TextFile_RemovePreLine(textFile);
        return;
    }
    // darray_remove(textFile->cursor.line, textFile->cursor.position-1);
    memmove(textFile->cursor.line->items + cursor_pos - 1, textFile->cursor.line->items + cursor_pos, (len - cursor_pos) * sizeof(char));
    textFile->cursor.position--;
    textFile->cursor.line->count--;
}

void TextFile_MoveCursor()
{
    TextFile *textFile = &editor.currentTextFile;
    switch (editor.key_pressed)
    {
    case KEY_RIGHT:

        textFile->cursor.position++;
        if (textFile->cursor.position + 1 > textFile->cursor.line->count)
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
    if (textFile->cursor.position + 1 > textFile->cursor.line->count)
        if (textFile->cursor.line->items[textFile->cursor.line->count - 1] == '\n')
            textFile->cursor.position = textFile->cursor.line->count - 1;
        else
            textFile->cursor.position = textFile->cursor.line->count;

    // DEBUG("cursor_pos %ld str_size %ld line %ld char %c", textFile->cursor.position, textFile->cursor.line->count, textFile->cursor.line_num, textFile->cursor.line->items[textFile->cursor.line->count - 1]);
}

Vector2 TextFile_GetCursorPosition()
{
    TextFile *textFile = &editor.currentTextFile;
    size_t cursor_pos = textFile->cursor.position;
    char cursor_char = textFile->cursor.line->items[cursor_pos];
    textFile->cursor.line->items[cursor_pos] = '\0';
    Vector2 cursor_vec_pos = MeasureTextEx(editor.font, textFile->cursor.line->items, editor.font_size, editor.font_spacing);
    textFile->cursor.line->items[cursor_pos] = cursor_char;
    cursor_vec_pos.y = editor.font_size * textFile->cursor.line_num;
    return cursor_vec_pos;
}

void TextFile_Logic()
{
    if (editor.editor_state != STATE_TEXTFILE)
        return;

    TextFile_MoveCursor(&editor.currentTextFile);
    editor.cursor_pos = TextFile_GetCursorPosition(editor.currentTextFile);

    if (editor.key_pressed == KEY_BACKSPACE)
        TextFile_RemoveChar(&editor.currentTextFile);

    if (editor.char_pressed != 0)
        TextFile_InsertChar(&editor.currentTextFile, editor.char_pressed);

    if (editor.key_pressed == KEY_ENTER)
        TextFile_InsertNewLine(&editor.currentTextFile);

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S))
    {
        TextFile_Save(editor.currentTextFile);
    }
}

int final_num_pos = 0;
char str_line_number[10];
Vector2 str_line_number_size;
void TextFile_Draw()
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