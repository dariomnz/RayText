#include "Common.h"
#include "Editor.h"
#include "DArray.h"

// Global variables
extern Editor editor;

struct DArray_int
{
    int *items;      // Array of elements can have any type
    size_t count;    // Count of current elements
    size_t capacity; // Capacity of current items in memory
};
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    struct DArray_int array = {0};
    DArray_append(&array, 1);
    DArray_append(&array, 2);
    DArray_append(&array, 3);
    DArray_append(&array, 4);
    // exit(0);
    DArray_append(&array, 5);
    DArray_insert(&array, 10, 0);
    for (size_t i = 0; i < 10; i++)
    {
        DArray_insert(&array, i, 2);
    }

    for (size_t i = 0; i < array.count; i++)
    {
        printf("Item i %ld: %d\n", i, array.items[i]);
    }

    printf("----------------------\n");
    DArray_remove(&array, 0);
    for (size_t i = 0; i < array.count; i++)
    {
        printf("Item i %ld: %d\n", i, array.items[i]);
    }
    printf("----------------------\n");
    DArray_remove_from(&array, 3);
    for (size_t i = 0; i < array.count; i++)
    {
        printf("Item i %ld: %d\n", i, array.items[i]);
    }

    DArray_free(&array);
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