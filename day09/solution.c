#include <string.h>

#include "..\utilities.h"

#define input_file "input"

#define grid_size 512
static u32 grid[grid_size][grid_size];

typedef struct point {
    int x;
    int y;
} point;

point point_subtract(point first, point second)
{
    return (point) {
        .x = first.x - second.x,
        .y = first.y - second.y
    };
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

int move_rope(int rope_length)
{
    memset(grid, 0, sizeof(grid));
    int answer = 0;
    char** input = aquire_input(input_file);

    point* rope = null_ptr;
    for (int i = 0; i < rope_length; ++i) {
        list_push(rope, (point) { .x = grid_size / 2, .y = grid_size / 2 });
    }

    char direction;
    int steps;
    for (int i = 0; i < list_length(input); ++i) {
        sscanf_s(input[i], "%c %d", &direction, 1, &steps);
        for (int j = 0; j < steps; ++j) {
            point *head = &rope[0];
            switch (direction)
            {
                case 'U': { head->y += 1; } break;
                case 'D': { head->y -= 1; } break;
                case 'L': { head->x -= 1; } break;
                case 'R': { head->x += 1; } break;
            }
            for (int k = 1; k < rope_length; ++k) {
                head = &rope[k - 1];
                point *tail = &rope[k];

                point tail_step = move_tail(*head, *tail);
                tail->x += tail_step.x;
                tail->y += tail_step.y;
            }
            grid[rope[rope_length - 1].y][rope[rope_length - 1].x]++;
        }
    }
    for (int i = 0; i < grid_size; ++i) {
        for (int j = 0; j < grid_size; ++j) {
            if (grid[i][j] > 0) 
                answer++;
        }
    }
    list_free(rope);
    release_input(input);
    return answer;
}

int part_one()
{
    return move_rope(2);
}

int part_two()
{
    return move_rope(10);
}

void main()
{
    printf("%d\n", part_one());
    printf("%d\n", part_two());
}