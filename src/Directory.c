#include "Directory.h"
#include "DArray.h"

void Directory_Update(Directories *directories, Directory *dir_name)
{
    if (directories->name.count == 0)
    {
        const char *cwd;
        cwd = GetWorkingDirectory();
        DArray_append_many(&directories->name, cwd, strlen(cwd));
    }

    const char *aux;
    if (strcmp(dir_name->items, "..") == 0)
    {
        aux = GetDirectoryPath(directories->name.items);
        DArray_clear(&directories->name);
        DArray_append_many(&directories->name, aux, strlen(aux));
    }
    else if (strcmp(dir_name->items, ".") == 0)
    {
        return;
    }
    else
    {
        DArray_append_many(&directories->name, PATH_SEPARATOR, 1);
        DArray_append_many(&directories->name, dir_name->items, dir_name->count);
    }
}

void Directory_Load(Directories *directories, Directory *dir_name)
{
    size_t i = 0, j = 0, k = 0;

    directories->selected = 0;
    DArray_clear(directories);

    Directory_Update(directories, dir_name);
    DEBUG("Opening dir: %s", directories->name.items);

    FilePathList filePathList = LoadDirectoryFiles(directories->name.items);
    const char *aux;
    for (i = 0; i < filePathList.count; i++)
    {
        aux = GetFileName(filePathList.paths[i]);
        Directory *directory = malloc(sizeof(Directory));
        memset(directory, 0, sizeof(Directory));
        DArray_append_many(directory, aux, strlen(aux));
        directory->is_file = IsPathFile(filePathList.paths[i]);

        DArray_append(directories, directory);
    }
    UnloadDirectoryFiles(filePathList);

    // Pre dir
    Directory *directory_pre = malloc(sizeof(Directory));
    memset(directory_pre, 0, sizeof(Directory));
    DArray_append_many(directory_pre, "..", 3);
    directory_pre->is_file = false;
    DArray_insert(directories, directory_pre, 0);

    // Current dir
    Directory *directory_current = malloc(sizeof(Directory));
    memset(directory_current, 0, sizeof(Directory));
    DArray_append_many(directory_current, ".", 2);
    directory_current->is_file = false;
    DArray_insert(directories, directory_current, 0);

    // Sorting files alphabetically
    Directory *tmp;
    for (k = 0; k < directories->count; k++)
        for (j = k + 1; j < directories->count; j++)
        {
            if (strcasecmp(directories->items[k]->items, directories->items[j]->items) > 0)
            {
                tmp = directories->items[k];
                directories->items[k] = directories->items[j];
                directories->items[j] = tmp;
            }
        }
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
            Directory aux = {0};
            DArray_append(&aux, '.');
            Directory_Load(&editor->currentDirectory, &aux);
            DArray_free(&aux);
        }
    }

    if (editor->editor_state != STATE_DIRECTORY)
        return;

    if (editor->key_pressed == KEY_ENTER)
    {
        DArray_char aux = {0};
        DArray_append_many(&aux, editor->currentDirectory.name.items, editor->currentDirectory.name.count);
        DArray_append_many(&aux, PATH_SEPARATOR, 1);
        DArray_append_many(&aux, editor->currentDirectory.items[editor->currentDirectory.selected]->items, editor->currentDirectory.items[editor->currentDirectory.selected]->count);
        if (editor->currentDirectory.items[editor->currentDirectory.selected]->is_file)
        {
            TextFile_Free(&editor->currentTextFile);
            editor->currentTextFile = TextFile_Load(&aux);
            editor->editor_state = STATE_TEXTFILE;
        }
        else
        {
            Directory_Load(&editor->currentDirectory, editor->currentDirectory.items[editor->currentDirectory.selected]);
        }
        DArray_free(&aux);
    }

    editor->cursor_pos = (Vector2){0, editor->currentDirectory.selected * FONT_SIZE};
}

void Directory_Draw(Editor *editor)
{

    if (editor->editor_state != STATE_DIRECTORY)
        return;
    Color color = RED;
    for (int a = 0; a < editor->currentDirectory.count; a++)
    {
        if (editor->currentDirectory.items[a]->is_file)
            color = WHITE;
        else
            color = BLUE;
        Vector2 pos = (Vector2){0, FONT_SIZE * a};
        DrawTextEx(editor->font, editor->currentDirectory.items[a]->items, pos, FONT_SIZE, FONT_SPACING, color);
    }
}

void Directory_Free(Directories *directories)
{
    for (size_t i = 0; i < directories->count; i++)
    {
        DArray_free(directories->items[i]);
        free(directories->items[i]);
    }

    DArray_free(directories);
}