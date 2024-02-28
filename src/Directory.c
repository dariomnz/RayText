#include "Directory.h"
#include "DArray.h"

extern Editor editor;

void Directory_Update(DArray_char *dir_name)
{
    if (editor.currentDirectory.name.count == 0)
    {
        const char *cwd;
        cwd = GetWorkingDirectory();
        DArray_append_many(&editor.currentDirectory.name, cwd, strlen(cwd));
    }

    const char *aux;
    if (strcmp(dir_name->items, "..") == 0)
    {
        aux = GetDirectoryPath(editor.currentDirectory.name.items);
        DArray_clear(&editor.currentDirectory.name);
        DArray_append_many(&editor.currentDirectory.name, aux, strlen(aux));
    }
    else if (strcmp(dir_name->items, ".") == 0)
    {
        return;
    }
    else
    {
        DArray_append_many(&editor.currentDirectory.name, PATH_SEPARATOR, 1);
        DArray_append_many(&editor.currentDirectory.name, dir_name->items, dir_name->count);
    }
}

void Directory_Load(DArray_char *dir_name)
{
    size_t i = 0, j = 0, k = 0;

    editor.currentDirectory.selected = 0;
    DArray_clear(&editor.currentDirectory);

    Directory_Update(dir_name);
    DEBUG("Opening dir: %s", editor.currentDirectory.name.items);

    FilePathList filePathList = LoadDirectoryFiles(editor.currentDirectory.name.items);
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

Vector2 Directory_GetCursorPosition(void)
{
    return (Vector2){0, editor.currentDirectory.selected * editor.font_size};
}

void Directory_Logic(void)
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
            DArray_char aux = {0};
            DArray_append(&aux, '.');
            Directory_Load(&aux);
            DArray_free(&aux);
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
        DArray_char aux = {0};
        DArray_append_many(&aux, editor.currentDirectory.name.items, editor.currentDirectory.name.count);
        DArray_append_many(&aux, PATH_SEPARATOR, 1);
        DArray_append_many(&aux, editor.currentDirectory.items[editor.currentDirectory.selected]->items, editor.currentDirectory.items[editor.currentDirectory.selected]->count);
        if (IsPathFile(aux.items))
        {
            TextFile_Free();
            editor.currentTextFile = TextFile_Load(&aux);
            editor.editor_state = STATE_TEXTFILE;
        }
        else
        {
            Directory_Load(editor.currentDirectory.items[editor.currentDirectory.selected]);
        }
        DArray_free(&aux);
    }

    editor.cursor_pos = Directory_GetCursorPosition();
}

void Directory_Draw(void)
{

    if (editor.editor_state != STATE_DIRECTORY)
        return;
    Color color = RED;
    DArray_char aux = {0};
    for (int a = 0; a < editor.currentDirectory.count; a++)
    {
        DArray_append_many(&aux, editor.currentDirectory.name.items, editor.currentDirectory.name.count);
        DArray_append_many(&aux, PATH_SEPARATOR, 1);
        DArray_append_many(&aux, editor.currentDirectory.items[a]->items, editor.currentDirectory.items[a]->count);
        if (IsPathFile(aux.items))
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
        DArray_clear(&aux);
    }
    DArray_free(&aux);
}

void Directory_Free(void)
{
    Directory *directory = &editor.currentDirectory;
    for (size_t i = 0; i < directory->count; i++)
    {
        DArray_free(directory->items[i]);
        free(directory->items[i]);
    }

    DArray_free(directory);
}