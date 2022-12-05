#include <string.h>

#include "..\utilities.h"

#define input_file "input"

/*
            [J]             [B] [W]
            [T]     [W] [F] [R] [Z]
        [Q] [M]     [J] [R] [W] [H]
    [F] [L] [P]     [R] [N] [Z] [G]
[F] [M] [S] [Q]     [M] [P] [S] [C]
[L] [V] [R] [V] [W] [P] [C] [P] [J]
[M] [Z] [V] [S] [S] [V] [Q] [H] [M]
[W] [B] [H] [F] [L] [F] [J] [V] [B]
 1   2   3   4   5   6   7   8   9 
*/

char stacks[9][128] = { 0 };
int stack_cnt[9];

void parse_stacks(char **input)
{
    for (int i = 7; i >= 0; --i) {
        int stack = 0;
        for (int j = 1; j < 35; j += 4) {
            char crate = input[i][j];
            if (crate > 'A') {
                stacks[stack][stack_cnt[stack]++] = crate;
            }
            stack++;
        }
    }
}

char pop(int from)
{
    return stacks[from][--stack_cnt[from]];
}

void push(int value, int to)
{
    stacks[to][stack_cnt[to]++] = value;
}

int part_one()
{
    int answer = 0;
    char** input = aquire_input(input_file);
    parse_stacks(input);

    for (int i = 10; i < list_length(input); ++i) {
        int count, from, to;
        sscanf_s(input[i], "move %d from %d to %d", &count, &from, &to);

        assert(count > 0 && count < 128);
        assert(from > 0 && from < 10);
        assert(to > 0 && to < 10);

        for (int j = 0; j < count; ++j) {
            char val = pop(from - 1);
            push(val, to - 1);
        }
    }

    for (int i = 0; i < 9; ++i) {
        printf("%c", stacks[i][stack_cnt[i] - 1]);
    }
    printf("\n");

    release_input(input);
    return answer;
}

int part_two()
{
    int answer = 0;
    char** input = aquire_input(input_file);

    memset(stacks, 0, sizeof(stacks));
    memset(stack_cnt, 0, sizeof(stack_cnt));

    parse_stacks(input);

    for (int i = 10; i < list_length(input); ++i) {
        int count, from, to;
        sscanf_s(input[i], "move %d from %d to %d", &count, &from, &to);

        assert(count > 0 && count < 128);
        assert(from > 0 && from < 10);
        assert(to > 0 && to < 10);

        for (int j = 0; j < count; ++j) {
            char val = stacks[from  - 1][stack_cnt[from - 1] - (count - j)];
            push(val, to - 1);
        }
        stack_cnt[from - 1] -= count;
    }

    for (int i = 0; i < 9; ++i) {
        printf("%c", stacks[i][stack_cnt[i] - 1]);
    }
    printf("\n");

    release_input(input);
    return answer;
}

void main()
{
    printf("%d\n", part_one());
    printf("%d\n", part_two());
}