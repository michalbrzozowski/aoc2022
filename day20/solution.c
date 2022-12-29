#include <memory.h>
#include "..\utilities.h"

#define input_file "input"

i64 decryption_key = 811589153;

// #define max_numbers 7
#define max_numbers 5000

static int numbers;
static int indices[max_numbers];
static i64 coordinates[max_numbers];

void parse_input()
{
    numbers = 0;
    char** input = aquire_input(input_file);
    for (int i = 0; i < list_length(input); ++i) {
        coordinates[numbers++] = atoi(input[i]);
        indices[i] = i;
    }
    release_input(input);
}

int mod(i64 x, int n)
{
    return (x % n + n) % n;
}

int mix()
{
    int head, tail, zero;
    head = tail = 0;
    zero = -1;

    for (int i = 0; i < numbers; ++i) {
        i64 value = coordinates[i];
        zero = value == 0 ? i : zero;

        tail = 0;
        while (indices[tail] != i) tail++;

        head = mod(tail + value % (numbers - 1), numbers);

        if (head > tail) {
            if (value < 0)
                head--;
            memmove(indices + tail, indices + tail + 1, (head - tail) * sizeof(int));
        }
        else if (tail > head) {
            if (value > 0) 
                head++;
            memmove(indices + head + 1, indices + head, (tail - head) * sizeof(int));
        }
        indices[head] = i;
    }
    return zero;
}

i64 find_coordinates(int zero)
{
    int start = 0;
    while (indices[start] != zero) start++;

    i64 number = coordinates[indices[(start + 1000) % numbers]];
    i64 answer = number;

    number = coordinates[indices[(start + 2000) % numbers]];
    answer += number;

    number = coordinates[indices[(start + 3000) % numbers]];
    answer += number;
    return answer;
}

int part_one()
{
    parse_input();
    return mix();
}

i64 part_two()
{
    parse_input();

    for (int i = 0; i < numbers; ++i) {
        coordinates[i] *= decryption_key;
    }

    int zero = -1;
    for (int i = 0; i < 10; ++i) {
        zero = mix();
    }
    return find_coordinates(zero);
}

void main()
{
    printf("%d\n", part_one());
    printf("%"PRIi64"\n", part_two());
}