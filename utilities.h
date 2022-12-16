#pragma once

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <inttypes.h>

typedef uint8_t     byte;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

#define set_bit(value, n) ((value) |= ((u64)1<<(n)))
#define clear_bit(value, n) ((value) &= ~((u64)1<<(n)))
#define flip_bit(value, n) ((value) ^= (1<<(n)))
#define check_bit(value, n) ((value) & (1<<(n)))

#define set_bitmask(value, mask) ((value) |= (mask))
#define clear_bitmask(value, mask) ((value) &= (~(mask)))
#define flip_bitmask(value, mask) ((value) ^= (mask))
#define check_bitmask(value, mask) (((value) & (mask)) == (mask))

#define __clamp(value, max, min) (__min(max, __max(value, min)))
#define __clamp_min(x, min) __max(x, min)
#define __clamp_max(x, max) __min(x, max)

#define null_ptr (void*)0
#define unused(arg) (void) (arg);
#define num_elements(array) (sizeof((array)) / (sizeof(array[0])))

#if 0
#include <windows.h>

static u64 hp_counter()
{
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return counter.QuadPart;
}

static u64 hp_frequency()
{
    static LARGE_INTEGER frequency = {0};
    if (frequency.QuadPart == 0)
        QueryPerformanceFrequency(&frequency);
    return frequency.QuadPart;
}

static double hp_elapsed(u64 start, u64 stop)
{
    return (stop - start) / (double)hp_frequency();
}
#endif


void print_bits(u64 val, size_t size)
{
    char bits[65] = { 0 };

    size_t i = size * 8;
    assert(i < 65 && "size is to large to fit in string");
    do {
        bits[--i] = (val & 1 ? '1' : '0');
        val >>= 1;
    } while (i != 0);

    printf("%s\n", bits);
}

/*
    list implementation based heavily on the stretchy buffers in the bitwise
    project (https://github.com/pervognsen/bitwise).
*/

typedef struct list_header {
    size_t length;
    size_t capacity;
    char data[];
} list_header;

#define list_ptr(l) ((list_header *)((char *)(l) - offsetof(list_header, data)))

#define list_length(l) ((l) ? list_ptr(l)->length : 0)
#define list_capacity(l) ((l) ? list_ptr(l)->capacity : 0)
#define list_end(l) ((l) + list_length(l))
#define list_sizeof(l) ((l) ? list_length(l)*sizeof(*l) : 0)

#define list_free(l) ((l) ? (free(list_ptr(l)), (l) = null_ptr) : 0)
#define list_fit(l, n) ((n) <= list_capacity(l) ? 0 : ((l) = list__grow((l), (n), sizeof(*(l)))))
#define list_push(l, ...) (list_fit((l), 1 + list_length(l)), (l)[list_ptr(l)->length++] = (__VA_ARGS__))
#define buf_pop(l) ((l)[--list_ptr(v)->length])
#define list_printf(l, ...) ((l) = list__printf((l), __VA_ARGS__))
#define list_clear(l) ((l) ? list_ptr(l)->length = 0 : 0)

void *list__grow(const void *data, size_t new_length, size_t element_size)
{
    size_t header_size = offsetof(list_header, data);
    size_t max_capacity = (SIZE_MAX - header_size) / element_size;
    size_t new_capacity = __clamp_min(2 * list_capacity(data), __max(new_length, 16));
    
    if (new_capacity > max_capacity)
        goto failure;

    size_t new_size = header_size + new_capacity * element_size;
    list_header *new_header;

    if (data) {
        new_header = realloc(list_ptr(data), new_size);
        if (!new_header) goto failure;
    } else {
        new_header = malloc(new_size);
        if (!new_header) goto failure;
        new_header->length = 0;
    }

    new_header->capacity = new_capacity;
    return new_header->data;

failure:
    fprintf(stderr, "error while growing list\n");
    abort();
    return null_ptr;
}

char *list__printf(char *data, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    size_t capacity = list_capacity(data) - list_length(data);
    size_t n = 1 + vsnprintf(list_end(data), capacity, fmt, args);
    va_end(args);
    if (n > capacity) {
        list_fit(data, n + list_length(data));
        va_start(args, fmt);
        size_t new_capacity= list_capacity(data) - list_length(data);
        n = 1 + vsnprintf(list_end(data), new_capacity, fmt, args);
        assert(n <= new_capacity);
        va_end(args);
    }
    list_ptr(data)->length += n - 1;
    return data;
}

/*
    quick and dirty input handling
*/

char** aquire_input(const char* filename)
{
    FILE* file = null_ptr;
    char** input = null_ptr;
    assert(fopen_s(&file, filename, "rb") == 0);

    char line[4096];
    while (fgets(line, 4096, file) != null_ptr) {
        int i = 0;
        while (line[i] != '\n' && line[i] != '\r' && line[i] != '\0') i++;
        line[i] = 0;

        char* row = null_ptr;
        list_printf(row, line);
        list_push(input, row);
    }
    fclose(file);
    return input;
}

void release_input(char** input)
{
    for (const char** it = input; it != list_end(input); it++)
        list_free(*it);
    list_free(input);
}

