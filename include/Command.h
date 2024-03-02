#ifndef Command_H
#define Command_H

#include "Common.h"
#include "Directory.h"
#include "Editor.h"

void Command_InsertChar(DArray_char *command, char c);
void Command_RemoveChar(DArray_char *command);
void Command_Consume(Editor *editor);

void Command_Logic(Editor *editor);
void Command_Draw(Editor *editor);
void Command_Free(DArray_char *command);

#endif // Command_H