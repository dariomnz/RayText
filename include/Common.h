#ifndef Common_H
#define Common_H

#define STR_MAX_LEN 64

#define DEBUG(fmt, ...) fprintf(stderr, "[%s:%d] %s(): " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

/**
 * @param type type of the elemments in the array
 * @param ptr pointer to get the value of each elem in array
 * @param arr the array
 */
#define FOR_EACH(type, ptr, arr) for (type *ptr = arr; ptr < arr + sizeof(arr) / sizeof(arr[0]); ptr++)

#endif // Common_H