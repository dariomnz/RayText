#ifndef Command_H
#define Command_H

#include "Common.h"
#include "Directory.h"
#include "Editor.h"

void Command_InsertChar();
void Command_RemoveChar();
void Command_Consume();

void Command_Logic();
void Command_Draw();

#endif // Command_H