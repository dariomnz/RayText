#ifndef Directory_H
#define Directory_H

#include "Common.hpp"
#include "Structs.hpp"
#include "Editor.hpp"
#include "raymath.h"
#include <string>

void Directory_Load(Directories *directories, std::string dir_name);

void Directory_Logic(Editor *editor);
void Directory_Draw(Editor *editor);
void Directory_Free(Directories *directories);

#endif // Directory_H