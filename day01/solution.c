#include "..\utilities.h"

#define input_file "input"

int part_one()
{
    int answer = 0;
    char** input = aquire_input(input_file);

    int elf = 0;
    int kcal = 0;

    int rows = (int)list_length(input);
    for (int row = 0; row < rows; ++row) {
        kcal = atoi(input[row]);
        if (kcal == 0) {
            answer = __max(answer, elf);
            elf = 0;
        }
        else {
            elf += kcal;
        }
    }

    release_input(input);
    return answer;
}

int compare (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}

int part_two()
{
    int answer = 0;
    char** input = aquire_input(input_file);

    int elf = 0;
    int kcal = 0;
    int *elfs = null_ptr;

    int rows = (int)list_length(input);
    for (int row = 0; row < rows; ++row) {
        kcal = atoi(input[row]);
        if (kcal == 0) {
            list_push(elfs, elf);
            elf = 0;
        }
        else {
            elf += kcal;
        }
    }
    list_push(elfs, elf);

    int list_len = (int)list_length(elfs) - 1;
    qsort(elfs, list_length(elfs), sizeof(int), compare);
    for (int i = list_len; i > list_len - 3; --i)
        answer += elfs[i];

    list_free(elfs);
    release_input(input);
    return answer;

}

void main()
{
    printf("%d\n", part_one());
    printf("%d\n", part_two());
}