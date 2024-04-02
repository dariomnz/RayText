#ifndef Command_H
#define Command_H

#include "Common.hpp"
#include "Editor.hpp"

void Command_Consume(Editor *editor);

void Command_Logic(Editor *editor);
void Command_Draw(Editor *editor);

#endif // Command_H