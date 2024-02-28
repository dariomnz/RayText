#ifndef Command_H
#define Command_H

#include "Common.h"
#include "Directory.h"
#include "Editor.h"

void Command_InsertChar(void);
void Command_RemoveChar(void);
void Command_Consume(void);

void Command_Logic(void);
void Command_Draw(void);
void Command_Free(void);

#endif // Command_H