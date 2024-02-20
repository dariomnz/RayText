#ifndef Editor_H
#define Editor_H

#include "TextFile.h"

typedef struct Editor
{
    bool initialized;
    TextFile currentTextFile;
    Font font;
    int font_size;
    int font_spacing;
} Editor;

void InitializeEditor();

#endif // Editor_H