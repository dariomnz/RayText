#ifndef DArray_H
#define DArray_H

#include <ctype.h>
#include <assert.h>

typedef struct DArray
{
    void *items;     // Array of elements can have any type
    size_t count;    // Count of current elements
    size_t capacity; // Capacity of current items in memory
} DArray;

#define DArray_INIT_CAP 16
// Append an item to a dynamic array
#define darray_append(da, item)                                                                          \
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

// Append an item to a dynamic array
#define darray_insert(da, item, index)                                                                           \
    do                                                                                                           \
    {                                                                                                            \
        if ((da)->count >= (da)->capacity)                                                                       \
        {                                                                                                        \
            (da)->capacity = (da)->capacity == 0 ? DArray_INIT_CAP : (da)->capacity * 2;                         \
            (da)->items = realloc((da)->items, (da)->capacity * sizeof(*(da)->items));                           \
            assert((da)->items != NULL && "Insuficient memory");                                                 \
            memset((da)->items + (da)->count, 0, ((da)->capacity - (da)->count) * sizeof(*(da)->items));         \
        }                                                                                                        \
        memmove((da)->items + (index + 1), (da)->items + (index), ((da)->count - index) * sizeof(*(da)->items)); \
        (da)->items[index] = (item);                                                                             \
        (da)->count++;                                                                                           \
    } while (0)

// Append an item to a dynamic array
#define darray_remove(da, index)                                                                                 \
    do                                                                                                           \
    {                                                                                                            \
        memmove((da)->items + (index), (da)->items + (index + 1), ((da)->count - index) * sizeof(*(da)->items)); \
        (da)->count--;                                                                                           \
    } while (0)

// Append several items to a dynamic array
#define darray_append_many(da, new_items, new_items_count)                                        \
    do                                                                                            \
    {                                                                                             \
        if ((da)->count + (new_items_count) > (da)->capacity)                                     \
        {                                                                                         \
            if ((da)->capacity == 0)                                                              \
                (da)->capacity = DArray_INIT_CAP;                                                 \
            while ((da)->count + (new_items_count) > (da)->capacity)                              \
                (da)->capacity *= 2;                                                              \
            (da)->items = realloc((da)->items, (da)->capacity * sizeof(*(da)->items));            \
            assert((da)->items != NULL && "Insuficient memory");                                  \
        }                                                                                         \
        memcpy((da)->items + (da)->count, (new_items), (new_items_count) * sizeof(*(da)->items)); \
        (da)->count += (new_items_count);                                                         \
    } while (0)

#define darray_free(da) \
    free((da)->items)
#endif // DArray_H