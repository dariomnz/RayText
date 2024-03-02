#ifndef TextFile_H
#define TextFile_H

#include "Common.h"
#include "DArray.h"

typedef struct Cursor
{
    DArray_char *line; // Pointer to line
    size_t line_num;   // Numero de la linea
    size_t position;   // Position in line
} Cursor;

typedef struct TextFile
{
    DArray_char **items; // Darray of pointers to lines
    size_t count;        // Darray count
    size_t capacity;     // Darray capacity

    Cursor cursor_start_select; // Cursor start select
    DArray_char cursor_select;  // Cursor select text
    Cursor cursor;              // Cursor
    DArray_char name;           // Name of file
} TextFile;

#include "Editor.h"

TextFile TextFile_Load(const DArray_char *file_name);
TextFile TextFile_LoadEmpty(void);
void TextFile_Free(TextFile *textFile);
void TextFile_Save(TextFile *textFile);

void TextFile_Print(TextFile *textFile);

void TextFile_InsertChar(TextFile *textFile, char c);
void TextFile_RemoveChar(TextFile *textFile);
void TextFile_InsertNewLine(TextFile *textFile);
void TextFile_RemovePreLine(TextFile *textFile);

void TextFile_MoveCursor(TextFile *textFile, KeyboardKey key_pressed);
Vector2 TextFile_GetCursorPosition(TextFile *textFile, Font font, int font_size, int font_spacing);

void TextFile_Logic(Editor *editor);
void TextFile_Draw(Editor *editor);

#endif // TextFile_H