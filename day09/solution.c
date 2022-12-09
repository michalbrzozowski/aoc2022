#include "..\utilities.h"

#define input_file "input"

#define grid_size 4096
static u32 grid[grid_size][grid_size];

typedef struct point {
    int x;
    int y;
} point;

point point_subtract(point first, point second)
{
    point result = { 0 };
    result.x = first.x - second.x;
    result.y = first.y - second.y;
    return result;
}

point move_tail(point head, point tail)
{
    point delta = point_subtract(head, tail);
    point move = { 0 };

    if (abs(delta.x) >= 2 || abs(delta.y) >= 2)
    {
        move.x = delta.x == 0 ? 0 : delta.x < 0 ? -1 : 1;
        move.y = delta.y == 0 ? 0 : delta.y < 0 ? -1 : 1;
    }
    return move;
}

int part_one()
{
    int answer = 0;
    char** input = aquire_input(input_file);

    // offset so we dont go out of bounds on the grid.
    point head = { .x = grid_size / 2, .y = grid_size / 2 };
    point tail = { .x = grid_size / 2, .y = grid_size / 2 };

    char direction;
    int steps;
    for (int i = 0; i < list_length(input); ++i) {
        sscanf_s(input[i], "%c %d", &direction, 1, &steps);

        for (int j = 0; j < steps; ++j) {
            switch (direction)
            {
                case 'U': { head.y += 1; } break;
                case 'D': { head.y -= 1; } break;
                case 'L': { head.x -= 1; } break;
                case 'R': { head.x += 1; } break;
            }

            point tail_step = move_tail(head, tail);
            tail.x += tail_step.x;
            tail.y += tail_step.y;

            grid[tail.y][tail.x]++;
        }
    }

    for (int i = 0; i < grid_size; ++i) {
        for (int j = 0; j < grid_size; ++j) {
            if (grid[i][j] > 0) 
                answer++;
        }
    }

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