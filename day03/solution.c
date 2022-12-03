#include "..\utilities.h"

#include <intrin.h>

#define input_file "input"

int part_one()
{
    int answer = 0;
    char** input = aquire_input(input_file);

    for (int i = 0; i < list_length(input); ++i) {
        int backpack_size = (int)list_length(input[i]);
        int middle = backpack_size / 2;
        u64 compartment_one = 0;
        u64 compartment_two = 0;

        for (int j = 0; j < middle; ++j) {
            int n = input[i][j] - 'A';
            set_bit(compartment_one, n);
        }
        for (int k = middle; k < backpack_size; ++k) {
            int n = input[i][k] - 'A';
            set_bit(compartment_two, n);
        }
        u32 count = 0;
        assert(_BitScanReverse64(&count, compartment_one & compartment_two));
        char item = (char)count + 'A';
        answer += (item >= 'a' && item <= 'z') ? item - 'a' + 1 : item - 'A' + 27;
    }

    release_input(input);
    return answer;
}

int part_two()
{
    int answer = 0;
    char** input = aquire_input(input_file);

    for (int i = 0; i < list_length(input); i += 3) {
        u64 backpack[3] = { 0 };

        for (int step = 0; step < 3; ++step) {
            for (int j = 0; j < list_length(input[i + step]); ++j) {
                set_bit(backpack[step], input[i + step][j] - 'A');
            }
        }

        u32 count = 0;
        assert(_BitScanReverse64(&count, backpack[0] & backpack[1] & backpack[2]));
        char item = (char)count + 'A';
        answer += (item >= 'a' && item <= 'z') ? item - 'a' + 1 : item - 'A' + 27;
    }
    release_input(input);
    return answer;
}

void main()
{
    printf("%d\n", part_one());
    printf("%d\n", part_two());
}