#ifndef Editor_H
#define Editor_H

#include "Common.h"
#include "Structs.h"
#include "TextFile.h"
#include "Command.h"
#include "Directory.h"
#include "Cursor.h"

void Editor_Init(Editor *editor);

void Editor_Logic(Editor *editor);
void Editor_Draw(Editor *editor);

void Editor_Close(Editor *editor);

#endif // Editor_H