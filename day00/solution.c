#include "..\utilities.h"

#define input_file "sample"

int part_one()
{
    int answer = 0;
    char** input = aquire_input(input_file);

    release_input(input);
    return answer;
}

int part_two()
{
    int answer = 0;
    char** input = aquire_input(input_file);

    release_input(input);
    return answer;
}

void main()
{
    printf("%d\n", part_one());
    printf("%d\n", part_two());
}