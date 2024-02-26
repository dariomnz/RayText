#ifndef DArray_H
#define DArray_H

#include <ctype.h>
#include <assert.h>
#include <stdlib.h>

typedef struct DArray_char
{
    char *items;     // Array of elements can have any type
    size_t count;    // Count of current elements
    size_t capacity; // Capacity of current items in memory
} DArray_char;

#define DArray_INIT_CAP 16
// Append an item to a dynamic array
#define DArray_append(da, item)                                                                          \
    do                                                                                                   \
    {                                                                                                    \
        if ((da)->count >= (da)->capacity)                                                               \
        {                                                                                                \
            (da)->capacity = (da)->capacity == 0 ? DArray_INIT_CAP : (da)->capacity * 2;                 \
            (da)->items = realloc((da)->items, (da)->capacity * sizeof(*(da)->items));                   \
            assert((da)->items != NULL && "Insuficient memory");                                         \
            memset((da)->items + (da)->count, 0, ((da)->capacity - (da)->count) * sizeof(*(da)->items)); \
        }                                                                                                \
                                                                                                         \
        (da)->items[(da)->count++] = (item);                                                             \
    } while (0)

// Append several items to a dynamic array
#define DArray_append_many(da, new_items, new_items_count)                                               \
    do                                                                                                   \
    {                                                                                                    \
        if ((da)->count + (new_items_count) > (da)->capacity)                                            \
        {                                                                                                \
            if ((da)->capacity == 0)                                                                     \
                (da)->capacity = DArray_INIT_CAP;                                                        \
            while ((da)->count + (new_items_count) > (da)->capacity)                                     \
                (da)->capacity *= 2;                                                                     \
            (da)->items = realloc((da)->items, (da)->capacity * sizeof(*(da)->items));                   \
            assert((da)->items != NULL && "Insuficient memory");                                         \
            memset((da)->items + (da)->count, 0, ((da)->capacity - (da)->count) * sizeof(*(da)->items)); \
        }                                                                                                \
        memcpy((da)->items + (da)->count, (new_items), (new_items_count) * sizeof(*(da)->items));        \
        (da)->count += (new_items_count);                                                                \
    } while (0)

// Insert an item to a dynamic array in an index
#define DArray_insert(da, item, index)                                                                               \
    do                                                                                                               \
    {                                                                                                                \
        if ((da)->count >= (da)->capacity)                                                                           \
        {                                                                                                            \
            (da)->capacity = (da)->capacity == 0 ? DArray_INIT_CAP : (da)->capacity * 2;                             \
            (da)->items = realloc((da)->items, (da)->capacity * sizeof(*(da)->items));                               \
            assert((da)->items != NULL && "Insuficient memory");                                                     \
            memset((da)->items + (da)->count, 0, ((da)->capacity - (da)->count) * sizeof(*(da)->items));             \
        }                                                                                                            \
        memmove((da)->items + ((index) + 1), (da)->items + (index), ((da)->count - (index)) * sizeof(*(da)->items)); \
        (da)->items[index] = (item);                                                                                 \
        (da)->count++;                                                                                               \
    } while (0)

// Remove an item of a dynamic array in the index
#define DArray_remove(da, index)                                                                                     \
    do                                                                                                               \
    {                                                                                                                \
        if ((index) >= (da)->count)                                                                                  \
        {                                                                                                            \
            DEBUG("Error: remove of index %d from darray of count %d", (int)(index), (int)(da)->count);              \
            break;                                                                                                   \
        }                                                                                                            \
        memmove((da)->items + (index), (da)->items + ((index) + 1), ((da)->count - (index)) * sizeof(*(da)->items)); \
        (da)->count--;                                                                                               \
        memset((da)->items + ((da)->count), 0, ((da)->capacity - (da)->count) * sizeof(*(da)->items));               \
    } while (0)

// Remove to of items from a dynamic array starting from index
#define DArray_remove_from(da, index)                                                                     \
    do                                                                                                    \
    {                                                                                                     \
        if ((index) >= (da)->count)                                                                       \
        {                                                                                                 \
            DEBUG("Error: remove from index %d from darray of count %d", (int)(index), (int)(da)->count); \
            break;                                                                                        \
        }                                                                                                 \
        memset((da)->items + (index), 0, ((da)->count - (index)) * sizeof(*(da)->items));                 \
        (da)->count -= ((da)->count - (index));                                                           \
    } while (0)

// Remove count of items from a dynamic array starting from index
#define DArray_remove_many(da, index, how_many)                                                                                            \
    do                                                                                                                                     \
    {                                                                                                                                      \
        if (((index) + (how_many)) >= (da)->count)                                                                                         \
        {                                                                                                                                  \
            DEBUG("Error: remove of index %d count %d from darray of count %d", (int)(index), (int)(how_many), (int)(da)->count);          \
            break;                                                                                                                         \
        }                                                                                                                                  \
        memmove((da)->items + (index), (da)->items + ((index) + (how_many)), ((da)->count - (index) + (how_many)) * sizeof(*(da)->items)); \
        (da)->count -= (how_many);                                                                                                         \
    } while (0)

#define DArray_free(da)            \
    do                             \
    {                              \
        if (((da)->capacity) == 0) \
        {                          \
            break;                 \
        }                          \
        free((da)->items);         \
    } while (0)
#endif // DArray_H