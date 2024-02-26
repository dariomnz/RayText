#include "Common.h"
#include "Editor.h"
#include "DArray.h"

// Global variables
extern Editor editor;

struct DArray_int
{
    char *items;      // Array of elements can have any type
    size_t count;    // Count of current elements
    size_t capacity; // Capacity of current items in memory
};
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    struct DArray_int array = {0};
    darray_append(&array, 'H');
    darray_append(&array, 'o');
    darray_append(&array, 'l');
    darray_append(&array, 'a');
    printf("Text: %s\n",array.items);
    // exit(0);
    darray_append(&array, 5);
    darray_insert(&array, 10, 0);
    for (size_t i = 0; i < 10; i++)
    {
        darray_insert(&array, i,2);
    }

    for (size_t i = 0; i < array.count; i++)
    {
        printf("Item i %ld: %c\n", i, array.items[i]);
    }

    printf("----------------------\n");
    darray_remove(&array, 0);
    for (size_t i = 0; i < array.count; i++)
    {
        printf("Item i %ld: %c\n", i, array.items[i]);
    }

    darray_free(&array);
    // exit(0);
    // Initialization
    //--------------------------------------------------------------------------------------
    Editor_Init();

    // SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose() && !editor.the_end) // Detect window close button or ESC key
    {
        //----------------------------------------------------------------------------------
        // Logic
        //----------------------------------------------------------------------------------
        Editor_Logic();
        //----------------------------------------------------------------------------------

        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        Editor_Draw();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    Editor_Close();
    //--------------------------------------------------------------------------------------
    return 0;
}