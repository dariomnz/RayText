#include "Directory.hpp"
#include <string>
#include <algorithm>

using namespace std;

bool sort_function(Directory i, Directory j) { return (i.name.compare(j.name) < 0); }

void Directory_Update(Directories *directories, string dir_name)
{
    if (directories->name.empty())
    {
        const char *cwd;
        cwd = GetWorkingDirectory();
        directories->name += cwd;
    }

    const char *aux;
    if (dir_name.compare("..") == 0)
    {
        aux = GetDirectoryPath(directories->name.c_str());
        directories->name.clear();
        directories->name += aux;
    }
    else if (dir_name.compare(".") == 0)
    {
        return;
    }
    else
    {
        directories->name += PATH_SEPARATOR;
        directories->name += dir_name;
    }
}

void Directory_Load(Directories *directories, string dir_name)
{
    size_t i = 0, j = 0, k = 0;

    directories->selected = 0;
    directories->items.clear();

    Directory_Update(directories, dir_name);
    DEBUG_MSG("Opening dir: " << directories->name);

    // Pre dir
    Directory directory_pre = {};
    directory_pre.name = ".";
    directory_pre.is_file = false;
    directories->items.push_back(directory_pre);

    // Current dir
    Directory directory_current = {};
    directory_current.name = "..";
    directory_current.is_file = false;
    directories->items.push_back(directory_current);

    FilePathList filePathList = LoadDirectoryFiles(directories->name.c_str());
    for (i = 0; i < filePathList.count; i++)
    {
        Directory directory = {};
        directory.name = GetFileName(filePathList.paths[i]);
        directory.is_file = IsPathFile(filePathList.paths[i]);
        directories->items.push_back(directory);
    }
    UnloadDirectoryFiles(filePathList);

    // Sorting files alphabetically
    std::sort(directories->items.begin(), directories->items.end(), sort_function);
}

void Directory_Logic(Editor *editor)
{
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_B))
    {
        if (editor->editor_state == STATE_DIRECTORY)
        {
            editor->editor_state = STATE_TEXTFILE;
        }
        else
        {
            editor->editor_state = STATE_DIRECTORY;
            Directory_Load(&editor->currentDirectory, (string) ".");
        }
    }

    if (editor->editor_state != STATE_DIRECTORY)
        return;

    if (editor->key_pressed == KEY_ENTER)
    {
        string aux = editor->currentDirectory.name;
        aux += PATH_SEPARATOR;
        aux += editor->currentDirectory.items[editor->currentDirectory.selected].name;
        if (editor->currentDirectory.items[editor->currentDirectory.selected].is_file)
        {
            TextFile_Free(&editor->currentTextFile);
            editor->currentTextFile = TextFile_Load(aux);
            editor->editor_state = STATE_TEXTFILE;
        }
        else
        {
            Directory_Load(&editor->currentDirectory, editor->currentDirectory.items[editor->currentDirectory.selected].name);
        }
    }

    editor->cursor_pos = (Vector2){0, editor->currentDirectory.selected * FONT_SIZE};
}

void Directory_Draw(Editor *editor)
{

    if (editor->editor_state != STATE_DIRECTORY)
        return;
    Color color = RED;
    for (int a = 0; a < (int)editor->currentDirectory.items.size(); a++)
    {
        if (editor->currentDirectory.items[a].is_file)
            color = WHITE;
        else
            color = BLUE;
        Vector2 pos = (Vector2){0, FONT_SIZE * a};
        DrawTextEx(editor->font, editor->currentDirectory.items[a].name.c_str(), pos, FONT_SIZE, FONT_SPACING, color);
    }
}

void Directory_Free(Directories *directories)
{
    directories->items.clear();
    // for (size_t i = 0; i < directories->count; i++)
    // {
    //     DArray_free(directories->items[i]);
    //     free(directories->items[i]);
    // }

    // DArray_free(directories);
}