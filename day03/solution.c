#include "..\utilities.h"

#define input_file "input"

u64 find_set_bit_pos(u64 value)
{
    u64 pos, bit;
    pos = bit = 1;
    while (!(bit & value)) {
        bit = bit << 1;
        pos++;
    }
    return pos;
}

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
        char item = (char)find_set_bit_pos(compartment_one & compartment_two) + 'A' - 1;
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

        char item = (char)find_set_bit_pos(backpack[0] & backpack[1] & backpack[2]) + 'A' - 1;
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