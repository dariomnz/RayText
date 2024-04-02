#ifndef Editor_H
#define Editor_H

#include "Common.hpp"
#include "Structs.hpp"
#include "TextFile.hpp"
#include "Command.hpp"
#include "Directory.hpp"
#include "Cursor.hpp"

void Editor_Init(Editor *editor);

void Editor_Logic(Editor *editor);
void Editor_Draw(Editor *editor);

void Editor_Close(Editor *editor);

#endif // Editor_H