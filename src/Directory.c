#include "Directory.h"
#include "DArray.h"

extern Editor editor;

void Directory_Update(const char *dir_name)
{
    if (strlen(editor.currentDirectory.name) == 0)
    {
        const char *cwd;
        cwd = GetWorkingDirectory();
        sprintf(editor.currentDirectory.name, "%s", cwd);
    }

    const char *aux;
    char aux2[MAX_PATH - 1];
    if (strcmp(dir_name, "..") == 0)
    {
        aux = GetDirectoryPath(editor.currentDirectory.name);
        sprintf(editor.currentDirectory.name, "%s", aux);
    }
    else if (strcmp(dir_name, ".") == 0)
    {
        return;
    }
    else
    {
        strncpy(aux2, editor.currentDirectory.name, 1023);
        sprintf(editor.currentDirectory.name, "%s" PATH_SEPARATOR "%s", aux2, dir_name);
    }
}

void Directory_Load(const char *dir_name)
{
    size_t i = 0, j = 0, k = 0;

    editor.currentDirectory.selected = 0;
    DArray_remove_from(&editor.currentDirectory, 0);

    Directory_Update(dir_name);
    DEBUG("Opening dir: %s", editor.currentDirectory.name);

    FilePathList filePathList = LoadDirectoryFiles(editor.currentDirectory.name);
    const char *aux;
    for (i = 0; i < filePathList.count; i++)
    {
        aux = GetFileName(filePathList.paths[i]);
        DArray_char *name = malloc(sizeof(DArray_char));
        memset(name, 0, sizeof(DArray_char));
        DArray_append_many(name, aux, strlen(aux));

        DArray_append(&editor.currentDirectory, name);
    }
    UnloadDirectoryFiles(filePathList);

    // Pre dir
    DArray_char *name_pre = malloc(sizeof(DArray_char));
    memset(name_pre, 0, sizeof(DArray_char));
    DArray_append_many(name_pre, "..", 2);
    DArray_insert(&editor.currentDirectory, name_pre, 0);

    // Current dir
    DArray_char *name_current = malloc(sizeof(DArray_char));
    memset(name_current, 0, sizeof(DArray_char));
    DArray_append_many(name_current, ".", 1);
    DArray_insert(&editor.currentDirectory, name_current, 0);

    // Sorting files alphabetically
    DArray_char *tmp;
    for (k = 0; k < editor.currentDirectory.count; k++)
        for (j = k + 1; j < editor.currentDirectory.count; j++)
        {
            if (strcasecmp(editor.currentDirectory.items[k]->items, editor.currentDirectory.items[j]->items) > 0)
            {
                tmp = editor.currentDirectory.items[k];
                editor.currentDirectory.items[k] = editor.currentDirectory.items[j];
                editor.currentDirectory.items[j] = tmp;
            }
        }
}

Vector2 Directory_GetCursorPosition()
{
    return (Vector2){0, editor.currentDirectory.selected * editor.font_size};
}

void Directory_Logic()
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
            Directory_Load(".");
        }
    }

    if (editor.editor_state != STATE_DIRECTORY)
        return;

    if (editor.key_pressed == KEY_DOWN)
    {
        if (editor.currentDirectory.selected + 1 < editor.currentDirectory.count)
        {
            editor.currentDirectory.selected++;
        }
    }
    if (editor.key_pressed == KEY_UP)
    {
        if (editor.currentDirectory.selected != 0)
        {
            editor.currentDirectory.selected--;
        }
    }
    if (editor.key_pressed == KEY_ENTER)
    {
        char aux[MAX_PATH + MAX_PATH];
        snprintf(aux, MAX_PATH + MAX_PATH, "%s" PATH_SEPARATOR "%s", editor.currentDirectory.name, editor.currentDirectory.items[editor.currentDirectory.selected]->items);
        if (IsPathFile(aux))
        {
            TextFile_Free();
            editor.currentTextFile = TextFile_Load(aux);
            editor.editor_state = STATE_TEXTFILE;
        }
        else
        {
            Directory_Load(editor.currentDirectory.items[editor.currentDirectory.selected]->items);
        }
    }

    editor.cursor_pos = Directory_GetCursorPosition();
}

void Directory_Draw()
{

    if (editor.editor_state != STATE_DIRECTORY)
        return;
    Color color = RED;
    for (int a = 0; a < editor.currentDirectory.count; a++)
    {
        char aux[MAX_PATH + MAX_PATH];
        snprintf(aux, MAX_PATH + MAX_PATH, "%s" PATH_SEPARATOR "%s", editor.currentDirectory.name, editor.currentDirectory.items[a]->items);
        if (IsPathFile(aux))
            color = WHITE;
        else
            color = BLUE;
        Vector2 pos = (Vector2){0, editor.font_size * a};
        if (a == editor.currentDirectory.selected)
        {
            Vector2 back_size = MeasureTextEx(editor.font, editor.currentDirectory.items[a]->items, editor.font_size, editor.font_spacing);
            DrawRectangleV(pos, back_size, DARKGRAY);
        }
        DrawTextEx(editor.font, editor.currentDirectory.items[a]->items, pos, editor.font_size, editor.font_spacing, color);
    }
}

void Directory_Free()
{
    Directory *directory = &editor.currentDirectory;
    for (size_t i = 0; i < directory->count; i++)
    {
        DArray_free(directory->items[i]);
        free(directory->items[i]);
    }

    DArray_free(directory);
}