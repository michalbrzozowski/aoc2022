#include "..\utilities.h"

#include <intrin.h>

#define input_file "input"

int part_one()
{
    int answer = 0;
    char** input = aquire_input(input_file);

    for (int i = 0; i < list_length(input[0]) - 3; ++i) {
        u64 marker = 0;
        set_bit(marker, input[0][i] - 'a');
        set_bit(marker, input[0][i + 1] - 'a');
        set_bit(marker, input[0][i + 2] - 'a');
        set_bit(marker, input[0][i + 3] - 'a');
        if (__popcnt64(marker) == 4) {
            answer = i + 4;
            break;
        }
    }

    release_input(input);
    return answer;
}

int part_two()
{
    int answer = 0;
    char** input = aquire_input(input_file);

    for (int i = 0; i < list_length(input[0]) - 13; ++i) {
        u64 marker = 0;
        for (int j = 0; j < 14; ++j)
            set_bit(marker, input[0][i + j] - 'a');

        if (__popcnt64(marker) == 14) {
            answer = i + 14;
            break;
        }
    }

    release_input(input);
    return answer;
}

void main()
{
    printf("%d\n", part_one());
    printf("%d\n", part_two());
}