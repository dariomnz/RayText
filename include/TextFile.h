#ifndef TextFile_H
#define TextFile_H

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

#include "Common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Line
{
    char *data;      // Data of line
    size_t size;     // Size of data
    size_t str_size; // Size of line
} Line;

typedef struct Cursor
{
    Line *line;      // Pointer to line
    size_t line_num; // Numero de la linea
    size_t position; // Position in line
} Cursor;

typedef struct TextFile
{
    size_t n_lines; // Size of lines
    Line **lines;   // Array of pointers to lines
    Cursor cursor;  // Cursor
    char name[256];
} TextFile;

size_t GetLineNumber(const char *file_name);

TextFile LoadTextFile(const char *file_name);

void SaveTextFile(TextFile textFile);

void FreeTextFile(TextFile textFile);

void PrintTextFile(TextFile textFile);

void InsertChar(TextFile *textFile, char c);

void InsertNewLine(TextFile *textFile);

void RemovePreLine(TextFile *textFile);

void RemoveChar(TextFile *textFile);

void MoveCursor(TextFile *textFile, int key_press);

Vector2 GetCursorPosition(TextFile textFile);

#endif // TextFile_H