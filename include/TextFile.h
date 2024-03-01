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
void TextFile_Free(void);
void TextFile_Save(void);

void TextFile_Print(void);

void TextFile_InsertChar(void);
void TextFile_RemoveChar(void);
void TextFile_InsertNewLine(void);
void TextFile_RemovePreLine(void);

void TextFile_MoveCursor(void);
Vector2 TextFile_GetCursorPosition(void);

void TextFile_Logic(void);
void TextFile_Draw(void);

#endif // TextFile_H