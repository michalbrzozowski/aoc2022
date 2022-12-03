#include "..\utilities.h"

#define input_file "input"

int part_one()
{
    int answer = 0;
    char** input = aquire_input(input_file);
    for (int i = 0; i < list_length(input); ++i) {
        int backpack_size = (int)list_length(input[i]);
        int middle = backpack_size / 2;

        char item = 0;
        for (int j = 0; j < middle; ++j) {
            for (int k = middle; k < backpack_size; ++k) {
                if (input[i][j] == input[i][k]) {
                    item = input[i][j];
                    answer += (item >= 'a' && item <= 'z') ? item - 'a' + 1 : item - 'A' + 27;
                    break;
                }
            }
            if (item > 0) break;
        }
    }

    release_input(input);
    return answer;
}

int part_two()
{
    int answer = 0;
    char** input = aquire_input(input_file);

    for (int it = 0; it < list_length(input); it += 3) {
        char badge = 0;
        for (int i = 0; i < list_length(input[it]); ++i) {
            for (int j = 0; j < list_length(input[it + 1]); ++j) {
                for (int k = 0; k < list_length(input[it + 2]); ++k) {
                    if (input[it][i] == input[it + 1][j] && input[it][i] == input[it + 2][k]) {
                        badge = input[it][i];
                        answer += (badge >= 'a' && badge <= 'z') ? badge - 'a' + 1 : badge - 'A' + 27;
                        break;
                    }
                }
                if (badge > 0) break;
            }
            if (badge > 0) break;
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