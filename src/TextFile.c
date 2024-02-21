#include "TextFile.h"

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
    TextFile textFile;
    FILE *file;
    size_t line_number = GetLineNumber(file_name);
    size_t line_len = 0;
    char *line = NULL;
    size_t line_size;

    file = fopen(file_name, "r");
    if (file == NULL)
    {
        DEBUG("Error: opening the file %s", file_name);
        exit(1);
    }

    sprintf(textFile.name, "%s", file_name);

    textFile.n_lines = line_number;
    textFile.lines = (Line **)malloc(sizeof(Line *) * line_number);

    for (size_t i = 0; i < line_number; i++)
    {
        textFile.lines[i] = (Line *)malloc(sizeof(Line));
        line_size = getline(&line, &line_len, file);
        if (line_size != -1)
        {
            textFile.lines[i]->data = malloc(sizeof(char) * line_size + 1);
            memcpy(textFile.lines[i]->data, line, line_size);
            textFile.lines[i]->data[line_size] = '\0';
            textFile.lines[i]->size = line_size + 1;
        }
        else
        {
            textFile.lines[i]->data = malloc(1);
            textFile.lines[i]->data[0] = '\0';
            textFile.lines[i]->size = 1;
        }

        textFile.lines[i]->str_size = strlen(textFile.lines[i]->data);
        // DEBUG("Size: %ld line: %s", textFile.lines[i]->size, textFile.lines[i]->data);
    }
    free(line);
    fclose(file);

    textFile.cursor.position = 0;
    textFile.cursor.line = textFile.lines[0];
    textFile.cursor.line_num = 0;

    return textFile;
}

TextFile TextFile_LoadEmpty()
{
    TextFile textFile;
    size_t line_number = 1;
    size_t line_len = 0;
    char *line = NULL;
    size_t line_size;

    sprintf(textFile.name, "empty.txt");

    textFile.n_lines = line_number;
    textFile.lines = (Line **)malloc(sizeof(Line *) * line_number);

    for (size_t i = 0; i < line_number; i++)
    {
        textFile.lines[i] = (Line *)malloc(sizeof(Line));

        textFile.lines[i]->data = malloc(1);
        textFile.lines[i]->data[0] = '\0';
        textFile.lines[i]->size = 1;
        textFile.lines[i]->str_size = strlen(textFile.lines[i]->data);
        // DEBUG("Size: %ld line: %s", textFile.lines[i]->size, textFile.lines[i]->data);
    }
    free(line);

    textFile.cursor.position = 0;
    textFile.cursor.line = textFile.lines[0];
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

    for (size_t i = 0; i < textFile->n_lines; i++)
    {
        fwrite(textFile->lines[i]->data, textFile->lines[i]->str_size, 1, file);
    }

    fclose(file);
}

void TextFile_Free()
{
    TextFile *textFile = &editor.currentTextFile;
    for (size_t i = 0; i < textFile->n_lines; i++)
    {
        free(textFile->lines[i]->data);
        free(textFile->lines[i]);
    }

    free(textFile->lines);
}

void TextFile_Print()
{
    TextFile *textFile = &editor.currentTextFile;
    printf("--------File \"%s\"--------\n", textFile->name);
    for (size_t i = 0; i < textFile->n_lines; i++)
    {
        printf("%s", textFile->lines[i]->data);
        if (textFile->lines[i]->data[0] == '\0')
            printf("\n");
    }
    printf("------End file \"%s\"------\n", textFile->name);
}

void TextFile_InsertChar()
{
    TextFile *textFile = &editor.currentTextFile;
    size_t len = textFile->cursor.line->size;
    size_t lenstr = strlen(textFile->cursor.line->data);
    size_t cursor_pos = textFile->cursor.position;

    if (lenstr + 1 >= len)
    {
        char *new_data = malloc((len + 128) * sizeof(char));
        if (textFile->cursor.line->data == NULL)
        {
            DEBUG("Error: Memory allocation failed\n");
            exit(1);
        }
        strcpy(new_data, textFile->cursor.line->data);
        free(textFile->cursor.line->data);
        textFile->cursor.line->data = new_data;
        len += 64;
        textFile->cursor.line->size = len;
    }

    memmove(textFile->cursor.line->data + cursor_pos + 1, textFile->cursor.line->data + cursor_pos, (len - cursor_pos) * sizeof(char));
    textFile->cursor.line->data[cursor_pos] = editor.char_pressed;
    textFile->cursor.position++;
    textFile->cursor.line->str_size++;
}

