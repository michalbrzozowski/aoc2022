#include "..\utilities.h"

#define input_file "input"

int part_one()
{
    int answer = 0;
    char** input = aquire_input(input_file);

    for (int i = 0; i < list_length(input); ++i) {
        int x1, x2, y1, y2;
        sscanf_s(input[i], "%d-%d,%d-%d", &x1, &x2, &y1, &y2);

        if ((x1 >= y1 && x2 <= y2) || (y1 >= x1 && y2 <= x2))
            answer++;
    }
    release_input(input);
    return answer;
}

int part_two()
{
    int answer = 0;
    char** input = aquire_input(input_file);

    for (int i = 0; i < list_length(input); ++i) {
        int x1, x2, y1, y2;
        sscanf_s(input[i], "%d-%d,%d-%d", &x1, &x2, &y1, &y2);

        if ((y1 <= x1 && x1 <= y2 || y1 <= x2 && x2 <= y2) ||
            (x1 <= y1 && y1 <= x2 || x1 <= y2 && y2 <= x2))
            answer++;
    }
    release_input(input);
    return answer;
}

void main()
{
    printf("%d\n", part_one());
    printf("%d\n", part_two());
}