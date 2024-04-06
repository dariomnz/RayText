#include "Directories.hpp"
#include "TextFile.hpp"
#include "Editor.hpp"
#include <algorithm>

using namespace std;

Directories::Directories()
{
    DEBUG_MSG("Construct Directories");
}

Directories::~Directories()
{
    DEBUG_MSG("delete Directories");
}

bool sort_function(Entry i, Entry j) { return (i.name.compare(j.name) < 0); }

void Directories::Update(string dir_name)
{
    if (name.empty())
    {
        name += GetWorkingDirectory();
    }

    if (dir_name.compare("..") == 0)
    {
        name.clear();
        name += GetDirectoryPath(name.c_str());
    }
    else if (dir_name.compare(".") == 0)
    {
        return;
    }
    else
    {
        name += PATH_SEPARATOR;
        name += dir_name;
    }
}

void Directories::Load(string dir_name)
{
    size_t i = 0;
    selected = 0;
    items.clear();

    Update(dir_name);
    DEBUG_MSG("Opening dir: " << name);

    // Current dir
    Entry entry_current = {};
    entry_current.name = ".";
    entry_current.is_file = false;
    items.push_back(entry_current);

    // Pre dir
    Entry entry_pre = {};
    entry_pre.name = "..";
    entry_pre.is_file = false;
    items.push_back(entry_pre);

    FilePathList filePathList = LoadDirectoryFiles(name.c_str());
    for (i = 0; i < filePathList.count; i++)
    {
        Entry entry = {};
        entry.name = GetFileName(filePathList.paths[i]);
        entry.is_file = IsPathFile(filePathList.paths[i]);
        items.push_back(entry);
    }
    UnloadDirectoryFiles(filePathList);

    // Sorting files alphabetically
    std::sort(items.begin(), items.end(), sort_function);
}

void Directories::Logic(Editor &editor)
{
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_B))
    {
        if (editor.editor_state == STATE_DIRECTORY)
        {
            editor.editor_state = STATE_TEXTFILE;
        }
        else
        {
            editor.editor_state = STATE_DIRECTORY;
            Load(".");
        }
    }

    if (editor.editor_state != STATE_DIRECTORY)
        return;

    if (editor.key_pressed == KEY_ENTER)
    {
        string aux = editor.currentDirectory.name;
        aux += PATH_SEPARATOR;
        aux += editor.currentDirectory.items[editor.currentDirectory.selected].name;
        if (editor.currentDirectory.items[editor.currentDirectory.selected].is_file)
        {
            editor.GetCurrentTextFile().Free();
            editor.GetCurrentTextFile().Load(aux);
            editor.editor_state = STATE_TEXTFILE;
        }
        else
        {
            Load(editor.currentDirectory.items[editor.currentDirectory.selected].name);
        }
    }

    editor.cursor_pos = (Vector2){0, (float)editor.currentDirectory.selected * FONT_SIZE};
}

void Directories::Draw(Editor &editor)
{

    if (editor.editor_state != STATE_DIRECTORY)
        return;
    Color color = RED;
    for (int a = 0; a < (int)editor.currentDirectory.items.size(); a++)
    {
        if (editor.currentDirectory.items[a].is_file)
            color = WHITE;
        else
            color = BLUE;
        Vector2 pos = (Vector2){0, (float)FONT_SIZE * a};
        DrawTextEx(editor.font, editor.currentDirectory.items[a].name.c_str(), pos, FONT_SIZE, FONT_SPACING, color);
    }
}