void TextFile_InsertNewLine()
{
    TextFile *textFile = &editor.currentTextFile;
    size_t new_line_pos = textFile->cursor.line_num + 1;
    Line **new_lines = (Line **)realloc(textFile->lines, sizeof(Line *) * (textFile->n_lines + 1));
    if (new_lines == NULL)
    {
        DEBUG("Error: realloc of size: %ld", sizeof(Line *) * (textFile->n_lines + 1));
        return;
    }
    textFile->lines = new_lines;

    Line *new_line = (Line *)malloc(sizeof(Line));
    if (new_lines == NULL)
    {
        DEBUG("Error: malloc of size: %ld", sizeof(Line));
        return;
    }
    size_t new_line_size = textFile->cursor.line->size - textFile->cursor.position;
    new_line->data = malloc(new_line_size);
    if (new_lines == NULL)
    {
        DEBUG("Error: malloc of size: %ld", new_line_size);
        return;
    }
    new_line->size = new_line_size;
    // Copy data
    memcpy(new_line->data, &textFile->cursor.line->data[textFile->cursor.position], new_line_size);
    // Move array by 1
    memcpy(&textFile->lines[new_line_pos + 1], &textFile->lines[new_line_pos], (textFile->n_lines - new_line_pos) * sizeof(Line *));

    textFile->lines[new_line_pos] = new_line;

    // Add return of line
    textFile->cursor.line->data[textFile->cursor.position] = '\n';
    textFile->cursor.line->data[textFile->cursor.position + 1] = '\0';
    textFile->cursor.line->str_size = strlen(textFile->cursor.line->data);

    // Update cursor
    textFile->cursor.line_num++;
    textFile->cursor.line = textFile->lines[textFile->cursor.line_num];
    textFile->cursor.line->str_size = strlen(textFile->cursor.line->data);
    textFile->cursor.position = 0;

    textFile->n_lines++;
}

void TextFile_RemovePreLine()
{
    TextFile *textFile = &editor.currentTextFile;
    if (textFile->cursor.line_num == 0)
        return;

    size_t new_line_size = textFile->lines[textFile->cursor.line_num - 1]->size + textFile->lines[textFile->cursor.line_num]->size;

    char *new_line_data = (char *)realloc(textFile->lines[textFile->cursor.line_num - 1]->data, new_line_size);
    if (new_line_data == NULL)
    {
        DEBUG("Error: realloc of size: %ld", new_line_size);
        return;
    }
    textFile->lines[textFile->cursor.line_num - 1]->data = new_line_data;
    memcpy(&new_line_data[textFile->lines[textFile->cursor.line_num - 1]->str_size - 1], textFile->lines[textFile->cursor.line_num]->data, textFile->lines[textFile->cursor.line_num]->str_size);

    free(textFile->lines[textFile->cursor.line_num]->data);
    free(textFile->lines[textFile->cursor.line_num]);

    memcpy(&textFile->lines[textFile->cursor.line_num], &textFile->lines[textFile->cursor.line_num + 1], (textFile->n_lines - textFile->cursor.line_num - 1) * sizeof(Line *));

    textFile->cursor.line_num--;
    textFile->cursor.line = textFile->lines[textFile->cursor.line_num];
    textFile->cursor.position = textFile->cursor.line->str_size - 1;

    textFile->cursor.line->str_size = strlen(textFile->cursor.line->data);
    textFile->cursor.line->size = new_line_size;
    textFile->cursor.line->data[textFile->cursor.line->str_size + 1] = '\0';

    textFile->n_lines--;
    textFile->lines = (Line **)realloc(textFile->lines, sizeof(Line *) * textFile->n_lines);
}

void TextFile_RemoveChar()
{
    TextFile *textFile = &editor.currentTextFile;
    size_t len = textFile->cursor.line->size;
    size_t cursor_pos = textFile->cursor.position;
    if (cursor_pos == 0)
    {
        TextFile_RemovePreLine(textFile);
        return;
    }

    memmove(textFile->cursor.line->data + cursor_pos - 1, textFile->cursor.line->data + cursor_pos, (len - cursor_pos) * sizeof(char));
    textFile->cursor.position--;
    textFile->cursor.line->str_size--;
}

void TextFile_MoveCursor()
{
    TextFile *textFile = &editor.currentTextFile;
    switch (editor.key_pressed)
    {
    case KEY_RIGHT:

        textFile->cursor.position++;
        if (textFile->cursor.position + 1 > textFile->cursor.line->str_size)
        {
            editor.key_pressed = KEY_DOWN;
            if (textFile->cursor.line_num != textFile->n_lines - 1)
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
            textFile->cursor.line = textFile->lines[textFile->cursor.line_num];
        }
        // DEBUG("KEY_UP");
        break;
    case KEY_DOWN:
        if (textFile->cursor.line_num < textFile->n_lines - 1)
        {
            textFile->cursor.line_num++;
            textFile->cursor.line = textFile->lines[textFile->cursor.line_num];
        }
        // DEBUG("KEY_DOWN");
        break;
    default:
        break;
    }
    if (textFile->cursor.position + 1 > textFile->cursor.line->str_size)
        if (textFile->cursor.line->data[textFile->cursor.line->str_size - 1] == '\n')
            textFile->cursor.position = textFile->cursor.line->str_size - 1;
        else
            textFile->cursor.position = textFile->cursor.line->str_size;

    // DEBUG("cursor_pos %ld str_size %ld line %ld char %c", textFile->cursor.position, textFile->cursor.line->str_size, textFile->cursor.line_num, textFile->cursor.line->data[textFile->cursor.line->str_size - 1]);
}

Vector2 TextFile_GetCursorPosition()
{
    TextFile *textFile = &editor.currentTextFile;
    size_t cursor_pos = textFile->cursor.position;
    char cursor_char = textFile->cursor.line->data[cursor_pos];
    textFile->cursor.line->data[cursor_pos] = '\0';
    Vector2 cursor_vec_pos = MeasureTextEx(editor.font, textFile->cursor.line->data, editor.font_size, editor.font_spacing);
    textFile->cursor.line->data[cursor_pos] = cursor_char;
    cursor_vec_pos.y = editor.font_size * textFile->cursor.line_num;
    return cursor_vec_pos;
}

void TextFile_Draw()
{
    
}