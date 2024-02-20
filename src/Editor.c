#include "Editor.h"

Editor editor = {0};
void InitializeEditor()
{
    if (editor.initialized != 0)
        return;
    editor.font_size = 20;
}